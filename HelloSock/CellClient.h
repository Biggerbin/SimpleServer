#ifndef _CELLCLIENT_H_
#define _CELLCLIENT_H_

#include "CellCommon.h"
typedef std::shared_ptr<DataHeader> dataHeaderPtr;
struct CellClient : public ObjectPoolBase<CellClient, 1000> {
public:
	CellClient(SOCKET cliSock);

	int SendData(dataHeaderPtr header);

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

#endif // !_CELLCLIENT_H_


