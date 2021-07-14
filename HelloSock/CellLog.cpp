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

void CellLog::setLogPath(const char * logPath, const char * mode)
{
	if (_logFile)
	{
		Info("CELLLog::setLogPath _logFile != nullptr\n");
		fclose(_logFile);
		_logFile = nullptr;
	}


	_logFile = fopen(logPath, mode);
	if (_logFile)
	{
		Info("CELLLog::setLogPath success,<%s,%s>\n", logPath, mode);
	}
	else {
		Info("CELLLog::setLogPath failed,<%s,%s>\n", logPath, mode);
	}
}

void CellLog::Info(const char * pStr)
{
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
			fprintf(pLog->_logFile, "%s", pStr);
			fflush(pLog->_logFile);
		}
		//printf("%s", pStr);
	});
}
