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

class SimpleServer
{
public:
	SimpleServer();

	//��ʼ��socket����bind������listen
	SOCKET InitSocket(int port);
	//accept

	//�ر�����
	void close(SOCKET sock);

	//����
	int onRun();
	//�ж��Ƿ�����
	bool isRun();

	int recvData();

	int onNetMsg(DataHeader& dataheader);
	int sendData(DataHeader& dataheader);
	void process(SOCKET sock_fd);
private:
	SOCKET _sock;
	SOCKET _cli_sock;
	SOCKET max_fd;
	fd_set read_set;
	std::vector<int> client_sockfd;
};

#endif // !1