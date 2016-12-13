#include "LogControl.h"
#include "LogMessage.h"
#include "LogClient.h"

/************************************************
LOG CLASS Implement
************************************************/

LogControl::LogControl()
{
	PrintLogTest("LogControl::LogControl()\n");
	m_client = NULL;
	OpenLogClient();
}
LogControl::~LogControl()
{
	PrintLogTest("LogControl::~LogControl()\n");
	CloseLogClient();
}
//Æô¶¯LogControl client
int LogControl::OpenLogClient()
{
	PrintLogTest("LogControl::OpenLogClient\n");
	if (m_client == NULL)
	{
		m_client = new LogClient;
		m_client->InitObject();
		m_client->Start();
	}
	return 0;
}
int LogControl::CloseLogClient()
{
	PrintLogTest("LogControl::CloseLogClient\n");
	if(m_client != NULL)
	{
		m_client->Stop();
		m_client->DestroyObject();
		delete m_client;
		m_client = NULL;
	}
	return 0;
}

void LogControl::PrintLog(int level, const char *file,  const char *functionname,int line, const char *fmt, ...)
{
	PrintLogTest("LogControl::PrintLog\n");
	char* message = NULL;
	message = new char[MAX_MESSAGE_LEN];
	if(message == NULL)
	return;

	va_list ap;
	va_start(ap,fmt);
	vsprintf(message,fmt,ap);
	va_end(ap);

	long process_id = GetCurrentProcessId();

	time_t ticktime = time(0);
	struct tm* current_time  = localtime(&ticktime);
	char date[100];
	sprintf(date, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	LogMessage *log_msg = new LogMessage();
	log_msg->InitMessage(line,process_id,date,file,message,functionname,level);
	delete message;
	message = NULL;

	if (log_msg != NULL)
		m_client->PutMessage(log_msg);	
}
//ÊµÀýinstance
LogControl* LogControl::instance = new LogControl;

LogControl* LogControl::GetInstance()
{
	return instance;
}
void LogControl::ClearInstance()
{
	if(instance != NULL){
		instance->CloseLogClient();
		delete instance;
		instance = NULL;
	}
}
void InitLogDriver()
{
	LogControl* instance = LogControl::GetInstance();
	if(instance == NULL){
		cout<<"ERROR:LogControl::GetInstance() == NULL"<<endl;
		return;
	}
	instance->OpenLogClient();
}
void UnInitLogDriver()
{
	LogControl* instance = LogControl::GetInstance();
	if(instance == NULL){
		cout<<"ERROR:LogControl::GetInstance() == NULL"<<endl;
		return;
	}
	instance->CloseLogClient();
	LogControl::ClearInstance();
}