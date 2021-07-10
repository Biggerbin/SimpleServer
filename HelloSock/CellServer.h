#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "CellCommon.h"
#include "CellClient.h"
#include "CellThread.h"

#include "CellCommon.h"
#include "CellClient.h"
#include "CellServer.h"
#include "CellTask.h"
#include "Timestamp.h"
#include <map>
#include <vector>

typedef std::shared_ptr<CellClient> ClientSocktPtr;

class CellServer;
class INetEvent
{
public:
	//纯虚函数
	//客户端加入事件
	virtual void OnNetJoin(ClientSocktPtr pClient) = 0;
	//客户端离开事件
	virtual void OnNetLeave(ClientSocktPtr pClient) = 0;
	//客户端消息事件
	virtual void OnNetMsg(CellServer*, ClientSocktPtr pClient, DataHeader* header) = 0;
	//recv事件
	virtual void OnNetRecv(ClientSocktPtr pClient) = 0;
private:

};

typedef std::shared_ptr<CellClient> ClientSocktPtr;

class CellServer
{
public:
	CellServer(SOCKET& sock);
	~CellServer();

	int onRun(CellThread* pthread);
	void checkTime();
	int recvData(ClientSocktPtr recvBuf);
	virtual void OnNetMsg(ClientSocktPtr pClient, DataHeader* header);
	int getCount();
	void addClient(ClientSocktPtr pClient);
	void setEventObj(INetEvent* event);
	void start();
	void addSendTask(ClientSocktPtr pClient, dataHeaderPtr header);
	void Close();

private:
	std::map<SOCKET, ClientSocktPtr> _client;
	std::vector<ClientSocktPtr> _clientBuf;
	std::mutex _mutex_client_buf;
	std::mutex _mutex_client;
	SOCKET _sock;
	SOCKET max_fd = 0;
	fd_set read_set;
	INetEvent* _pNetEvent;
	CellTaskServer _taskServer;
	time_t _oldTime;
	CellThread _thread;

};

#endif // !_CELLSERVER_H_


