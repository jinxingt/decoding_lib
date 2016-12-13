#include "LogChannel.h"
#include "LogCommon.h"
#include <ctype.h>

LogChannel::LogChannel()
{
	PrintLogTest("LogChannel Created\n");
}
LogChannel::~LogChannel()
{
	PrintLogTest("LogChannel Deleted\n");
}
//channel name, logmask, lineno
int LogChannel::MakeChannel(const char* filepath, const char *filename, const char *levels)
{
	if(!strcasecmp(filename, "console")){
		this->logtype = LOGTYPE_CONSOLE;
	}
	else if(!strcasecmp(filename,"syslog")){
		this->logtype = LOGTYPE_SYSLOG;
	}
	else{
		//设置文件名,linux如果以/字符开始，表示是绝对路径
		char fullpathfile[MAX_FILE_FULL_PATH];
		snprintf(fullpathfile, MAX_FILE_FULL_PATH, "%s%s%s",
			filepath,filepath[strlen(filepath)-1] != '/' ?  "/": "", filename);

		//判断文件是否可以打开，如果不可以向控制台发出消息
		if (!(fileptr = fopen(fullpathfile, "a"))) {
			printf("ERROR: Unable to open log file[%s]\n",fullpathfile);
			return -1;
		}
		this->logtype = LOGTYPE_FILE;
	}
	//构造日志掩码
	MakeLogMask(levels);
	return 0;
}
//对字符串s根据，分割
void LogChannel::MakeLogMask(const char* s)
{
	char *wordb, *worde;
	char word[MAX_LEVEL_STRING_LEN];
	int wordlen;
	unsigned int res = 0;
	char *stringp = strdup(s);
	unsigned int x = 0;
	logmsk = 0;
	int found = 0;
	//逐个扫描字符串，以，分割，然后与level字符串数组比较
	while(*stringp != '\0')
	{
		if(isspace(*stringp))
		{//skip blanks
			stringp++;
			continue;
		}
		if (*stringp == '*')
		{//deal *: means all leves
			logmsk = 0xFFFFFFFF;
			return;
		}
		wordb = stringp; 
		if(!(worde = strstr(stringp, ","))){
			worde = stringp+strlen(stringp);
			found = 0;
		}
		else 
			found = 1;
		*worde = '\0';
		wordlen = worde-wordb;
		wordlen = (wordlen > MAX_LEVEL_STRING_LEN-1) ? (MAX_LEVEL_STRING_LEN-1) : wordlen;
		strcpy(word, wordb);
		for (x = 0; x < NUMLOGLEVELS; x++) {
			if (levelstr[x] && !strcasecmp(word, levelstr[x])) {
				logmsk |= (1 << x);
				break;
			}
		}//for
		stringp = worde+found;
	}
	PrintLogTest("logmask=%x\n", logmsk);
}
