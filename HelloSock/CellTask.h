#ifndef _CELLTASK_H_
#define _CELLTASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>

class CellTaskServer
{
	typedef std::function<void()> CellTask;
private:
	std::list<CellTask> _tasks;
	std::list<CellTask> _tasksBuf;
	std::mutex _mutex;
public:
	void addTask(CellTask task);
	void start();
protected:
	void onRun();
};
#endif // !_CELLTASK_H_




