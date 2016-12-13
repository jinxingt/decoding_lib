#ifndef __CS__RECOVERY__SCHEME__H__
#define __CS__RECOVERY__SCHEME__H__


#include "CS_Common.h"
#include "CS_Group.h"

class Elem;

/******************************************
建议使用CSRecoveryAlgorithmType = 1， 3， 4
1： 遍历fast
3： 贪心Fast
4： 用户自定义恢复策略
********************************************/
enum CSRecoveryAlgorithmType : char{
	CSRecoveryAlgorithmTypeEnumeration = 0,
	CSRecoveryAlgorithmTypeEnumerFast = 1,
	CSRecoveryAlgorithmTypeGreedySlow = 2,
	CSRecoveryAlgorithmTypeGreedyFast = 3,
	CSRecoveryAlgorithmTypeUserDefined = 4,
	CSRecoveryAlgorithmTypeMax,
};

/*************************************
恢复方案中，只需要接收一个Elem矩阵，以及故障的点，以及方案的类型，就可以找到方案的结果。
后面尝试将type的判定移动到这个里面

要返回 代价，以及相应的恢复方案
**************************************/

struct CSRecoveryResult
{
	CSRecoveryResult() : cost(INT_MAX){

	}
	bool canRecovery(){ return cost != INT_MAX; }
	int cost;
	unordered_map<Elem*, int> elemunordered_map; 
	vector<Group*> recoveryGrp; //注意此vector的顺序必须是严格按照数据单元的恢复顺序的（例如两列故障，单元之间的恢复是有先后的）
};

typedef CSRecoveryResult(*PFuncRecovery)(vector<Point>& points, Elem matrix[][MAX_COL]);

extern CSRecoveryResult enumerationRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);
//暴力搜索-遍历恢复
extern CSRecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//每次都是从所有的故障单元的所有校验组中选择一个故障单元，然后再选择下一个故障单元
extern CSRecoveryResult greedyRecoverySlow(vector<Point>& points, Elem matrix[][MAX_COL]);
extern CSRecoveryResult greedyRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//用户定义的恢复策略，需要提供类似于ppppqqqqrrrr这样的字符串 recoveryStr
extern CSRecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);

//算法恢复的结果
extern void
recoveryAlgorithmResult(unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp);
#endif//__CS__RECOVERY__SCHEME__H__