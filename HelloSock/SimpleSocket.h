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

#define RECV_BUF_SIZE 10240

struct RecvBuf {
public:
	RecvBuf(SOCKET cliSock) {
		_recvBuf1[RECV_BUF_SIZE] = {};
		_szMsgBuf2[RECV_BUF_SIZE * 10] = {};
		_lastPos = 0;
		_cli_sock = cliSock;
	}


	//接收缓冲区
	char _recvBuf1[RECV_BUF_SIZE];
	//第二缓冲区
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;
	int _cli_sock;
};

class SimpleServer
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

	int recvData(RecvBuf* recvBuf);

	int onNetMsg(DataHeader& dataheader, SOCKET cliSock);
	int sendData(DataHeader& dataheader);
private:
	SOCKET _sock;
	SOCKET _cli_sock;
	SOCKET max_fd;
	fd_set read_set;
	std::vector<RecvBuf*> client_sockfd;
};

#endif // !1