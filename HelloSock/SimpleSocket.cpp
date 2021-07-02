#include "SimpleSocket.h"

SimpleServer::SimpleServer()
{
	_sock = INVALID_SOCKET;
	FD_ZERO(&read_set);
}

SOCKET SimpleServer::InitSocket(int port)
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2); //typedef unsigned short WORD 
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("建立socket失败");
		return INVALID_SOCKET;
	}
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
#ifdef _WIN32
	ser_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	ser_addr.sin_addr.s_addr = INADDR_ANY;
#endif

	if (bind(_sock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		printf("绑定地址失败");
		return -1;
	}

	if (listen(_sock, 5) == SOCKET_ERROR) {
		printf("监听失败");
		return 0;
	}
	printf("初始化socket成功, <socket %d>\n", _sock);
	return _sock;
}

void SimpleServer::Close()
{
if (isRun()) {
#ifdef WIN32
		closesocket(_sock);
		for (auto& recvBuf : client_sockfd) {
			closesocket(recvBuf->_cli_sock);
			delete recvBuf;
}
#else
		for (auto& recvBuf : client_sockfd) {
			close(recvBuf->_cli_sock);
			delete recvBuf;
		}
		close(sock);
#endif // WIN32
		_sock = INVALID_SOCKET;
		client_sockfd.clear();

}

}

int SimpleServer::onRun()
{
	if (isRun()) {
		max_fd = _sock;
		FD_SET(_sock, &read_set);
		while (true) {
			fd_set tmp_set = read_set;
			timeval t = { 0, 0 };
			int num = select(max_fd + 1, &tmp_set, NULL, NULL, &t);
			if (num < 0) {
				printf("select 调用失败...\n");
				return -1;
			}
			if (FD_ISSET(_sock, &tmp_set)) {
				struct sockaddr_in cli_addr;
				SOCKET cli_fd = INVALID_SOCKET;
				int addlen = sizeof(struct sockaddr);
				char recv_buf[BUFSIZ];
				_cli_sock = accept(_sock, (struct sockaddr *)&cli_addr, &addlen);
				if (_cli_sock == INVALID_SOCKET) {
					printf("accept fail");
					continue;
				}
				printf("new client 建立, ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
				FD_SET(_cli_sock, &read_set);
				NewUser newuser;
				newuser.result = 1;
				newuser.sock = _cli_sock;
				for (int i = 0; i < client_sockfd.size(); ++i) {
					if (FD_ISSET(client_sockfd[i]->_cli_sock, &read_set) && client_sockfd[i]->_cli_sock != _sock) {
						send(client_sockfd[i]->_cli_sock, (const char*)&newuser, newuser.data_length, 0);
					}
				}
				client_sockfd.push_back(new RecvBuf(_cli_sock));
				max_fd = max_fd < _cli_sock ? _cli_sock : max_fd;
				
			}

			for (int i = 0; i < client_sockfd.size(); ++i) {
				if (FD_ISSET(client_sockfd[i]->_cli_sock, &tmp_set) && client_sockfd[i]->_cli_sock != _sock) {
					recvData(client_sockfd[i]);
				}
			}
		}
		return 0;
	}
	return -1;
}

bool SimpleServer::isRun()
{
	return _sock != INVALID_SOCKET;
}

int SimpleServer::recvData(RecvBuf* recvBuf)
{
	int len = recv(recvBuf->_cli_sock, (char*)recvBuf->_recvBuf1, sizeof(RECV_BUF_SIZE), 0);
	if (len <= 0) {
		printf("client<socket %d> 退出...\n", recvBuf->_cli_sock);
		NewUser newuser;
		newuser.result = 0;
		newuser.sock = recvBuf->_cli_sock;
		for (int i = 0; i < client_sockfd.size(); ++i) {
			if (FD_ISSET(client_sockfd[i]->_cli_sock, &read_set) && client_sockfd[i]->_cli_sock != _sock) {
				send(client_sockfd[i]->_cli_sock, (const char*)&newuser, newuser.data_length, 0);
			}
		}
		FD_CLR(recvBuf->_cli_sock, &read_set);
		auto it = find(client_sockfd.begin(), client_sockfd.end(), recvBuf);
		delete *it;
		client_sockfd.erase(it);
		
		
		return 0;
	}
	memcpy(recvBuf->_szMsgBuf2 + recvBuf->_lastPos, recvBuf->_recvBuf1, len);
	recvBuf->_lastPos += len;
	while (recvBuf->_lastPos >= sizeof(struct DataHeader)) {
		DataHeader* dataheader = (DataHeader *)recvBuf->_szMsgBuf2;

		if (recvBuf->_lastPos >= dataheader->data_length) {

			int siz = recvBuf->_lastPos - dataheader->data_length;
			onNetMsg(*dataheader, recvBuf->_cli_sock);
			memcpy(recvBuf->_szMsgBuf2, recvBuf->_szMsgBuf2 + dataheader->data_length, siz);
			recvBuf->_lastPos = siz;
		}
		else {
			break;
		}
	}
	return 0;
}

int SimpleServer::onNetMsg(DataHeader& dataheader, SOCKET cliSock) {

	
	switch (dataheader.cmd) {
	case(CMD_LOGIN): {
		Login* login = (Login*)&dataheader;
		printf("usrname: %s, password: %s\n", login->username, login->password);
		LoginResult login_result;
		send(cliSock, (const char*)&login_result, login_result.data_length, 0);
	}
						break;
	case(CMD_LOGOUT): {
		Logout* logout = (Logout*)&dataheader;
		LogoutResult logout_result;
		send(cliSock, (const char*)&logout_result, logout_result.data_length, 0);
		printf("username: %s, length : %d\n", logout->username, logout->data_length);
	}
						break;
	default:
		printf("未知命令...");
		break;
	}
	return 0;

	
}
