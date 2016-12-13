#include "LogMessage.h"
#include "LogCommon.h"
#include <stdio.h>
#include <stdlib.h>
#include "LogChannel.h"
#include <list>
using std::list;

char *levelstr[NUMLOGLEVELS] = {
	//System Level,0-15
	"DEBUG",
	"VERBOSE",
	"NOTICE",
	"WARNING",
	"ERROR",
	"FATAL",//6
	"","","","","",//5
	"","","","","",//5
	//Module Level,16-31
	"MODULE1","MODULE2","MODULE3","MODULE4","MODULE5","MODULE6",//6
	"MODULE7","MODULE8","MODULE9","MODULE10","",//5
	"","","","","",//5
};

//此构造函数需要重写，构造函数设置默认数据，再有一个单独的设置属性的方法，这样可以判断是否成功
int LogMessage::InitMessage(const int line, long process_id, const char* date, const char* file, char* message,const char* functionstr,int level)
{
	int len;
	this->line = line;//line
	this->process_id = process_id;//process_id
	len = strlen(date)+1;
	this->datestr = new char[len];//datestr
	memcpy(this->datestr,date,len);
	//cut the filename
	len = strlen(file) + 1;//filestr
	for (int i = len-1; i >= 0; --i)
	{
		if (file[i] == '\\' || file[i] == '\/') {
			this->filestr = new char[len];
			memcpy(this->filestr, file+i+1, len-i-1);
			break;
		}
	}
	len = strlen(message)+1;//message
	this->messagestr = new char[len];
	memcpy(this->messagestr, message, len);
	len = strlen(functionstr) + 1;//functionstr
	this->functionstr = new char[len];
	memcpy(this->functionstr, functionstr,len);
	this->level = level;//level
	return 0;
}
int LogMessage::DestroyMessage()
{
	if(messagestr != NULL)
	{
		delete messagestr;
		messagestr = NULL;
	}
	if (filestr != NULL)
	{
		delete filestr;
		filestr = NULL;
	}
	if(datestr != NULL)
	{
		delete datestr;
		datestr = NULL;
	}
	if(functionstr != NULL)
	{
		delete functionstr;
		functionstr = NULL;
	}
	return 0;
}
//
int LogMessage::PrintMessageToChannels(tt_config* msg_config,void* channellist)
{
	char buf[MAX_MESSAGE_LEN];
	int need_log = 0;
	if(!msg_config)
		return -1;

	list<LogChannel*>* chan_list = (list<LogChannel*>*)channellist;
	if(!chan_list)
		return -1;
	list<LogChannel*>::iterator iter = chan_list->begin();
	for ( ; iter != chan_list->end(); ++iter)
	{
		LogChannel* current_chan = *iter;
		if(current_chan)
		{
			PrintLogTest("Current Channel logmask: %x\n", current_chan->logmsk);
			if(BuildMessageBuf(buf,current_chan,msg_config)!= 0)
				continue;
			switch (current_chan->logtype)
			{
			case LOGTYPE_CONSOLE:
				fprintf(stdout,"%s\n", buf);
				fflush(stdout);
				break;
			case LOGTYPE_SYSLOG:
				break;
			case LOGTYPE_FILE:
				fprintf(current_chan->fileptr,"%s\n", buf);
				fflush(current_chan->fileptr);
				break;
			default:
				printf("Current Log Channel: Unknown Channel LogType\n");
				break;
			}
			if (level == 5){//FATAL
				exit(-1);
			}
		}
	}
	
	
}
LogMessage::~LogMessage()
{
	DestroyMessage();
}
LogMessage::LogMessage()
{
}
//0: to log  -1: no need to log
int LogMessage::BuildMessageBuf(char* buf, LogChannel* channel, tt_config* config)
{
	//datestr, levelstr, filestr, line, functionstr, 
	if((channel->logmsk >> level) & 0x01)
	{
		
		memset(buf, 0, MAX_MESSAGE_LEN);
		PrintLogTest("BuildMessageBuf: Yes, Need To Log\n");
		//[level:linenumber][file:function][date:processid]:[message] 
		snprintf(buf, MAX_MESSAGE_LEN, "[%-8s:%5d][%s:%s][%s:%6ld] : [%s]",
			levelstr[level], 
			(config->linenumber) ? line : 0, 
			(config->filename) ? filestr : "", 
			(config->functionname) ? functionstr : "",
			(config->date)? datestr : "", 
			(config->threadid)? process_id : 0, 
			messagestr);
		buf[MAX_MESSAGE_LEN-1] = '\0';

		/*
		char* cur_pos = buf;
		cur_pos += sprintf(buf, "[%s", levelstr[level]);
		if (config->linenumber)
			cur_pos += sprintf(buf,":%5d]", line);
		else
			cur_pos += sprintf(cur_pos,"%s","]");
		if(config->filename || config->functionname)
		{
			*cur_pos = "[";
			cur_pos++;
			if(config->filename && config->functionname)
				cur_pos += sprintf(cur_pos, "%s:%s", filestr, functionstr);
			else if (config->filename){
				cur_pos += sprintf(cur_pos, "%s", filestr);
			else if (config->functionname)
					cur_pos += sprintf(cur_pos, "%s", functionstr);
			*cur_pos = "]";
			cur_pos++;
		}
		if(config->date || config->threadid)
		{
			*cur_pos = "[";
			cur_pos++;
			if(config->date && config->threadid)
				cur_pos += sprintf(cur_pos, "%s:%d", datestr, process_id);
			else if (config->date){
				cur_pos += sprintf(cur_pos, "%s", datestr);
			else if (config->threadid)
				cur_pos += sprintf(cur_pos, "%d", process_id);
			*cur_pos = "]";
			cur_pos++;
		}
		*/
		return 0;
	}
	return -1;
}