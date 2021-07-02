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

#define RECV_BUF_SIZE 10240

struct RecvBuf {
	//���ջ�����
	char _recvBuf1[RECV_BUF_SIZE] = {};
	//�ڶ�������
	char _szMsgBuf2[RECV_BUF_SIZE * 10] = {};
	int _lastPos = 0;
};

class SimpleClient
{
public:
	SimpleClient();

	//��ʼ��socket
	SOCKET InitSocket();
	//���ӷ�����
	void Connect(std::string ip_addr, int port);
	//�ر�����
	void close();

	//����
	int onRun();
	//�ж��Ƿ�����
	bool isRun();

	int recvData();

	int onNetMsg(DataHeader* dataheader);
	int sendData(DataHeader& dataheader);
private:
	SOCKET _sock;
	fd_set read_set;
	//���ջ�����
	RecvBuf* _recvBuf;
};

#endif // !1