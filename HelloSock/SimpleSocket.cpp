#include "SimpleSocket.h"

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
#ifdef WIN32
			closesocket(_sock);
#else
			close(sock);
#endif // WIN32
			_sock = INVALID_SOCKET;
	}

}

int SimpleServer::onRun()
{
	FD_SET(_sock, &read_set);
	while (isRun()) {
		time4msg();
		fd_set tmp_set = read_set;
		timeval t = { 1, 0 };
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
			return 0;
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
	addClientToCellServer(new ClientSocket(cli_fd));
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

void SimpleServer::addClientToCellServer(ClientSocket * pClient)
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
}



void SimpleServer::OnNetJoin(ClientSocket* pClient)
{
	//_clientCount++;
}
//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void SimpleServer::OnNetLeave(ClientSocket* pClient)
{
	//_clientCount--;
}
//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void SimpleServer::OnNetMsg(CellServer* pCellServer, ClientSocket* pClient, DataHeader* header)
{
	_msgCount++;
}

void SimpleServer::OnNetRecv(ClientSocket * pClient)
{
	_recvCount++;
}


CellServer::CellServer(SOCKET sock)
{
	_sock = sock;
	_pNetEvent = nullptr;
}

CellServer::~CellServer()
{
	Close();
	_sock = INVALID_SOCKET;
}

int CellServer::onRun()
{
	FD_ZERO(&read_set);
	
	while (isRun()) {
		if (!_clientBuf.empty()) {
			std::lock_guard<std::mutex> lg(_mutex_client);
			for (auto iter : _clientBuf) {
				_client[iter->_cli_sock] = iter;
				
			}
			_clientBuf.clear();
		}
		if (_client.empty()) {
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		for (auto iter : _client)
		{
			if (max_fd < iter.first)
			{
				
				max_fd = iter.first;
				FD_SET(iter.first, &read_set);
			}
		}
		
		fd_set tmp_set = read_set;
		timeval t = { 0, 0 };
		int num = select(max_fd + 1, &tmp_set, NULL, NULL, &t);
		if (num < 0) {
			printf("select<pthread = %d>...%d 调用失败...\n", std::this_thread::get_id(), max_fd);
			Close();
			return -1;
		}
		if (num == 0) continue;
		for (auto& iter : _client) {
			if (FD_ISSET(iter.first, &tmp_set)) {
				if (-1 == recvData(iter.second)) {
					if (_pNetEvent)
						_pNetEvent->OnNetLeave(iter.second);
					_client.erase(iter.first);
					FD_CLR(iter.first, &read_set);
				}
			}
		}
	}
	return 0;
}

int CellServer::recvData(ClientSocket * pClient)
{
	_pNetEvent->OnNetRecv(pClient);
	int len = recv(pClient->_cli_sock, (char*)pClient->_recvBuf1, sizeof(RECV_BUF_SIZE), 0);
	if (len <= 0) {
		printf("client<socket %d> 退出...\n", pClient->_cli_sock);
		/*NewUser newuser;
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
		client_sockfd.erase(it);*/
		return -1;
	}
	memcpy(pClient->_szMsgBuf2 + pClient->_lastPos, pClient->_recvBuf1, len);
	pClient->_lastPos += len;
	
	while (pClient->_lastPos >= sizeof(struct DataHeader)) {
		DataHeader* dataheader = (DataHeader *)pClient->_szMsgBuf2;

		if (pClient->_lastPos >= dataheader->data_length) {

			int siz = pClient->_lastPos - dataheader->data_length;
			OnNetMsg(pClient, dataheader);
			memcpy(pClient->_szMsgBuf2, pClient->_szMsgBuf2 + dataheader->data_length, siz);
			pClient->_lastPos = siz;
		}
		else {
			break;
		}
	}
	return 0;
}
void CellServer::OnNetMsg(ClientSocket* pClient, DataHeader* header) {
	_pNetEvent->OnNetMsg(this, pClient, header);
}
int CellServer::getCount()
{
	return _client.size() + _clientBuf.size();
}

bool CellServer::isRun()
{
	return _sock != INVALID_SOCKET;
}

void CellServer::addClient(ClientSocket * pClient)
{
	
	std::lock_guard<std::mutex> lg(_mutex_client_buf);
	_clientBuf.push_back(pClient);
}

void CellServer::setEventObj(INetEvent * event)
{
	_pNetEvent = event;
}

void CellServer::start()
{
	_thread = std::thread(std::mem_fn(&CellServer::onRun), this);
	_taskServer.start();
}

void CellServer::addSendTask(ClientSocket* pClient, DataHeader* header)
{
	CellSendMsg2ClientTask* task = new CellSendMsg2ClientTask(pClient, header);
	_taskServer.addTasks(task);
}

void CellServer::Close()
{
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		for (auto iter : _client)
		{
			closesocket(iter.first);
			delete iter.second;
		}
		//关闭套节字closesocket
		closesocket(_sock);
#else
		for (auto iter : _clients)
		{
			close(iter.first);
			delete iter.second;
		}
		//关闭套节字closesocket
		close(_sock);
#endif
		_client.clear();
	}
}

