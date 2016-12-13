#ifndef _LOG_MESSAGE_H
#define _LOG_MESSAGE_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//#include "channel.h"
#include <iostream>
#include <queue>
#include <time.h>
#include "LogCommon.h"
using namespace std;


class LogChannel;
class LogClient;
class LogMessage
{
public:
	LogMessage();
	~LogMessage();
	int InitMessage(const int line, long process_id, const char* date, const char* file, char* message,const char* functionstr,int level);
	int DestroyMessage();
	int PrintMessageToChannels(tt_config* config, void* channellist);
private:
	int BuildMessageBuf(char* buf, LogChannel* channel, tt_config* config);
private:
	int line;
	long process_id;
	char* datestr;
	char* filestr;
public:
	char* messagestr;
	char* functionstr;
	int level;	
};
#endif