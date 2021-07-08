#include "Allocator.h"
#include <SimpleServer.h>

class MyServer : public SimpleServer
{
public:
	MyServer() {

	}
	//ֻ�ᱻһ���̴߳��� ��ȫ
	virtual void OnNetJoin(ClientSocktPtr pClient)
	{
		SimpleServer::OnNetJoin(pClient);
		//printf("client<%d> join\n", pClient->sockfd());
	}
	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetLeave(ClientSocktPtr pClient)
	{
		SimpleServer::OnNetLeave(pClient);
		//printf("client<%d> leave\n", pClient->sockfd());
	}
	//cellServer 4 ����̴߳��� ����ȫ
	//���ֻ����1��cellServer���ǰ�ȫ��
	virtual void OnNetMsg(CellServer* pCellServer, ClientSocktPtr pClient, DataHeader* header)
	{
		SimpleServer::OnNetMsg(pCellServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//send recv 
			Login* login = (Login*)header;
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", cSock, login->dataLength, login->userName, login->PassWord);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//LoginResult ret;
			//pClient->SendData(&ret);
			std::shared_ptr<LoginResult> ret(new LoginResult());
			pCellServer->addSendTask(pClient, ret);
		}//���� ��Ϣ---���� ����   ������ ���ݻ�����  ������ 
		break;
		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			//printf("�յ��ͻ���<Socket=%d>����CMD_LOGOUT,���ݳ��ȣ�%d,userName=%s \n", cSock, logout->dataLength, logout->userName);
			//�����ж��û������Ƿ���ȷ�Ĺ���
			//LogoutResult ret;
			//SendData(cSock, &ret);
		}
		break;
		default:
		{
			printf("<socket=%d>�յ�δ������Ϣ,���ݳ��ȣ�%d\n", pClient->_cli_sock, header->data_length);
			//DataHeader ret;
			//SendData(cSock, &ret);
		}
		break;
		}
	}
	virtual void OnNetRecv(ClientSocktPtr pClient)
	{
		SimpleServer::OnNetRecv(pClient);
		//printf("client<%d> leave\n", pClient->sockfd());
	}
private:

};

bool g_bRun = true;
void cmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			g_bRun = false;
			printf("�˳�cmdThread�߳�\n");
			break;
		}
		else {
			printf("��֧�ֵ����\n");
		}
	}
}

int main() {

	MyServer server;
	server.InitSocket(8080);
	server.Start(4);

	//����UI�߳�
	std::thread t1(cmdThread);
	t1.detach();

	while (g_bRun)
	{
		server.onRun();
		//printf("����ʱ�䴦������ҵ��..\n");
	}
	server.Close();
	printf("���˳���\n");
	getchar();
	return 0;
}