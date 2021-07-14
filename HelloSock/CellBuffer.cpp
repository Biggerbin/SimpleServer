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
	////д��������ݲ�һ��Ҫ�ŵ��ڴ���
		////Ҳ���Դ洢�����ݿ���ߴ��̵ȴ洢����
		//if (_nLast + nLen > _nSize)
		//{
		//	//��Ҫд������ݴ��ڿ��ÿռ�
		//	int n = (_nLast + nLen) - _nSize;
		//	//��չBUFF
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
	//������������
	if (_nLast > 0 && INVALID_SOCKET != sockfd)
	{
		//��������
		ret = send(sockfd, _pBuff, _nLast, 0);
		//����β��λ������
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
		//���տͻ�������
		char* szRecv = _pBuff + _nLast;
		int nLen = (int)recv(sockfd, szRecv, _nSize - _nLast, 0);
		//printf("nLen=%d\n", nLen);
		if (nLen <= 0)
		{
			return nLen;
		}
		//��Ϣ������������β��λ�ú���
		_nLast += nLen;
		return nLen;
	}
	return 0;
}

bool CellBuffer::hasMsg()
{
	if (_nLast >= sizeof(DataHeader))
	{
		//��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
		DataHeader* header = (DataHeader*)_pBuff;
		//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		return _nLast >= header->data_length;
	}
	return false;
}
