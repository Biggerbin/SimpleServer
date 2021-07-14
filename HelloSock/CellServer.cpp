#include "CellServer.h"


CellServer::CellServer(int id)
{
	_id = id;
	_pNetEvent = nullptr;
	_oldTime = Timestamp::getNowInMilliSec();
	_taskServer.serverId = id;

}

CellServer::~CellServer()
{
	CellLog::Info("CELLServer%d.~CELLServer exit begin\n", _id);
	Close();
	CellLog::Info("CELLServer%d.~CELLServer exit end\n", _id);
}

int CellServer::onRun(CellThread* pthread)
{
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
		FD_ZERO(&rw_set);
		for (auto iter : _client)
		{
			if (max_fd < iter.first)
			{
				max_fd = iter.first;
			}
			FD_SET(iter.first, &rw_set);
		}

		fd_set read_set = rw_set;
		fd_set write_set = rw_set;
		timeval t = {0, 1};
		int num = select(max_fd + 1, &read_set, &write_set, NULL, &t);
		if (num < 0) {
			CellLog::Info("CELLServer%d.OnRun.select Error exit\n", _id);
			Close();
			return -1;
		}
		
		if (num == 0) continue;
		readData(read_set);
		writeData(write_set);
		//心跳
		checkTime();
	}
	return 0;
}

void CellServer::readData(fd_set read_set)
{
	std::vector<SOCKET> temp;
	for (auto& iter : _client) {
		if (FD_ISSET(iter.first, &read_set)) {
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
}

void CellServer::writeData(fd_set write_set)
{
	std::vector<SOCKET> temp;
	for (auto& iter : _client) {
		if (FD_ISSET(iter.first, &write_set)) {
			if (-1 == iter.second->SendReal()) {
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
		//iter->second->checkSend(dt);
		iter++;
	}
}

int CellServer::recvData(ClientSocktPtr pClient)
{
	
	int len = pClient->recvData();
	if (len <= 0) {
		CellLog::Info("client<socket %d> 退出...\n", pClient->_cli_sock);
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
	_pNetEvent->OnNetRecv(pClient);
	while (pClient->hasMsg()) {
		OnNetMsg(pClient, pClient->front_msg());
		pClient->pop_front_msg();
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
	CellLog::Info("CELLServer%d.Close begin\n", _id);
	_taskServer.close();
	_thread.Close();
	CellLog::Info("CELLServer%d.Close end\n", _id);
}