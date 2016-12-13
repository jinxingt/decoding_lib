#include <iostream>
#include "TLog.h"
using namespace std;

int this_is_my_test_log_abcdexxx()
{
	int x = 0;
	InitLogDriver();
	while(1)
	{
		//Sleep(1);
		++x;
		LogDebug("logmessage = %d", x);
		LogVerbose("logmessage = %d", x);
		LogNotice("logmessage = %d", x);
		LogWarning("logmessage = %d", x);
		LogError("logmessage = %d", x);
		LogModule1("logmessage = %d", x);
		LogModule2("logmessage = %d", x);
		LogModule4("logmessage = %d", x);
		LogModule5("logmessage = %d", x);
		LogModule6("logmessage = %d", x);
		LogModule7("logmessage = %d", x);
		LogModule8("logmessage = %d", x);
		LogModule9("logmessage = %d", x);
		LogModule10("logmessage = %d", x);
		Sleep(10000);
		if(x == 1){
			printf("While Out\n");
			break;
		}
	}
	UnInitLogDriver();
	return 0;
}