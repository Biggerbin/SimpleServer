#ifndef H_SIMPLE_SERVER
#define H_SIMPLE_SERVER

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET SOCKET(~0)
#define SOCKET_ERROR      (-1)

#endif

#include <stdio.h>
#include <string>
#include <DataHeader.hpp>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <Timestamp.h>
#include <atomic>
#include "CellTask.h"


#define RECV_BUF_SIZE 10240
#define SEND_BUFF_SZIE 10240

class CellServer;
typedef std::shared_ptr<DataHeader> dataHeaderPtr;
struct ClientSocket {
public:
	ClientSocket(SOCKET cliSock) {
		_recvBuf1[RECV_BUF_SIZE] = {};
		_szMsgBuf2[RECV_BUF_SIZE * 10] = {};
		_lastPos = 0;
		_cli_sock = cliSock;
	}
	
	int SendData(dataHeaderPtr header)
	{
		int ret = SOCKET_ERROR;
		int nSendLen = header->data_length;
		const char* pSendData = (char*)header.get();
		while (true) {
			if (_lastSendPos + nSendLen >= SEND_BUFF_SZIE) {
				int nCopyLen = SEND_BUFF_SZIE - _lastSendPos;
				memcpy(_szSendBuf + _lastSendPos, pSendData, nCopyLen);
				pSendData += nCopyLen;
				nSendLen -= nCopyLen;
				ret = send(_cli_sock, _szSendBuf, SEND_BUFF_SZIE, 0);
				_lastSendPos = 0;
				if (SOCKET_ERROR == ret) {
					return ret;
				}
			}
			else {
				memcpy(_szSendBuf + _lastSendPos, pSendData, nSendLen);
				_lastSendPos += nSendLen;
				break;
			}
		}
		
	}
	
	int _cli_sock;
	//接收缓冲区
	char _recvBuf1[RECV_BUF_SIZE];
	//第二缓冲区
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;

	//第二缓冲区 发送缓冲区
	char _szSendBuf[SEND_BUFF_SZIE];
	//发送缓冲区的数据尾部位置
	int _lastSendPos = 0;
	
};
typedef std::shared_ptr<ClientSocket> ClientSocktPtr;
//网络事件接口
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

class CellSendMsg2ClientTask :public CellTask
{
	ClientSocktPtr _pClient;
	dataHeaderPtr _pHeader;
public:
	CellSendMsg2ClientTask(ClientSocktPtr pClient, dataHeaderPtr header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	//执行任务
	void doTask()
	{
		_pClient->SendData(_pHeader);
	}
};

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
	std::map<SOCKET ,ClientSocktPtr> _client;
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
	int onRun();
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
};

#endif // !1