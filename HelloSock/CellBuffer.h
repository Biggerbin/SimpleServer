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
	//�ڶ������� ���ͻ�����
	char* _pBuff = nullptr;
	//�������������������������ݿ�
	//list<char*> _pBuffList;
	//������������β��λ�ã��������ݳ���
	int _nLast = 0;
	//�������ܵĿռ��С���ֽڳ���
	int _nSize = 0;
	//������д����������
	int _fullCount = 0;
};

#endif //_CELLBUFFER_H_
