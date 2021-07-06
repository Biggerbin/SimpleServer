#include "CellTask.h"

void CellTaskServer::addTasks(CellTask * task)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_tasksBuf.push_back(task);
}

void CellTaskServer::start()
{
	std::thread t(std::mem_fn(&CellTaskServer::onRun), this);
	t.detach();
}

void CellTaskServer::onRun()
{
	while (true) {
		if (!_tasksBuf.empty()) {
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto& ptask : _tasksBuf) {
				_tasks.push_back(ptask);
			}
			_tasksBuf.clear();
		}
		if (_tasks.empty()) {
			std::chrono::microseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		for (auto ptask : _tasks) {
			ptask->doTask();
			delete ptask;
		}
		_tasks.clear();
	}
}
