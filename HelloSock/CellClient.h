#ifndef _CELLCLIENT_H_
#define _CELLCLIENT_H_
#define CLIENT_HREAT_DEAD_TIME 5000
#define CLIENT_SEND_MSG_TIEM 200

#include "CellCommon.h"
typedef std::shared_ptr<DataHeader> dataHeaderPtr;
struct CellClient : public ObjectPoolBase<CellClient, 1000> {
public:
	CellClient(SOCKET cliSock);
	~CellClient();
	int SendReal();
	int SendData(dataHeaderPtr header);
	bool checkHeart(time_t dt);
	void resetDTHeart();
	bool checkSend(time_t dt);
	void resetDTSend();
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
	time_t _dtHeart;
	time_t _dtSend;

};

#endif // !_CELLCLIENT_H_


