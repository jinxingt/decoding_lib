#ifndef __CS__TIME__H__
#define __CS__TIME__H__

#include "CS_Common.h"

#include <time.h>
#ifdef _WIN32
#   include <windows.h>
#else
#include <sys/time.h>
#endif
#ifdef _WIN32
int gettimeofday(struct timeval *tp, void *tzp);
#endif /* WIN32 */

extern long timeDiffMacroSeconds(timeval& start, timeval& end);
extern char* timeFormatMacroToString(long cost);

#endif