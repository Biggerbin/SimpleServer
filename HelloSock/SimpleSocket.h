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


#define RECV_BUF_SIZE 10240

struct ClientSocket {
public:
	ClientSocket(SOCKET cliSock) {
		_recvBuf1[RECV_BUF_SIZE] = {};
		_szMsgBuf2[RECV_BUF_SIZE * 10] = {};
		_lastPos = 0;
		_cli_sock = cliSock;
	}
	
	int SendData(DataHeader* header)
	{
		if (header)
		{
			return send(_cli_sock, (const char*)header, header->data_length, 0);
		}
		return SOCKET_ERROR;
	}
	//接收缓冲区
	char _recvBuf1[RECV_BUF_SIZE];
	//第二缓冲区
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;
	int _cli_sock;
};

//网络事件接口
class INetEvent
{
public:
	//纯虚函数
	//客户端加入事件
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//客户端离开事件
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//客户端消息事件
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header) = 0;
	//recv事件
	virtual void OnNetRecv(ClientSocket* pClient) = 0;
private:

};

class CellServer
{
public:
	CellServer(SOCKET sock);
	~CellServer();

	int onRun();
	int recvData(ClientSocket* recvBuf);
	int getCount();
	//判断是否运行
	bool isRun();
	void addClient(ClientSocket* pClient);
	void setEventObj(INetEvent* event);
	void start();
	void Close();

private:
	std::map<SOCKET ,ClientSocket*> _client;
	std::vector<ClientSocket*> _clientBuf;
	std::mutex _mutex_client_buf;
	std::mutex _mutex_client;
	std::thread _thread;
	SOCKET _sock;
	SOCKET max_fd = 0;
	fd_set read_set;
	INetEvent* _pNetEvent;

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
	void addClientToCellServer(ClientSocket* pClient);
	void Start(int pthtread_cnt);
	virtual void OnNetJoin(ClientSocket* pClient);
	//客户端离开事件
	virtual void OnNetLeave(ClientSocket* pClient);
	//客户端消息事件
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header);
	//recv事件
	virtual void OnNetRecv(ClientSocket* pClient);

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