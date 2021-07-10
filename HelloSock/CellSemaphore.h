#ifndef _CELLSEMAPHORE_H_
#define _CELLSEMAPHORE_H_

#include <condition_variable>
#include <thread>
#include <chrono>

class CellSemaphore
{
public:
	void release();
	void acquire();

private:
	//改变数据缓冲区时需要加锁
	std::mutex _mutex;
	//阻塞等待-条件变量
	std::condition_variable _cv;
	//等待计数
	int _wait = 0;
	//唤醒计数
	int _wakeup = 0;
};

#endif // !_CELLSEMAPHORE_H_


