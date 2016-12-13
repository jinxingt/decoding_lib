#ifndef _LOG_CHANNEL_H
#define _LOG_CHANNEL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class LogChannel 
{
public:
	FILE *fileptr;
	int logmsk;
	int logtype;
public:
	LogChannel();
	~LogChannel();
	//构造日志通道
	int MakeChannel(const char* filepath, const char *filename, const char *levelstr);
	//根据传入的字符串s(debug, warning)构造掩码
	void MakeLogMask(const char* s);
};
#endif

