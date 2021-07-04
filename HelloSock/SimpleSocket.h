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
	//���ջ�����
	char _recvBuf1[RECV_BUF_SIZE];
	//�ڶ�������
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;
	int _cli_sock;
};

//�����¼��ӿ�
class INetEvent
{
public:
	//���麯��
	//�ͻ��˼����¼�
	virtual void OnNetJoin(ClientSocket* pClient) = 0;
	//�ͻ����뿪�¼�
	virtual void OnNetLeave(ClientSocket* pClient) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header) = 0;
	//recv�¼�
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
	//�ж��Ƿ�����
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

	//��ʼ��socket����bind������listen
	SOCKET InitSocket(int port);
	//accept

	//�ر�����
	void Close();

	//����
	int onRun();
	//�ж��Ƿ�����
	bool isRun();
	int Accept();
	void time4msg();
	void addClientToCellServer(ClientSocket* pClient);
	void Start(int pthtread_cnt);
	virtual void OnNetJoin(ClientSocket* pClient);
	//�ͻ����뿪�¼�
	virtual void OnNetLeave(ClientSocket* pClient);
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header);
	//recv�¼�
	virtual void OnNetRecv(ClientSocket* pClient);

private:
	SOCKET _sock;
	SOCKET _cli_sock;
	SOCKET max_fd;
	fd_set read_set;
	std::vector<CellServer*> _cellServers;
protected:
	//SOCKET recv����
	std::atomic_int _recvCount;
	//�յ���Ϣ����
	std::atomic_int _msgCount;
	//�ͻ��˼���
	std::atomic_int _clientCount;
	Timestamp _tTime;
};

#endif // !1