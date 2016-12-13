#ifndef __CS__COMMON__H__
#define __CS__COMMON__H__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <string.h>
#include <limits.h>
#include <list>
#include <cassert>
#include <unordered_map>
#include "json.h"
#include "TLog.h"
#include <fstream>
using namespace std;

#define ENABLE_DEBUG 0

const int loglevel = 0; //loglevelֻ����ENABLE_DEBUGΪ0��ʱ��Ż������ã����ƽʱ��Ϊ0���ɡ�
const char* const programName = "piterCode.exe";
const char* const configDir = "config/config.json";
extern char* g_RecoveryStr;


#define MAX_ROW 300
#define MAX_COL 300
#define MAX_STACK_SIZE 10000
#define MAX_LOST_CIRCUMSTANCES 2

#endif//__CS__COMMON__H__