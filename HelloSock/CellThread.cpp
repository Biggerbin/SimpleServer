#include "CellThread.h"

void CellThread::Start(EventCall onCreate, EventCall onRun, EventCall onDestory)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (!_isRun)
	{
		_isRun = true;

		if (onCreate)
			_onCreate = onCreate;
		if (onRun)
			_onRun = onRun;
		if (onDestory)
			_onDestory = onDestory;

		//线程
		std::thread t(std::mem_fn(&CellThread::OnWork), this);
		t.detach();
	}
}

void CellThread::Close()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_isRun)
	{
		_isRun = false;
		_sem.acquire();
	}
}

void CellThread::Exit()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_isRun)
	{
		_isRun = false;
	}
}

bool CellThread::isRun()
{
	return _isRun;
}

void CellThread::OnWork()
{
	if (_onCreate)
		_onCreate(this);
	if (_onRun)
		_onRun(this);
	if (_onDestory)
		_onDestory(this);

	_sem.release();
}
