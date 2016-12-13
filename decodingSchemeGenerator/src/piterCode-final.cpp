
/*
此为最终版本：


Non-MDS编码
给定的非标准RDP编码，在进行行列缩减后，故障单个块后，所需要的恢复代价。
输入： 原始矩阵n行，形成n*n的矩阵块，额外的3列校验，以及额外的冗余校验块(2个)
输出： 故障单列的代价
使用了三种方法遍历输出结果：枚举法，贪心法
注意：本文件主要是考虑到程序暴力方法运行太慢，而转向进行一些优化策略。能提高几倍的话就非常好了。
没优化之前：
rdor.exe 8 3 0 0 0 0   运行时间3.213s
第一次优化，将所有的map修改为unordered_map，运行时间变为 2.556 s
第二次优化，将函数中的参数减少两个(minMap, minGroup)，不再使用它，运行时间变为2.17s(未采用)
第三次优化，删除lost_num的排错型判别，运行时间为2.185
第四次优化，将从map元素中取出elem的操作过程修改一下，利用了iterator，运行时间变为1.50s

容错方式：外部可输出，只测试过3容错

支持三种编码方式，标准RDP，方阵的RDP，非方阵类型的RDP
方阵类型的RDP： 标准RDP中的构造中，去掉p校验的那一列，然后对每参与对角校验的那一列增加一个额外的q校验和r校验
非方阵类型的RDP：标准EVENODD编码中，将正反的主对角异或S增加到额外的p校验和r校验上。

*/

#include "CS_Common.h"
#include "CS_Time.h"
#include "CS_Group.h"
#include "CS_RecoveryAlgorithm.h"
#include "CS_CodingScheme.h"


int minCostArray[] = { 0, 1, 2, 3, 12, 17, 25, 33, 44, 55, 69, 81, 99, 114, 135, 151, 176, 194, 222, 241 };
int row, k, m, short_row, short_col, lost_col, algorithm, code_type;
char* g_RecoveryStr = "";


void read_params(int argc, char** argv);
bool isPrime(int num) {
	if (num <= 1) return false;
	for (int i = 2; i * i <= num; i++) {
		if (num % i == 0) return false;
	}
	return true;
}
int main(int argc, char** argv)
{
	read_params(argc, argv);

	CS_CodingSchemeMgr& mgr = CS_CodingSchemeMgr::getInstance();
	CS_CodingSchemeKey key((CSCodingSchemeType)code_type, row, k, m, short_row, short_col, algorithm);
	CS_CodingScheme& scheme = mgr.getCodingScheme(key);

	vector<int> cols = { lost_col };
	CS_CodingSchemeRecoveryKey rkey(cols, algorithm);
	CS_CodingSchemeRecoveryValue& rv = scheme.recoveryDetailScheme(rkey);
	scheme.printRecoveryScheme(rv);

	while (1)
	{
		printf("Read Cmd(type, row, k, m, lost_col, algorithm): ");
		cin >> code_type >> row >> k >> m >> lost_col >> algorithm;

		CS_CodingSchemeKey key((CSCodingSchemeType)code_type, row, k, m, short_row, short_col, algorithm);
		CS_CodingScheme& scheme = mgr.getCodingScheme(key);
	}
	return 0;
}

void read_params(int argc, char** argv)
{
	if (argc < 9){
		printf("Usage: %s <row-12> <k-12> <m-3> <short_row-0> <short_col-6> <lost_col-0> <algorithm-3> <codeType-1> [loglevel-1]\n", programName);
		printf("        short_col: delete the data columns in the right side\n ");
		printf("        algorithm: (0): enumeration, (1):greedySlow, (2):greedyFast, (3):enumeration-optimization, (4):user-defined(to add another string to define)\n");
		printf("			ex: p0 p1 p2 q3 r4 p5 r6 q7 q8 r9 q10 r11 means pppqrprqqrq\n");
		printf("        codeType : (0): RDP, (1):square array code-12*12, (2):not square-12*13\n");
		printf("        loglevel: (0): no log, (1):logInfo, (2):logDebug\n");
		printf("example:\n");
		printf("        ./%s 12 3 0 6 0 3 1 1(row=12,failt=3,s_row=0,s_col=6,lost_col=0, enumeration-optimization, square code, loglevel=info)\n", programName);
		exit(-1);
	}

	row = atoi(argv[1]);
	k = atoi(argv[2]);
	m = atoi(argv[3]);
	if (m != 3 && m != 2){
		LogFatal("bad params: row:%d k:%d, m:%d", row, k, m);
	}
	short_row = atoi(argv[4]);
	short_col = atoi(argv[5]);
	lost_col = atoi(argv[6]);
	algorithm = atoi(argv[7]);
	code_type = atoi(argv[8]);
	//loglevel = atoi(argv[9]);
	if (algorithm == 4 && argc < 11){
		LogFatal("Error: when algorithm == 4, it should provide a user defined parameter for recoveryStr like ppppqqqqrrrr");
	}
	g_RecoveryStr = (argv[10]);

	//参数合法性
	if (code_type == 0 ){//RDP
		if (k != row ||  (!isPrime(k + 1))){
			LogFatal("RDP bad params: row:%d k:%d, m:%d", row, k, m);
		}
	}
	else if (code_type == 1){//RDP-> PIT code
		if (k != row - 1 || (!isPrime(k + 1))){
			LogFatal("RDP->PIT code bad params: row:%d k:%d, m:%d\n", row, k, m);
		}
	}
	else if (code_type == 2){//EVEN -> PIT code
		if (k != row || (!isPrime(k))){
			LogFatal("EVENODD->PIT code bad params: row:%d k:%d, m:%d\n", row, k, m);
		}
	}
	LogNotice("%s %d %d %d %d %d %d %d %d %d", programName, row, k, m, short_row, short_col, lost_col, algorithm, code_type, loglevel);
}


