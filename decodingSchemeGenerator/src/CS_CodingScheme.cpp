#include "CS_Common.h"
#include "CS_RecoveryAlgorithm.h"
#include "CS_CodingScheme.h"
#include "CS_Group.h"
#include "CS_Time.h"

//  /usr/local/bin/parallel -j 8 head -n {1} run_cycle.txt| tail -n 1 | bash > res28p{1}.txt

/*******************************
考虑将算法作为CS_CodingScheme的内部类
算法类本身并不暴露到外面，由CS_CodingScheme类的构造函数传入的算法类型决定选择算法


CS_CodingScheme类
利用算法类得到恢复方案，或者不可恢复的标记，如果可恢复则能够存储到hash表中
提供函数：能够将方案保存到文件中
能够从文件中读取到相应的方案

CS_CodingScheme类
1，初始化从文件中载入方案
2，将方案存入到hash表中
3，支持外部参数一次生成所有的恢复方案（故障数目小于等于校验块数目的方案）

1，外部请求相应的恢复策略
	1.1 首先请求hash表中是否存储有此方案，如果有直接返回
	1.2 如果hash表中找不到此方案，则调用算法类构造方案，并返回此方案，同时将新的方案存入到hash表，注意不可恢复的方案也是方案


*******************************/

const short magic_number = (short)0xA5A5A;
vector < vector<Point> > _emptyRecoverySchemeValue;



//return: INT_MAX for can not recovery. others means recovery cost
CSRecoveryResult CS_CodingScheme::columnsLostWithRecoveryFunc(vector<int>& cols, PFuncRecovery pfunc)
{
	vector<Point> lost_points;

	//set
	for (int p = 0; p < row - shorten_row; p++){
		for (int q : cols)
		{
			matrix[p][q].is_lost = 1;
			lost_points.push_back(Point(p, q));
		}
	}
#if ENABLE_DEBUG
	printf("Lost Circumstances...\n");
	print_matrix(1);
	print_matrix(2);
	global_mg.print();
#endif
	//process
	CSRecoveryResult result;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	if (algorithm == 4)
	{
	}
	result = (*pfunc)(lost_points, matrix);
	int cost = result.cost;
	gettimeofday(&end, NULL);
	long timediff = timeDiffMacroSeconds(start, end);

	if (cost == INT_MAX){
		LogError("can not recovery: columns\n");
		LogError("time cost %s, %ld us\n", timeFormatMacroToString(timediff), timediff);
	}
	else{
		LogNotice("recovery [%d, %d] mincost:%2d columns: (%d) time cost %s, %ld us", k, m, cost, cols[0], timeFormatMacroToString(timediff), timediff);
		
		for (int i = 0; i < result.recoveryGrp.size(); ++i)
		{
			Group* gp = result.recoveryGrp[i];
			Point p = lost_points[i];
			//处理recoveryGrp,使其第一个单元为故障单元，简单说来将其移动到数组开头
			for (int j = 0; j < gp->ve.size(); ++j)
			{
				Elem* e = gp->ve[j];
				if (e->i == p.i && e->j == p.j){
					gp->ve[j] = gp->ve[0];
					gp->ve[0] = e;
					break;
				}
			}
		}
	}

	//reset
	for (int p = 0; p < row - shorten_row; p++){
		for (int q : cols)
		{
			matrix[p][q].is_lost = 0;
		}
	}
	return result;
}
CSRecoveryResult CS_CodingScheme::columnsLostRecovery(CS_CodingSchemeRecoveryKey& key)
{
	PFuncRecovery pfunc = NULL;
	switch (key.algorithm){
	case 0:
		pfunc = enumerationRecovery;
		break;
	case 1:
		pfunc = greedyRecoverySlow;
		break;
	case 2:
		pfunc = greedyRecoveryFast;
		break;
	case 3:
		pfunc = enumerationRecoveryFast;
		break;
	case 4:
		pfunc = userdefinedRecovery;
	default:
		printf("Error, unknown algorithm %d\n", algorithm);
		break;
	}
	return columnsLostWithRecoveryFunc(key.cols, pfunc);
	
}

/******************************
外部公共函数，用来重置编码方式
*******************************/
void CS_CodingScheme::reset(CSCodingSchemeType type, int row, int k, int m, int shorten_row, int shorten_col){
	code_type = type;
	this->row = row; this->k = k; this->m = m; this->shorten_row = shorten_row; this->shorten_col = shorten_col;
	set_shorten(this->shorten_row, this->shorten_col);
}

/******************************
下面的是属于3种编码进行构造的代码
******************************/
void CS_CodingScheme::set_matrix(short i, short j, short cost, short is_lost, short magic)
{
	matrix[i][j].i = i;
	matrix[i][j].j = j;
	matrix[i][j].cost = cost;
	matrix[i][j].is_lost = is_lost;
	matrix[i][j].magic = magic;
}
void CS_CodingScheme::init(int r, int c)
{
	short i, j, k;
	//row+1表明考虑了非标准RDP的两个校验块
	for (i = 0; i < r; ++i)
	{
		for (j = 0; j < c; ++j){
			set_matrix(i, j, 1, 0, magic_number);
		}
	}
}
/*
功能：对标准RDP进行处理，设置校验组的
simple to shorten last rows, left cols
举例：
matrix...
0 1 2 3  p q r
______________
x 1 3 1  1 0 0
x 2 1 2  1 0 0
x 0 0 0  1 1 0
x 0 0 0  1 0 1
*/
void CS_CodingScheme::set_shorten_std_RDP(short shorten_row, short shorten_col)
{
	init(row, k+m);

	int i, j;
	//colomns
	for (i = 0; i < row; ++i){
		for (j = row - shorten_col; j < row; ++j)
		{
			matrix[i][j].cost = 0;
		}
	}

	//rows
	for (i = row - shorten_row; i < row; ++i)
	{
		for (j = 0; j < row + 1; ++j){
			matrix[i][j].cost = 0;
		}
	}

	//ok, after shorten, let's see what it is now.
#if ENABLE_DEBUG
	if (loglevel >= 2)
		print_matrix();
#endif

	/*called in the main
	* * * * p q r
	* * * * p q r
	* * * * p q r
	q r
	*/
	for (int i = 0; i < row - shorten_row; ++i)//p
	{
		int j = row;
		Group *gp = global_mg.newGroup();
		gp->addElem(&matrix[i][j]);
		gp->setName("p", i);
		for (int k = 0; k < row; k++)
		{
			gp->addElem(&matrix[i][k]);
		}
	}
	for (int i = 0; i < row; ++i)//q
	{
		int j = row + 1;
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("q", i);
		gp->addElem(&matrix[i][j]);
		//row=4: (00, 14,23,32,q0), (01,10,24,33,q1)
		for (int k = 0; k < row; k++)//a group
		{
			int k2 = ((row + 1 + i) - k) % (row + 1);
			gp->addElem(&matrix[k][k2]);
		}
	}
	for (int i = 0; i < row; ++i)//r
	{
		int j = row + 2;
		//jump the shorten
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("r", i);
		//row=4: (00, 11,22,33,r0), (10,21,32,04,r1), (20,31,03,14,r2), (30,02,13,24)
		gp->addElem(&matrix[i][j]);
		for (int k = 0; k < row; k++)//a group
		{
			//((k + 5) - k2) % (5) == i
			int k2 = (k + row + 1 - i) % (row + 1);
			gp->addElem(&matrix[k][k2]);
		}
	}
#if ENABLE_DEBUG
	if (loglevel >= 2)
		global_mg.print();
#endif
	return;
}
/*
功能：缩减相应的行和列，并构造校验组，此方法为非标准RDP的构造方法
这个数据阵列为方阵形式
放置一个4*4的例子
matrix...
0 1 2 3  p q r
______________
x 1 3 1  1 0 0
x 2 1 2  1 0 0
x 0 0 0  0 1 0
x 0 0 0  0 0 1
0 0 0 0  0 0 0
*/
void CS_CodingScheme::set_shorten_nonstd_RDP(short shorten_row, short shorten_col)
{
	init(row, k+m);
	// old_row + 1 = new_row
	// old_row = k

	int i, j;
	//colomns
	for (i = 0; i < row-1; ++i){
		for (j = k - shorten_col; j < k; ++j)
		{
			matrix[i][j].cost = 0;
		}
	}

	//rows 行缩减，缩减后6列
	for (i = row-1 - shorten_row; i < row-1; ++i)
	{
		for (j = 0; j < k + 1; ++j){
			matrix[i][j].cost = 0;
		}
	}

	//ok, after shorten, let's see what it is now.
#if ENABLE_DEBUG
	if (loglevel >= 2)
		print_matrix();
#endif

	/*called in the main
	* * * * p q r
	* * * * p q r
	* * * * p q r
	q r
	*/
	for (int i = 0; i < row -1 - shorten_row; ++i)//p
	{
		int j = row - 1;
		Group *gp = global_mg.newGroup();
		gp->addElem(&matrix[i][j]);
		gp->setName("p", i);
		for (int k = 0; k < row-1; k++)
		{
			gp->addElem(&matrix[i][k]);
		}
	}
	for (int i = 0; i < row; ++i)//q
	{
		int j = row;
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("q", i);
		gp->addElem(&matrix[i][j]);
		//row=4: (00,23,32,q0), (01,10,33,q1),...,(q4,13,22,31)
		for (int k = 0; k < row-1; k++)//a group
		{
			int k2 = ((row + i) - k) % (row);
			if (k2 == row-1)
				continue;//not standard RDP, ignore the 'p' parity
			gp->addElem(&matrix[k][k2]);
		}
	}
	for (int i = 0; i < row; ++i)//r
	{
		int j = row -1 + 2;
		//jump the shorten
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("r", i);
		//row=4: (00, 11,22,33,r0), (10,21,32,r1), (20,31,03,r2), (30,02,13,r3),(01,12,23,r4)
		gp->addElem(&matrix[i][j]);
		for (int k = 0; k < row-1; k++)//a group
		{
			//((k + 5) - k2) % (5) == i
			int k2 = (k + row - i) % (row);
			if (k2 == row-1)
				continue;//not standard RDP, ignore the 'p' parity
			gp->addElem(&matrix[k][k2]);
		}
	}
#if ENABLE_DEBUG
	if (loglevel >= 2)
		global_mg.print();
#endif
	return;
}

/*
功能：缩减相应的行和列，并构造校验组，此方法为非标准RDP的构造方法
这个数据阵列为不是方阵形式，多额外的一列
放置一个4*5的例子
matrix...
0 1 2 3 4  p q r
________________
x 0 0 2 1  0 1 0
x 0 1 0 2  0 0 1
x 2 0 1 0  0 0 1
x 1 2 0 0  0 1 0
0 0 0 0 0  0 0 0
*/
void CS_CodingScheme::set_shorten_nonstd_RDP_not_squre(short shorten_row, short shorten_col)
{
	//old_row + 1 = new_row
	init(row, k+m);

	int i, j;
	//colomns
	for (i = 0; i < row-1; ++i){
		for (j = row  - shorten_col; j < row-1; ++j)
		{
			matrix[i][j].cost = 0;
		}
	}

	//rows 行缩减，缩减后6列
	for (i = row-1 - shorten_row; i < row-1; ++i)
	{
		for (j = 0; j < row -1 + 2; ++j){
			matrix[i][j].cost = 0;
		}
	}

	//ok, after shorten, let's see what it is now.
#if ENABLE_DEBUG
	if (loglevel == 2)
		print_matrix();
#endif

	/*called in the main
	* * * * p q r
	* * * * p q r
	* * * * p q r
	q r
	*/
	int col = k + m;
	for (int i = 0; i < row -1 - shorten_row; ++i)//p
	{
		int j = col - m;
		Group *gp = global_mg.newGroup();
		gp->addElem(&matrix[i][j]);
		gp->setName("p", i);
		for (int k = 0; k < col - m; k++)
		{
			gp->addElem(&matrix[i][k]);
		}
	}
	for (int i = 0; i < row; ++i)//q
	{
		int j = col - m + 1;
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("q", i);
		gp->addElem(&matrix[i][j]);
		//row=4: (00, 14,23,32,q0), (01,10,24,33,q1)
		for (k = 0; k < row-1; k++)//a group
		{
			int k2 = ((row + i) - k) % (row + 1);
			if (k2 == col - m)
				continue;//not standard RDP, ignore the 'p' parity
			gp->addElem(&matrix[k][k2]);
		}
	}
	for (int i = 0; i < row; ++i)//r
	{
		int j = col - m + 2;
		//jump the shorten
		if (matrix[i][j].cost == 0)
			continue;
		Group *gp = global_mg.newGroup();
		gp->setName("r", i);
		//row=4: (00, 11,22,33,r0), (10,21,32,04,r1), (20,31,03,14,r2), (30,02,13,24)
		gp->addElem(&matrix[i][j]);
		for (k = 0; k < row-1; k++)//a group
		{
			//((k + 5) - k2) % (5) == i
			int k2 = (k + row - i) % (row );
			if (k2 == col - m)
				continue;//not standard RDP, ignore the 'p' parity
			gp->addElem(&matrix[k][k2]);
		}
	}
#if ENABLE_DEBUG
	if (loglevel >= 2)
		global_mg.print();
#endif
	return;
}

/*
CSCodingSchemeTypeRDP = 0,
CSCodingSchemeTypePIT = 1,
CSCodingSchemeTypeEVENODD = 2,
*/
void CS_CodingScheme::set_shorten(short shorten_row, short shorten_col)
{
	if (code_type == CSCodingSchemeTypeRDP){
		set_shorten_std_RDP(shorten_row, shorten_col);
	}
	else if (code_type == CSCodingSchemeTypePIT){
		set_shorten_nonstd_RDP(shorten_row, shorten_col);
	}
	else if (code_type == CSCodingSchemeTypeEVENODD){
		set_shorten_nonstd_RDP_not_squre(shorten_row, shorten_col);
	}
	else{
		printf("Error: unknown code type %d\n", code_type);
		exit(-1);
	}
}

void CS_CodingScheme::print_matrix(int islost)
{
	short i, j;
	const char *lable = " cost";
	if (islost == 1){
		lable = " is_lost";
	}
	else if (islost == 2){
		lable = " dimensions";
	}
	printf("matrix%s: %d %d %d %d %d\n", lable, row, k, m, shorten_row, shorten_col);

	int col = k + m, fail_tol = m;
	for (i = 0; i < row; ++i)
	{
		for (j = 0; j < col; ++j){
			if (islost == 0){
				printf("%2d%s", matrix[i][j].cost, (j == col - fail_tol - 1) ? ("  ") : (" "));
			}
			else if (islost == 1){
				printf("%2d%s", matrix[i][j].is_lost, (j == col - fail_tol - 1) ? ("  ") : (" "));
			}
			else if (islost == 2){
				if (j <= col - fail_tol - 1){
					printf("(%d,%d)%s", i, j, (j == col - fail_tol - 1) ? ("  ") : (" "));
				}
				else{
					const char* str = "pqr";
					printf("(%d,%d ", i, j);
					printf("%c%d", str[j - (col - fail_tol)], i);
					printf(") ");
				}

			}
		}
		printf("\n");
	}
}

/**********************************************
CS_CodingSchemeMgr类
**********************************************/
CS_CodingScheme CS_CodingSchemeMgr::m_cs;
