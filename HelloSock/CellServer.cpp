#include "CellServer.h"


CellServer::CellServer(SOCKET& sock)
{
	_sock = sock;
	_pNetEvent = nullptr;
	_oldTime = Timestamp::getNowInMilliSec();

}

CellServer::~CellServer()
{
	printf("cellserver...1\n");
	Close();
	printf("cellserver...2\n");

}

int CellServer::onRun(CellThread* pthread)
{
	FD_ZERO(&read_set);

	while (pthread->isRun()){
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
			_oldTime = Timestamp::getNowInMilliSec();
			continue;
		}
		FD_ZERO(&read_set);
		for (auto iter : _client)
		{
			if (max_fd < iter.first)
			{
				max_fd = iter.first;
			}
			FD_SET(iter.first, &read_set);
		}

		fd_set tmp_set = read_set;
		timeval t = { 0, 0 };
		int num = select(max_fd + 1, &tmp_set, NULL, NULL, &t);
		if (num < 0) {
			printf("%d", tmp_set);
			printf("select<pthread = %d>...%d 调用失败...\n", std::this_thread::get_id(), max_fd);
			Close();
			return -1;
		}
		
		if (num == 0) continue;
		std::vector<SOCKET> temp;
		for (auto& iter : _client) {
			if (FD_ISSET(iter.first, &tmp_set)) {
				//iter.second->resetDTHeart();
				if (-1 == recvData(iter.second)) {
					if (_pNetEvent)
						_pNetEvent->OnNetLeave(iter.second);
					temp.push_back(iter.first);
				}
			}
		}
		for (auto sock : temp)
		{
			std::lock_guard<std::mutex> lg(_mutex_client);
			_client.erase(sock);
			
		}
		//心跳
		checkTime();
	}
	return 0;
}

void CellServer::checkTime()
{
	//当前时间戳
	auto nowTime = Timestamp::getNowInMilliSec();
	auto dt = nowTime - _oldTime;
	_oldTime = nowTime;

	for (auto iter = _client.begin(); iter != _client.end(); )
	{
		if (iter->second->checkHeart(dt))
		{
			if (_pNetEvent)
				_pNetEvent->OnNetLeave(iter->second);
			iter = _client.erase(iter);
			continue;
		}
		iter->second->checkSend(dt);
		iter++;
	}
}

int CellServer::recvData(ClientSocktPtr pClient)
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
	pClient->resetDTHeart();
	memcpy(pClient->_szMsgBuf2 + pClient->_lastPos, pClient->_recvBuf1, len);
	pClient->_lastPos += len;

	while (pClient->_lastPos >= sizeof(struct DataHeader)) {
		DataHeader* dataheader = (DataHeader*)(pClient->_szMsgBuf2);

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
void CellServer::OnNetMsg(ClientSocktPtr pClient, DataHeader* header) {
	_pNetEvent->OnNetMsg(this, pClient, header);
}
int CellServer::getCount()
{
	return _client.size() + _clientBuf.size();
}

void CellServer::addClient(ClientSocktPtr pClient)
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
	_taskServer.start();
	_thread.Start(nullptr, 
		[this](CellThread* pthread) {
			onRun(pthread);},
		nullptr);
}

void CellServer::addSendTask(ClientSocktPtr pClient, dataHeaderPtr header)
{
	_taskServer.addTask([pClient, header]() {
		pClient->SendData(header);
	});
}

void CellServer::Close()
{

	_taskServer.close();
	_thread.Close();


}