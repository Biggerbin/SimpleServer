#include "CellSemaphore.h"

void CellSemaphore::release()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (++_wait <= 0) {
		++_wakeup;
		_cv.notify_one();
	}
}

void CellSemaphore::acquire()
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (--_wait < 0) {
		_cv.wait(lock, [this]() {
			return _wakeup > 0;
		});
		_wakeup--;
	}
}
