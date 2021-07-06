#ifndef _CELLTASK_H_
#define _CELLTASK_H_

#include <thread>
#include <mutex>
#include <list>

class CellTask
{
public:
	CellTask() {}
	virtual ~CellTask() {}
	virtual void doTask() {}
};


class CellTaskServer
{
private:
	std::list<CellTask*> _tasks;
	std::list<CellTask*> _tasksBuf;
	std::mutex _mutex;
public:
	void addTasks(CellTask* task);
	void start();
protected:
	void onRun();
};
#endif // !_CELLTASK_H_




