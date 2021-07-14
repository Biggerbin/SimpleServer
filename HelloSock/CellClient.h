#ifndef _CELLCLIENT_H_
#define _CELLCLIENT_H_
#define CLIENT_HREAT_DEAD_TIME 60000
#define CLIENT_SEND_MSG_TIEM 200

#include "CellBuffer.h"
#include "CellCommon.h"
typedef std::shared_ptr<DataHeader> dataHeaderPtr;
struct CellClient : public ObjectPoolBase<CellClient, 1000> {
public:
	CellClient(SOCKET cliSock);
	~CellClient();
	bool hasMsg();
	DataHeader* front_msg();
	void pop_front_msg();
	int recvData();
	int SendReal();
	int SendData(dataHeaderPtr header);
	bool checkHeart(time_t dt);
	void resetDTHeart();
	bool checkSend(time_t dt);
	void resetDTSend();

public:
	int _cli_sock;
	//第二缓冲区 接收消息缓冲区
	CellBuffer _recvBuff;
	//发送缓冲区
	CellBuffer _sendBuff;
	time_t _dtHeart;
	time_t _dtSend;
	int id;
	int serverId;

};

#endif // !_CELLCLIENT_H_


