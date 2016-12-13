#ifndef __LOG_CLIENT_H
#define __LOG_CLIENT_H
#include <iostream>
#include <queue>
#include <list>
using namespace std;

#include "LogMessage.h"
class LogMessage;
class LogChannel;
class LogClient
{
public:
	LogClient();
	~LogClient();
	int InitObject();
	int Start();
	int Stop();
	int PutMessage(LogMessage* log_msg);
	void ThreadWorkMethod();
	int DestroyObject();
	int CreateChannelToList(const char* filepath, const char *filename, const char *levelstr);
	int DestroyChannelList();
private:
	RTCS_Queue<LogMessage*> *message_queue;//all log messages queue
	RTCS_Thread  *message_thread;
	int logger_client_status;
	tt_config profile_cfg;
	list<LogChannel*> *channel_list;
private:
	int LoadProfile(const char* name);
};

#endif//__LOG_CLIENT_H