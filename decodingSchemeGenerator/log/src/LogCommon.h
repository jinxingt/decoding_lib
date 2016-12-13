#ifndef _LOG_COMMON_H
#define _LOG_COMMON_H
#include "rtcsbaseobject.h"

//�����ִ�Сд
#ifdef _WIN32
#define strcasecmp _stricmp
#define strdup     _strdup
#endif

//��־Ŀ¼��·��
#define kLogDir   "g:\\"
//#define	kPorfileDir			".\\log_prof.ini"
#define kProfileGeneral		 "general"
#define kProfileLogFiles	 "logfiles"
#define kDateFormatNormal	 "%F %T"
#define kDateFormatMilsec	 "%F %T.%3q"
//��־��ӡλ��,ϵͳ��־���ļ�������̨
enum logtypes {
	LOGTYPE_SYSLOG,
	LOGTYPE_FILE,
	LOGTYPE_CONSOLE,
};


#define __ALL__ __FILE__,__FUNCTION__,__LINE__
#define LOG_DEBUG	 0,__ALL__
#define LOG_VERBOSE	 1,__ALL__
#define LOG_NOTICE	 2,__ALL__
#define LOG_WARNING	 3,__ALL__
#define LOG_ERROR	 4,__ALL__
#define LOG_FATAL	 5,__ALL__
#define LOG_MODULE1  16,__ALL__
#define LOG_MODULE2  17,__ALL__
#define LOG_MODULE3  18,__ALL__
#define LOG_MODULE4  19,__ALL__
#define LOG_MODULE5  20,__ALL__
#define LOG_MODULE6  21,__ALL__
#define LOG_MODULE7  22,__ALL__
#define LOG_MODULE8  23,__ALL__
#define LOG_MODULE9  24,__ALL__
#define LOG_MODULE10  25,__ALL__

//��־����������
#define NUMLOGLEVELS 32
extern char *levelstr[NUMLOGLEVELS];
#define ENUMLEVELSTRING(x)   #x 
#define MAX_FILE_FULL_PATH 1000 //�ļ�·����󳤶�
#define MAX_LEVELS_STRING_LEN 200 //prof.ini����־�����ַ�������󳤶�
#define MAX_LEVEL_STRING_LEN 50   //ÿһ����־�����ַ�������󳤶�
#define  MAX_MESSAGE_LEN  1024  //һ����־����󳤶�
/*
static const int colors[NUMLOGLEVELS] = {
	COLOR_BRGREEN,
	COLOR_BRBLUE,		
	COLOR_YELLOW,
	COLOR_BRRED,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BRGREEN,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
	COLOR_BRBLUE,
};
*/


#define ast_log(level,fmt,...) ast_log(int level, __FILE__, __LINE__, __FUNCTION__, const char *fmt, ...)
#define PrintLogTest(...) //printf(__VA_ARGS__)
typedef struct{
	bool filename;
	bool functionname;
	bool linenumber;
	bool date;
	bool time;
	bool threadid;	
}tt_config;

//LOG_CLIENT_STATUS
enum{
	LOG_CLIENT_STATUS_NONE = 0,
	LOG_CLIENT_STATUS_INIT,
	LOG_CLIENT_STATUS_START,
	LOG_CLIENT_STATUS_STOP,
	LOG_CLIENT_STATUS_DESTROY,
};

#endif//__COMMON_H