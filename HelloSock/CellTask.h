#ifndef _CELLTASK_H_
#define _CELLTASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "CellSemaphore.h"

class CellTaskServer
{
	typedef std::function<void()> CellTask;
private:
	std::list<CellTask> _tasks;
	std::list<CellTask> _tasksBuf;
	std::mutex _mutex;
	CellSemaphore _sem;
	bool _isRun = true;

public:
	void addTask(CellTask task);
	void start();
	void close();
protected:
	void onRun();
};
#endif // !_CELLTASK_H_




