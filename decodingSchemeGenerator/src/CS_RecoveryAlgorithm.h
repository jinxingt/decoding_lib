#ifndef __CS__RECOVERY__SCHEME__H__
#define __CS__RECOVERY__SCHEME__H__


#include "CS_Common.h"
#include "CS_Group.h"

class Elem;

/******************************************
����ʹ��CSRecoveryAlgorithmType = 1�� 3�� 4
1�� ����fast
3�� ̰��Fast
4�� �û��Զ���ָ�����
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
�ָ������У�ֻ��Ҫ����һ��Elem�����Լ����ϵĵ㣬�Լ����������ͣ��Ϳ����ҵ������Ľ����
���波�Խ�type���ж��ƶ����������

Ҫ���� ���ۣ��Լ���Ӧ�Ļָ�����
**************************************/

struct CSRecoveryResult
{
	CSRecoveryResult() : cost(INT_MAX){

	}
	bool canRecovery(){ return cost != INT_MAX; }
	int cost;
	unordered_map<Elem*, int> elemunordered_map; 
	vector<Group*> recoveryGrp; //ע���vector��˳��������ϸ������ݵ�Ԫ�Ļָ�˳��ģ��������й��ϣ���Ԫ֮��Ļָ������Ⱥ�ģ�
};

typedef CSRecoveryResult(*PFuncRecovery)(vector<Point>& points, Elem matrix[][MAX_COL]);

extern CSRecoveryResult enumerationRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);
//��������-�����ָ�
extern CSRecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//ÿ�ζ��Ǵ����еĹ��ϵ�Ԫ������У������ѡ��һ�����ϵ�Ԫ��Ȼ����ѡ����һ�����ϵ�Ԫ
extern CSRecoveryResult greedyRecoverySlow(vector<Point>& points, Elem matrix[][MAX_COL]);
extern CSRecoveryResult greedyRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//�û�����Ļָ����ԣ���Ҫ�ṩ������ppppqqqqrrrr�������ַ��� recoveryStr
extern CSRecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);

//�㷨�ָ��Ľ��
extern void
recoveryAlgorithmResult(unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp);
#endif//__CS__RECOVERY__SCHEME__H__