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
	//�ı����ݻ�����ʱ��Ҫ����
	std::mutex _mutex;
	//�����ȴ�-��������
	std::condition_variable _cv;
	//�ȴ�����
	int _wait = 0;
	//���Ѽ���
	int _wakeup = 0;
};

#endif // !_CELLSEMAPHORE_H_


