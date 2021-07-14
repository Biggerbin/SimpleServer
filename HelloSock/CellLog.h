#ifndef _CELL_LOG_H_
#define _CELL_LOG_H_

#include "CellCommon.h"
#include "CellTask.h"
#include <ctime>
//Info
//Debug
//Warring
//Error
class CellLog
{
private:
	CellLog();
	~CellLog();
public:
	
	static CellLog& getInstance();
	void setLogPath(const char* logPath, const char* mode);
	static void Info(const char* pStr);
	template<typename ...Args>
	static void Info(const char* pformat, Args ... args) {
		CellLog* pLog = &getInstance();
		pLog->_taskServer.addTask([=]() {
			if (pLog->_logFile)
			{
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::gmtime(&tNow);
				fprintf(pLog->_logFile, "%s", "Info ");
				fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(pLog->_logFile, pformat, args...);
				fflush(pLog->_logFile);
			}
			//printf(pformat, args...);
		});
	}
private:
	FILE* _logFile = nullptr;
	CellTaskServer _taskServer;
};

#endif // !_CELL_LOG_H_
