#include "MemoryPool.h"

MemoryPool::MemoryPool()
{
	_pBuf = nullptr;
	_pHeader = nullptr;
	_bsize = 0;
	_nCount = 0;
}

void * MemoryPool::allocMemory(size_t nsize)
{
	std::lock_guard<std::mutex> lg(_mutex);
	if (!_pBuf) {
		initMemory();
	}
	MemoryBlock* pReturn = nullptr;
	if (nullptr == _pHeader) {
		pReturn = (MemoryBlock*)malloc(nsize + sizeof(MemoryBlock));
		pReturn->_pBlock = nullptr;
		pReturn->_isInPool = false;
		pReturn->_nID = -1;
		pReturn->_pNext = nullptr;
		pReturn->_nRef = 1;
	}
	else {
		pReturn = _pHeader;
		_pHeader = _pHeader->_pNext;
		pReturn->_nRef = 1;
	}
	return ((char*)pReturn + sizeof(MemoryBlock));
}

void MemoryPool::freeMemory(void* pfree)
{
	MemoryBlock *pFree = (MemoryBlock*)((char*)pfree - sizeof(MemoryBlock));

	if (pFree->_isInPool) {
		std::lock_guard<std::mutex> lg(_mutex);
		if (0 == --pFree->_nRef) {
			pFree->_pNext = _pHeader;
			_pHeader = pFree;
		}
	}
	else {
		std::lock_guard<std::mutex> lg(_mutex);
		if (0 == --pFree->_nRef) {
			free(pFree);
		}
	}
}

void MemoryPool::initMemory()
{
	if (_pBuf) {
		return;
	}
	size_t realSize = _bsize + sizeof(MemoryBlock);
	size_t bufSize = realSize * _nCount;
	_pBuf = (char*)malloc(bufSize);
	_pHeader = (MemoryBlock*)_pBuf;
	_pHeader->_nID = 0;
	_pHeader->_pBlock = this;
	_pHeader->_nRef = 0;
	_pHeader->_isInPool = true;
	_pHeader->_pNext = nullptr;
	MemoryBlock* tmp = _pHeader;
	for (size_t i = 1; i < _nCount; ++i) {
		MemoryBlock* cur = (MemoryBlock*)(_pBuf + i * realSize);
		cur->_isInPool = true;
		cur->_nID = i;
		cur->_nRef = 0;
		cur->_pNext = nullptr;
		cur->_pBlock = this;
		tmp->_pNext = cur;
		tmp = cur;
	}
}

MemoryBlock::MemoryBlock()
{
	_pBlock = nullptr;
	_isInPool = false;
	_nID = -1;
	_pNext = nullptr;
	_nRef = 0;
}



void * MemoryManager::malloc_mem(size_t nsize)
{
	if (nsize <= MAX_MEMORY_SZIE) {
		return _szAlloc[nsize]->allocMemory(nsize);
	}
	else {
		MemoryBlock* pReturn = (MemoryBlock*)malloc(nsize + sizeof(MemoryBlock));
		pReturn->_isInPool = false;
		pReturn->_nID = -1;
		pReturn->_nRef = 1;
		pReturn->_pBlock = nullptr;
		pReturn->_pNext = nullptr;
		return ((char*)pReturn + sizeof(MemoryBlock));
	}
	return nullptr;
}

void MemoryManager::free_mem(void * pMem)
{
	MemoryBlock* pFree = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	if (pFree->_isInPool) {
		pFree->_pBlock->freeMemory(pMem);
	}
	else {
		if(0 == --pFree->_nRef)
			free(pFree);
	}
}

void MemoryManager::addRef(void * pMem)
{
	std::lock_guard<std::mutex> lg(_mutex);
	MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
	pBlock->_nRef++;
}

void MemoryManager::initMap(int nBegin, int nEnd, MemoryPool * pool)
{
	for (int n = nBegin; n <= nEnd; n++)
	{
		_szAlloc[n] = pool;
	}
}

