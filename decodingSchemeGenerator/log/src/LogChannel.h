#ifndef _LOG_CHANNEL_H
#define _LOG_CHANNEL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class LogChannel 
{
public:
	FILE *fileptr;
	int logmsk;
	int logtype;
public:
	LogChannel();
	~LogChannel();
	//������־ͨ��
	int MakeChannel(const char* filepath, const char *filename, const char *levelstr);
	//���ݴ�����ַ���s(debug, warning)��������
	void MakeLogMask(const char* s);
};
#endif

