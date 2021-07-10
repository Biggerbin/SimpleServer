#ifndef H_SIMPLE_SERVER
#define H_SIMPLE_SERVER

#include "CellCommon.h"
#include "CellClient.h"
#include "CellServer.h"
#include "CellTask.h"
#include "CellThread.h"

#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <Timestamp.h>
#include <atomic>
#include <stdio.h>
#include <string>

typedef std::shared_ptr<CellClient> ClientSocktPtr;
class CellServer;

class SimpleServer : public INetEvent
{
public:
	SimpleServer();

	//初始化socket、绑定bind、监听listen
	SOCKET InitSocket(int port);
	//accept

	//关闭链接
	void Close();

	//运行
	int onRun(CellThread* pthread);
	//判断是否运行
	bool isRun();
	int Accept();
	void time4msg();
	void addClientToCellServer(ClientSocktPtr pClient);
	void Start(int pthtread_cnt);
	virtual void OnNetJoin(ClientSocktPtr pClient);
	//客户端离开事件
	virtual void OnNetLeave(ClientSocktPtr pClient);
	//客户端消息事件
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocktPtr pClient, DataHeader* header);
	//recv事件
	virtual void OnNetRecv(ClientSocktPtr pClient);

private:
	SOCKET _sock;
	SOCKET _cli_sock;
	SOCKET max_fd;
	fd_set read_set;
	std::vector<CellServer*> _cellServers;
protected:
	//SOCKET recv计数
	std::atomic_int _recvCount;
	//收到消息计数
	std::atomic_int _msgCount;
	//客户端计数
	std::atomic_int _clientCount;
	Timestamp _tTime;
	CellThread _thread;
};

#endif // !1