#ifndef _CELLBUFFER_H_
#define _CELLBUFFER_H_
#include "CellCommon.h"
class CellBuffer
{
public:
	CellBuffer(size_t size = 8192);
	~CellBuffer();
	char* getData();
	bool push(const char* pData, size_t len);
	void pop(size_t size);
	int write2socket(SOCKET sockfd);
	int read4socket(SOCKET sockfd);
	bool hasMsg();
private:
	//第二缓冲区 发送缓冲区
	char* _pBuff = nullptr;
	//可以用链表或队列来管理缓冲数据块
	//list<char*> _pBuffList;
	//缓冲区的数据尾部位置，已有数据长度
	int _nLast = 0;
	//缓冲区总的空间大小，字节长度
	int _nSize = 0;
	//缓冲区写满次数计数
	int _fullCount = 0;
};

#endif //_CELLBUFFER_H_
