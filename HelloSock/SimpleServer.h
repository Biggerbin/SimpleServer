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

	//��ʼ��socket����bind������listen
	SOCKET InitSocket(int port);
	//accept

	//�ر�����
	void Close();

	//����
	int onRun(CellThread* pthread);
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
	CellThread _thread;
};

#endif // !1