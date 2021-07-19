#ifndef _CELL_LOG_H_
#define _CELL_LOG_H_

#include "CellCommon.h"
#include "CellTask.h"
#include <ctime>
//Info
//Debug
//Warring
//Error

#ifdef _DEBUG
	#ifndef CELLLOG_DEBUG
		#define CELLLOG_DEBUG(...) CellLog::Debug(_VA_ARGS)
	#endif
#else
	#ifndef CELLLOG_DEBUG
		#define CELLLOG_DEBUG(...)
	#endif
#endif

#define CELLLOG_INFO(...) CellLog::Info(_VA_ARGS)
#define CELLLOG_WARNING(...) CellLog::Warning(_VA_ARGS)
#define CELLLOG_ERROR(...) CellLog::Error(_VA_ARGS)

class CellLog
{
private:
	CellLog();
	~CellLog();
public:
	
	static CellLog& getInstance();
	void setLogPath(const char* logPath, const char* mode);
	static void Echo(const char * pformat, const char* pStr);
	static void Info(const char* pStr);
	static void Debug(const char* pStr);
	static void Warning(const char* pStr);
	static void Error(const char* pStr);

	template<typename ...Args>
	static void Info(const char* pformat, Args ... args) {
		Echo("Info", pformat, args...);
	}
	template<typename ...Args>
	static void Debug(const char* pformat, Args ... args) {
		Echo("Debug", pformat, args...);
	}
	template<typename ...Args>
	static void Warning(const char* pformat, Args ... args) {
		Echo("Warning", pformat, args...);
	}
	template<typename ...Args>
	static void Error(const char* pformat, Args ... args) {
		Echo("Error", pformat, args...);
	}

	template<typename ...Args>
	static void Echo(const char* type, const char* pformat, Args ... args) {
		CellLog* pLog = &getInstance();
		pLog->_taskServer.addTask([=]() {
			if (pLog->_logFile)
			{
				auto t = system_clock::now();
				auto tNow = system_clock::to_time_t(t);
				//fprintf(pLog->_logFile, "%s", ctime(&tNow));
				std::tm* now = std::localtime(&tNow);
				fprintf(pLog->_logFile, "%s", type);
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
