#ifndef _INET_EVENT_H_
#define _INET_EVENT_H_

#include "CellCommon.h"
#include "CellClient.h"
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

#endif // !_INET_EVENT_H_


#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "CellCommon.h"
#include "CellClient.h"
#include "CellServer.h"
#include "CellTask.h"

#include <map>
#include <vector>

typedef std::shared_ptr<CellClient> ClientSocktPtr;

class CellServer
{
public:
	CellServer(SOCKET sock);
	~CellServer();

	int onRun();
	int recvData(ClientSocktPtr recvBuf);
	virtual void OnNetMsg(ClientSocktPtr pClient, DataHeader* header);
	int getCount();
	//判断是否运行
	bool isRun();
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
	std::thread _thread;
	SOCKET _sock;
	SOCKET max_fd = 0;
	fd_set read_set;
	INetEvent* _pNetEvent;
	CellTaskServer _taskServer;

};

#endif // !_CELLSERVER_H_


