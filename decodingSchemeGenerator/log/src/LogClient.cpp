#include "LogClient.h"
#include "rtcserror.h"
#include "LogMessage.h"
#include "LogChannel.h"
#include "TLog.h"
#include <stdio.h>

//声明线程的方法
THREAD_RESULT HandleLoggerThread(void* Arg);
LogClient::LogClient()
{
	message_queue = NULL;
	message_thread = NULL;
	channel_list = NULL;
	logger_client_status = 0;

	profile_cfg.date = true;
	profile_cfg.filename = true;
	profile_cfg.functionname = true;
	profile_cfg.linenumber = true;
	PrintLogTest("LogClient NONE\n");
}
int LogClient::CreateChannelToList(const char* filepath,
								   const char *filename,
								   const char *levelstr)
{
	LogChannel* new_channel = new LogChannel();
	new_channel->MakeChannel(filepath,filename, levelstr);
	channel_list->push_back(new_channel);
	return 0;
}
int LogClient::DestroyChannelList()
{
	if(!channel_list)
		return -1;
	list<LogChannel*>::iterator iter = channel_list->begin();
	PrintLogTest("LogClient::DestroyChannelList\n");
	for(;iter != channel_list->end();iter++)
	{
		LogChannel* del_chan = *iter;
		PrintLogTest("del_chan = %p\n", del_chan);
		if(del_chan){
			if(del_chan->logtype != LOGTYPE_CONSOLE)
				fclose(del_chan->fileptr);
			delete del_chan;
			del_chan = NULL;
		}
		
	}
	delete channel_list;
	channel_list  = NULL;
	return 0;
}
void LogClient::ThreadWorkMethod()
{
	LogMessage* msg = NULL;
	for(;;)
	{
		//printf("begin to Pop\n");
		message_queue->Pop(msg);
		//printf("Pop Over\n");
		msg->PrintMessageToChannels(&profile_cfg, channel_list);
		delete msg;
		msg = NULL;
		if(logger_client_status != LOG_CLIENT_STATUS_START)
		{
			cout<<"WARNING:Log Thread Not Start"<<endl;
		}
	}
}

int LogClient::LoadProfile(const char* name)
{
	const int valueStrSize = 1024;
	char *buf = new char[valueStrSize];
	if (!buf)
		return -1;
	GetPrivateProfileString(kProfileGeneral,"filename","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.filename = true;
	}
	else
		profile_cfg.filename = false;
	GetPrivateProfileString(kProfileGeneral,"functionname","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.functionname = true;
	}
	else
		profile_cfg.functionname = false;
	GetPrivateProfileString(kProfileGeneral,"linenumber","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.linenumber = true;
	}
	else
		profile_cfg.linenumber = false;
	GetPrivateProfileString(kProfileGeneral,"date","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.date = true;
	}
	else
		profile_cfg.date = false;
	GetPrivateProfileString(kProfileGeneral,"time","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.time = true;
	}
	else
		profile_cfg.time = false;
	GetPrivateProfileString(kProfileGeneral,"threadid","yes",buf, valueStrSize,name);
	if(!strcasecmp(buf,"yes")){
		profile_cfg.threadid = true;
	}
	else
		profile_cfg.threadid = false;


	GetPrivateProfileString(kProfileGeneral,"logdir","yes",buf, valueStrSize,name);
	char logdir[MAX_FILE_FULL_PATH];
	memset(logdir, 0, MAX_FILE_FULL_PATH);
	strncpy(logdir, buf, MAX_FILE_FULL_PATH);
	logdir[MAX_FILE_FULL_PATH-1] = '\0';

	//logfiles
	char* filename = new char[MAX_FILE_FULL_PATH];//key
	char* levelstr = new char[MAX_LEVELS_STRING_LEN];//value
	char all_keys_name_string[valueStrSize];//对应kProfileLogFiles的所有KeyName的返回值
	long all_keys_name_size = GetPrivateProfileString(kProfileLogFiles,NULL,NULL,all_keys_name_string,valueStrSize,name);

	int nKeyNameLen=0;    //每个KeyName的长度
	for(int j = 0;j<all_keys_name_size;j++)
	{

		filename[nKeyNameLen++]=all_keys_name_string[j];
		if(all_keys_name_string[j]=='\0')
		{
			PrintLogTest("%s",filename);//key
			PrintLogTest(("="));
			if(GetPrivateProfileString(kProfileLogFiles,filename,NULL,levelstr,1024,name))
			{
				PrintLogTest("%s",levelstr);//value
				PrintLogTest(("\r\n"));
				if(0 != CreateChannelToList(logdir, filename, levelstr))
				{
					printf("CreateChannelToList Failed\n");
				}
			}
			memset(filename,0,MAX_FILE_FULL_PATH);
			nKeyNameLen=0;
		}//if
	}//for
	delete filename;
	filename = NULL;
	delete levelstr;
	levelstr = NULL;
	delete buf;
	buf = NULL;
	return 0;
}

// -1 for false ,0 for success
int LogClient::InitObject()
{
	channel_list = new list<LogChannel*>();
	if(channel_list == NULL)
		return -1;
	if(LoadProfile(kPorfileDir) != 0){
		cout<<"WARNING:LoadProfile != 0"<<endl;
		return -1;
	}
	message_queue = new RTCS_Queue<LogMessage*>(RTCS_QUEUESIZE);
	if(message_queue == NULL)
		return -1;

	message_thread = new RTCS_Thread((RTCS_THREADFUNC)HandleLoggerThread,this,RTCS_CTboolFALSE);
	if(message_thread == NULL)
		return -1;

	PrintLogTest("LogClient INIT\n");
	logger_client_status = LOG_CLIENT_STATUS_INIT;
	return 0;
}
//此方法是否需要增加对队列的元素进行释放内存的操作，需要测试
int LogClient::DestroyObject()
{
	if(message_thread != NULL)
	{
		delete message_thread;
		message_thread = NULL;
	}
	if (message_queue != NULL)
	{
		delete message_queue;
		message_queue = NULL;
	}
	DestroyChannelList();
	logger_client_status = LOG_CLIENT_STATUS_DESTROY;
	return 0;
}
LogClient::~LogClient()
{
	if (logger_client_status != LOG_CLIENT_STATUS_DESTROY)
	{
		DestroyObject();
	}
}
int LogClient::Start()
{
	if(logger_client_status == LOG_CLIENT_STATUS_NONE){
		cout<<"ERROR:please CALL InitObject to INIT Objects"<<endl;
		return -1;
	}
	if (message_thread == NULL)
	{
		message_thread = new RTCS_Thread((RTCS_THREADFUNC)HandleLoggerThread,this,RTCS_CTboolFALSE);
		if(message_thread == NULL)
			return -1;
	}
	if(message_thread != NULL && message_thread->IsValid()){
		PrintLogTest("Thread Resume\n");
		message_thread->Resume();
	}
	PrintLogTest("LogClient START!!\n");
	logger_client_status = LOG_CLIENT_STATUS_START;
	return 0;
}
int LogClient::Stop()
{
	if(logger_client_status == LOG_CLIENT_STATUS_NONE){
		cout<<"ERROR:please CALL InitObject to INIT Objects"<<endl;
		return -1;
	}

	if(message_thread != NULL){
		delete message_thread;
		message_thread = NULL;
	}
	PrintLogTest("LogClient STOP\n");
	logger_client_status = LOG_CLIENT_STATUS_STOP;
	return 0;
} 

int LogClient::PutMessage(LogMessage* log_msg)
{
	//如果线程方法执行完毕，线程还叫做Running吗
	if(message_thread != NULL && message_thread->Running())
	{
		if(logger_client_status != LOG_CLIENT_STATUS_START){
			cout<<"ERROR:LogClient::PutMessage FAILED, Not Start"<<endl;
			delete log_msg;
			log_msg = NULL;
		}
		else{
			message_queue->Push(log_msg);
			message_thread->Resume();
		}
	}
	else
	{
		//打印消息到控制台，说明线程没有启动
		cout<<"WARNING:LogThread is not Running"<<endl;
		log_msg->PrintMessageToChannels(&profile_cfg, channel_list);
		delete log_msg;
		log_msg = NULL;
	}
	return 0;
}

THREAD_RESULT HandleLoggerThread(void* Arg)
{
	((LogClient*)Arg)->ThreadWorkMethod();
	return 0;
}