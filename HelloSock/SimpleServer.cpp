#include "SimpleServer.h"

SimpleServer::SimpleServer()
{
	_sock = INVALID_SOCKET;
	FD_ZERO(&read_set);
	_recvCount = 0;
	_msgCount = 0;
	_clientCount = 0;

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
		_thread.Close();
		for (auto ser : _cellServers)
		{
			delete ser;
		}
		_cellServers.clear();
#ifdef WIN32
			closesocket(_sock);
			WSACleanup();
#else
			close(sock);
#endif // WIN32
			_sock = INVALID_SOCKET;
	}
	for (auto it : _cellServers) {
		delete it;
	}

}

int SimpleServer::onRun(CellThread* pthread)
{
	while (pthread->isRun()) {
		FD_SET(_sock, &read_set);
		time4msg();
		fd_set tmp_set = read_set;
		timeval t = { 0, 0 };
		int num = select(_sock + 1, &tmp_set, NULL, NULL, &t);
		if (num < 0) {
			printf("select 调用失败...\n");
			Close();
			return -1;
		}
		if (FD_ISSET(_sock, &tmp_set)) {
			if (-1 == Accept()) {
				return -1;
			}
			/*NewUser newuser;
			newuser.result = 1;
			newuser.sock = _cli_sock;
			for (int i = 0; i < client_sockfd.size(); ++i) {
				if (FD_ISSET(client_sockfd[i]->_cli_sock, &read_set) && client_sockfd[i]->_cli_sock != _sock) {
					send(client_sockfd[i]->_cli_sock, (const char*)&newuser, newuser.data_length, 0);
				}
			}*/
		}
	}
	return -1;
}
bool SimpleServer::isRun()
{
	return _sock != INVALID_SOCKET;
}

int SimpleServer::Accept()
{
	struct sockaddr_in cli_addr;
	SOCKET cli_fd = INVALID_SOCKET;
	int addlen = sizeof(struct sockaddr);
	cli_fd = accept(_sock, (struct sockaddr *)&cli_addr, &addlen);
	if (cli_fd == INVALID_SOCKET) {
		printf("accept fail");
		return -1;
	}
	//printf("new client 建立, ip = %s, port = %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
	std::shared_ptr<CellClient> cli_ptr(new CellClient(cli_fd));
	addClientToCellServer(cli_ptr);
	return 0;
}

void SimpleServer::time4msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0)
	{
		printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,recv<%d>,msg<%d>\n", _cellServers.size(), t1, _sock, (int)_clientCount, (int)(_recvCount / t1), (int)(_msgCount / t1));
		_recvCount = 0;
		_msgCount = 0;
		_tTime.update();
	}
}

void SimpleServer::addClientToCellServer(ClientSocktPtr pClient)
{
	CellServer* pMinServer = _cellServers[0];
	for (auto& pCellServer : _cellServers) {
		if (pMinServer->getCount() > pCellServer->getCount()) {
			pMinServer = pCellServer;
		}
	}
	pMinServer->addClient(pClient);
	OnNetJoin(pClient);

}

void SimpleServer::Start(int pthtread_cnt)
{
	for (int i = 0; i < pthtread_cnt; ++i) {
		auto ser = new CellServer(_sock);
		_cellServers.push_back(ser);
		//注册网络事件接受对象
		ser->setEventObj(this);
		//启动消息处理线程
		ser->start();
	}
	_thread.Start(nullptr, [this](CellThread* pthread) {
		onRun(pthread);
	}, nullptr);
}



void SimpleServer::OnNetJoin(ClientSocktPtr pClient)
{
	_clientCount++;
}
//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void SimpleServer::OnNetLeave(ClientSocktPtr pClient)
{
	_clientCount--;
}
//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void SimpleServer::OnNetMsg(CellServer* pCellServer, ClientSocktPtr pClient, DataHeader* header)
{
	_msgCount++;
}

void SimpleServer::OnNetRecv(ClientSocktPtr pClient)
{
	_recvCount++;
}


