#include "CS_Time.h"

#ifdef _WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;

	return (0);
}
#endif /* WIN32 */
long timeDiffMacroSeconds(timeval& start, timeval& end){
	return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}
char* timeFormatMacroToString(long cost){
	static char buf[256];
	if (cost >= 1000 * 1000 * 60){
		cost /= 1000;
		sprintf(buf, "%d min %.3f s", cost / 1000 / 60, cost % (6000)*1.0 / 1000);
	}
	else if (cost >= 1000 * 1000){
		cost /= 1000;
		sprintf(buf, "%.3f s", cost*1.0 / 1000);
	}
	else if (cost >= 1000){
		sprintf(buf, "%.3f ms", cost*1.0 / 1000);
	}
	else{
		sprintf(buf, "%d us", cost);
	}
	return buf;
}

