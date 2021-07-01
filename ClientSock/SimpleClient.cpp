#include "SimpleClient.hpp"

SimpleClient::SimpleClient()
{
	_sock = INVALID_SOCKET;
	FD_ZERO(&read_set);
}

SOCKET SimpleClient::InitSocket()
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2); //typedef unsigned short WORD 
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	FD_SET(_sock, &read_set);
	return _sock;
}

void SimpleClient::Connect(std::string ip_addr, int port)
{
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);

#ifdef _WIN32
	ser_addr.sin_addr.S_un.S_addr = inet_addr(ip_addr.c_str());
#else
	ser_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
#endif

	int ret = connect(_sock, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
}

void SimpleClient::close()
{
if (isRun()) {
#ifdef WIN32
		closesocket(_sock);
		WSACleanup();
#else
		close(_sock);
#endif // WIN32
		_sock = INVALID_SOCKET;
}

}


int SimpleClient::onRun()
{
	
	if (isRun()) {
		fd_set tmp_set = read_set;
		timeval t = { 0, 0 };
		int num = select(_sock + 1, &tmp_set, NULL, NULL, &t);
		if (num < 0) {
			close();
			printf("与服务器连接结束...\n");
			return -1;
		}
		if (FD_ISSET(_sock, &tmp_set)) {
			if (-1 == recvData()) {
				printf("<socket %d>与服务器连接结束...\n", _sock);
				return -1;
			}
		}
		return 0;
	}
	return -1;
}

bool SimpleClient::isRun()
{
	return _sock != INVALID_SOCKET;
}

int SimpleClient::recvData()
{
	char recv_buf[4096];
	memset(&recv_buf, 0, sizeof(recv_buf));
	int len = recv(_sock, recv_buf, sizeof(DataHeader), 0);
	if (len <= 0) {
		return -1;
	}
	DataHeader* dataheader = (DataHeader *)recv_buf;
	len = recv(_sock, recv_buf + sizeof(struct DataHeader), dataheader->data_length - sizeof(struct DataHeader), 0);

	onNetMsg(dataheader);
	return 0;
}

int SimpleClient::onNetMsg(DataHeader* dataheader)
{
	if (CMD_LOGIN_RESULT == dataheader->cmd) {
		LoginResult* result = (LoginResult *)dataheader;
		printf("收到登陆反馈结果: %d， 命令长度为%d\n", result->result, result->data_length);
	}
	else if (CMD_LOGOUT_RESULT == dataheader->cmd) {
		LogoutResult* result = (LogoutResult *)dataheader;
		printf("收到退出反馈结果: %d， 命令长度为%d\n", result->result, result->data_length);
	}
	else if (CMD_NEW_USER == dataheader->cmd) {
		NewUser* result = (NewUser *)dataheader;
		if (result->result) {
			printf("新用户登录<%d>， 命令长度为%d\n", result->sock, result->data_length);
		}
		else {
			printf("用户退出<%d>， 命令长度为%d\n", result->sock, result->data_length);
		}	
	}
	else {
		printf("无法识别命令...\n");
	}

	return 0;
}

int SimpleClient::sendData(DataHeader &dataheader)
{
	if (isRun()) {
		send(_sock, (const char*)&dataheader, dataheader.data_length, 0);
	}
	return 0;
}





