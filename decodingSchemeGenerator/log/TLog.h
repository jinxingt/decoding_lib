#ifndef _TLOG_H
#define _TLOG_H
#include "src/LogControl.h"

extern void InitLogDriver();//Need to Be CALLED FIRST!!!
extern void UnInitLogDriver();//IF you don't NEED log, Call this! ATTENTION

//ÅäÖÃÎÄ¼þÄ¿Â¼
#define	kPorfileDir			"config/log_prof.ini"

//System Log Level
#define LogDebug(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_DEBUG,__VA_ARGS__);\
}while(0);
#define LogVerbose(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_VERBOSE,__VA_ARGS__);\
}while(0);
#define LogNotice(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_NOTICE,__VA_ARGS__);\
}while(0);
#define LogWarning(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_WARNING,__VA_ARGS__);\
}while(0);
#define LogError(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_ERROR,__VA_ARGS__);\
}while(0);
#define LogFatal(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_FATAL,__VA_ARGS__);\
}while(0);

//Module Level,16-25(1-10)
#define LogModule1(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE1,__VA_ARGS__);\
}while(0);
#define LogModule2(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE2,__VA_ARGS__);\
}while(0);
#define LogModule3(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE3,__VA_ARGS__);\
}while(0);
#define LogModule4(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE4,__VA_ARGS__);\
}while(0);
#define LogModule5(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE5,__VA_ARGS__);\
}while(0);
#define LogModule6(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE6,__VA_ARGS__);\
}while(0);
#define LogModule7(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE7,__VA_ARGS__);\
}while(0);
#define LogModule8(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE8,__VA_ARGS__);\
}while(0);
#define LogModule9(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE9,__VA_ARGS__);\
}while(0);
#define LogModule10(...)  do{\
	LogControl* instance = LogControl::GetInstance();\
	instance->PrintLog(LOG_MODULE10,__VA_ARGS__);\
}while(0);
#endif
