#include "CellBuffer.h"

CellBuffer::CellBuffer(size_t size)
{
	_nSize = size;
	_pBuff = new char[size];
}

CellBuffer::~CellBuffer()
{
	if (_pBuff) {
		delete[] _pBuff;
		_pBuff = nullptr;
	}
	
}

char * CellBuffer::getData()
{
	return _pBuff;
}

bool CellBuffer::push(const char * pData, size_t len)
{
	////写入大量数据不一定要放到内存中
		////也可以存储到数据库或者磁盘等存储器中
		//if (_nLast + nLen > _nSize)
		//{
		//	//需要写入的数据大于可用空间
		//	int n = (_nLast + nLen) - _nSize;
		//	//拓展BUFF
		//	if (n < 8192)
		//		n = 8192;
		//	char* buff = new char[_nSize+n];
		//	memcpy(buff, _pBuff, _nLast);
		//	delete[] _pBuff;
		//	_pBuff = buff;
		//}
	if (_nLast + len <= _nSize) {
		memcpy(_pBuff + _nLast, pData, len);
		_nLast += len;
		if (_nLast == SEND_BUFF_SZIE) {
			_fullCount++;
		}
		return true;
	}
	else {
		_fullCount++;
	}
	
	return false;
}

void CellBuffer::pop(size_t size)
{
	int diff = _nLast - size;
	if (diff > 0) {
		memcpy(_pBuff, _pBuff + size, diff);

	}
	_nLast = diff > 0 ? diff : 0;
	if (_fullCount) {
		_fullCount--;

	}
}

int CellBuffer::write2socket(SOCKET sockfd)
{
	int ret = 0;
	//缓冲区有数据
	if (_nLast > 0 && INVALID_SOCKET != sockfd)
	{
		//发送数据
		ret = send(sockfd, _pBuff, _nLast, 0);
		//数据尾部位置清零
		_nLast = 0;
		//
		_fullCount = 0;
	}
	return ret;
}

int CellBuffer::read4socket(SOCKET sockfd)
{
	if (_nSize - _nLast > 0)
	{
		//接收客户端数据
		char* szRecv = _pBuff + _nLast;
		int nLen = (int)recv(sockfd, szRecv, _nSize - _nLast, 0);
		//printf("nLen=%d\n", nLen);
		if (nLen <= 0)
		{
			return nLen;
		}
		//消息缓冲区的数据尾部位置后移
		_nLast += nLen;
		return nLen;
	}
	return 0;
}

bool CellBuffer::hasMsg()
{
	if (_nLast >= sizeof(DataHeader))
	{
		//这时就可以知道当前消息的长度
		DataHeader* header = (DataHeader*)_pBuff;
		//判断消息缓冲区的数据长度大于消息长度
		return _nLast >= header->data_length;
	}
	return false;
}
