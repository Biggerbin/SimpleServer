#ifndef _MemoryPool_H_
#define _MemoryPool_H_

#include <mutex>

#define MAX_MEMORY_SZIE 1024

class MemoryPool;

class MemoryBlock
{
public:
	MemoryBlock();
	//�������ڴ�飨�أ�
	MemoryPool* _pBlock;
	//��һ��λ��
	MemoryBlock* _pNext;
	//�ڴ����
	int _nID;
	//���ô���
	int _nRef;
	//�Ƿ����ڴ����
	bool _isInPool;				
};

class MemoryPool
{
public:
	MemoryPool();

	void* allocMemory(size_t nsize);

	void freeMemory(void* pfree);

	void initMemory();

protected:
	//�ڴ��ַ
	char* _pBuf;
	//ͷ���ڴ浥Ԫ
	MemoryBlock* _pHeader;
	//�ڴ���С
	size_t _bsize;
	//�ڴ������
	size_t _nCount;
	std::mutex _mutex;
};

template<size_t count, size_t nBlocksize>
class MemoryAllocator : public MemoryPool {
public:
	MemoryAllocator() {
		size_t n = sizeof(void*);
		_bsize = nBlocksize;
		_nCount = count / n * n + count % n ? n : 0;
	}
};

class MemoryManager {
private:
	MemoryManager() {
		initMap(0, 64, &_mem64);
		initMap(65, 128, &_mem128);
		initMap(129, 256, &_mem256);
		initMap(257, 512, &_mem512);
		initMap(513, 1024, &_mem1024);
	}
	~MemoryManager()
	{

	}
public:
	static MemoryManager& getInstance() {
		static MemoryManager _mmgr;
		return _mmgr;
	};
	void* malloc_mem(size_t nsize);
	void free_mem(void *pMem);
	void addRef(void *pMem);
private:
	void initMap(int nBegin, int nEnd, MemoryPool* pool);
	MemoryAllocator<64, 1000> _mem64;
	MemoryAllocator<128, 1000> _mem128;
	MemoryAllocator<256, 1000> _mem256;
	MemoryAllocator<512, 1000> _mem512;
	MemoryAllocator<1024, 1000> _mem1024;
	MemoryPool* _szAlloc[MAX_MEMORY_SZIE + 1];
	std::mutex _mutex;
};
#endif //_MemoryPool_H_
