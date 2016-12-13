#ifndef __LOG_CONTROL_H
#define __LOG_CONTROL_H
#include "LogCommon.h"
class LogClient;
class LogControl
{
public:
	int OpenLogClient();
	int CloseLogClient();
	LogControl();
	~LogControl();
	void PrintLog(int level, const char *file,  const char *function,int line, const char *fmt, ...);
	static LogControl* GetInstance();
	static void ClearInstance();
protected:
private:
	LogClient* m_client;
	static LogControl* instance;
};

void InitLogDriver();
void UnInitLogDriver();
#endif//__LOG_H