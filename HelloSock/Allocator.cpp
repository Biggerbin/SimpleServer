#include "Allocator.h"

void* operator new(size_t nSize)
{
	return MemoryManager::getInstance().malloc_mem(nSize);
}

void operator delete(void* p)
{
	MemoryManager::getInstance().free_mem(p);
}

void* operator new[](size_t nSize)
{
	return MemoryManager::getInstance().malloc_mem(nSize);
}

void operator delete[](void* p)
{
	MemoryManager::getInstance().free_mem(p);
}

void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_free(void* p)
{
	free(p);
}
