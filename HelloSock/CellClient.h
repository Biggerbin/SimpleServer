#ifndef _CELLCLIENT_H_
#define _CELLCLIENT_H_

#include "CellCommon.h"
typedef std::shared_ptr<DataHeader> dataHeaderPtr;
struct CellClient : public ObjectPoolBase<CellClient, 1000> {
public:
	CellClient(SOCKET cliSock);

	int SendData(dataHeaderPtr header);

	int _cli_sock;
	//接收缓冲区
	char _recvBuf1[RECV_BUF_SIZE];
	//第二缓冲区
	char _szMsgBuf2[RECV_BUF_SIZE * 10];
	int _lastPos;

	//第二缓冲区 发送缓冲区
	char _szSendBuf[SEND_BUFF_SZIE];
	//发送缓冲区的数据尾部位置
	int _lastSendPos = 0;

};

#endif // !_CELLCLIENT_H_


