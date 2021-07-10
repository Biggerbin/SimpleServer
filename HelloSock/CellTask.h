#ifndef _CELLTASK_H_
#define _CELLTASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "CellThread.h"

class CellTaskServer
{
	typedef std::function<void()> CellTask;
private:
	std::list<CellTask> _tasks;
	std::list<CellTask> _tasksBuf;
	std::mutex _mutex;
	CellThread _thread;

public:
	void addTask(CellTask task);
	void start();
	void close();
protected:
	void onRun(CellThread* pthread);
};
#endif // !_CELLTASK_H_




