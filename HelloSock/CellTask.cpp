#include "CellTask.h"

void CellTaskServer::addTask(CellTask task)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_tasksBuf.push_back(task);
}

void CellTaskServer::start()
{
	_thread.Start(nullptr, [this](CellThread* pthread) {
		onRun(pthread);
	}, nullptr);
}

void CellTaskServer::close()
{
	_thread.Close();
}

void CellTaskServer::onRun(CellThread* pthread)
{
	while (pthread->isRun()) {
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
			ptask();
		}
		_tasks.clear();
	}
}
