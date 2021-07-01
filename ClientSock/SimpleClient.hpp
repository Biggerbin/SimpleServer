#ifndef H_SIMPLE_CLIENT
#define H_SIMPLE_CLIENT

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

class SimpleClient
{
public:
	SimpleClient();

	//初始化socket
	SOCKET InitSocket();
	//连接服务器
	void Connect(std::string ip_addr, int port);
	//关闭链接
	void close();

	//运行
	int onRun();
	//判断是否运行
	bool isRun();

	int recvData();

	int onNetMsg(DataHeader* dataheader);
	int sendData(DataHeader& dataheader);
private:
	SOCKET _sock;
	fd_set read_set;
};

#endif // !1