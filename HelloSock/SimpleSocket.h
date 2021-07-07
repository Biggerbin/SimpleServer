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
	//���ջ�����
	char _recvBuf1[RECV_BUF_SIZE];
	//�ڶ�������
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;

	//�ڶ������� ���ͻ�����
	char _szSendBuf[SEND_BUFF_SZIE];
	//���ͻ�����������β��λ��
	int _lastSendPos = 0;
	
};
typedef std::shared_ptr<ClientSocket> ClientSocktPtr;
//�����¼��ӿ�
class INetEvent
{
public:
	//���麯��
	//�ͻ��˼����¼�
	virtual void OnNetJoin(ClientSocktPtr pClient) = 0;
	//�ͻ����뿪�¼�
	virtual void OnNetLeave(ClientSocktPtr pClient) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(CellServer*, ClientSocktPtr pClient, DataHeader* header) = 0;
	//recv�¼�
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

	//ִ������
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
	//�ж��Ƿ�����
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
	void addClientToCellServer(ClientSocktPtr pClient);
	void Start(int pthtread_cnt);
	virtual void OnNetJoin(ClientSocktPtr pClient);
	//�ͻ����뿪�¼�
	virtual void OnNetLeave(ClientSocktPtr pClient);
	//�ͻ�����Ϣ�¼�
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocktPtr pClient, DataHeader* header);
	//recv�¼�
	virtual void OnNetRecv(ClientSocktPtr pClient);

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