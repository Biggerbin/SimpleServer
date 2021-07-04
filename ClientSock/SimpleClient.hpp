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
//#include <chrono>
#define RECV_BUF_SIZE 10240
//using namespace std;
//using namespace std::chrono;
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
	/*high_resolution_clock::time_point t1 = high_resolution_clock::now();
	int count = 0;*/
};

#endif // !1