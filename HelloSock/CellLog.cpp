#include "CellLog.h"

CellLog::CellLog()
{
	_taskServer.start();
}

CellLog::~CellLog()
{
	_taskServer.close();
	if (_logFile) {
		fclose(_logFile);
		_logFile = nullptr;
	}
}

CellLog& CellLog::getInstance()
{
	static CellLog _log;
	return _log;
}

void CellLog::setLogPath(const char * logName, const char * mode)
{
	if (_logFile)
	{
		Info("CELLLog::setLogPath _logFile != nullptr\n");
		fclose(_logFile);
		_logFile = nullptr;
	}
	static char logPath[256] = {};
	auto t = system_clock::now();
	auto tNow = system_clock::to_time_t(t);
	std::tm* now = std::localtime(&tNow);
	sprintf(logPath, "[%d_%d_%d %d_.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour);

	_logFile = fopen(logPath, mode);
	if (_logFile)
	{
		Info("CELLLog::setLogPath success,<%s,%s>\n", logPath, mode);
	}
	else {
		Info("CELLLog::setLogPath failed,<%s,%s>\n", logPath, mode);
	}
}

void CellLog::Echo(const char * pformat, const char * pStr)
{
	CellLog* pLog = &getInstance();
	pLog->_taskServer.addTask([=]() {
		if (pLog->_logFile)
		{
			auto t = system_clock::now();
			auto tNow = system_clock::to_time_t(t);
			//fprintf(pLog->_logFile, "%s", ctime(&tNow));
			std::tm* now = std::localtime(&tNow);
			fprintf(pLog->_logFile, "%s", pformat);
			fprintf(pLog->_logFile, "[%d-%d-%d %d:%d:%d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
			fprintf(pLog->_logFile, "%s", pStr);
			fflush(pLog->_logFile);
		}
		//printf("%s", pStr);
	});
}

void CellLog::Info(const char * pStr)
{
	Echo("Info", pStr);
}

void CellLog::Debug(const char * pStr)
{
	Echo("Debug", pStr);
}

void CellLog::Warning(const char * pStr)
{
	Echo("Warning", pStr);
}

void CellLog::Error(const char * pStr)
{
	Echo("Error", pStr);
}
