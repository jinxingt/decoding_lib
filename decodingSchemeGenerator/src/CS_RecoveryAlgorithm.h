#ifndef __CS__RECOVERY__SCHEME__H__
#define __CS__RECOVERY__SCHEME__H__

#include "CS_Time.h"
#include "CS_Common.h"
#include "CS_Group.h"

class Elem;

/******************************************
建议使用CSRecoveryAlgorithmType = 1， 3， 4
1： 遍历fast
3： 贪心Fast
4： 用户自定义恢复策略

算法恢复定义两大类型
1，搜索类（包括遍历，贪心等）： 如果用户直接指定CSRecoveryAlgorithmTypeSearch，那么会自动选择最合适的方案，或者用户强制指定遍历或者贪心，那么只会使用这些方案
2，用户自定义类型
********************************************/
enum CSRecoveryAlgorithmType : char{
	CSRecoveryAlgorithmTypeSearch = (1 << 5),
	CSRecoveryAlgorithmTypeEnumeration = CSRecoveryAlgorithmTypeSearch + 1,

	CSRecoveryAlgorithmTypeGreedy = CSRecoveryAlgorithmTypeSearch + (1 << 4),
	//CSRecoveryAlgorithmTypeEnumerFast = CSRecoveryAlgorithmTypeSearch + 1,
	CSRecoveryAlgorithmTypeGreedySlow = CSRecoveryAlgorithmTypeGreedy + 1,
	CSRecoveryAlgorithmTypeGreedyFast = CSRecoveryAlgorithmTypeGreedy + 2,
	CSRecoveryAlgorithmTypeGreedyRandom = CSRecoveryAlgorithmTypeGreedy + 3,

	CSRecoveryAlgorithmTypeUnique = (1 << 6),
	CSRecoveryAlgorithmTypeUserDefined = CSRecoveryAlgorithmTypeUnique + 0,
	CSRecoveryAlgorithmTypeMax,
};

//TODO
extern const char* getAlgTypeName(CSRecoveryAlgorithmType type);
/*************************************
恢复方案中，只需要接收一个Elem矩阵，以及故障的点，以及方案的类型，就可以找到方案的结果。
后面尝试将type的判定移动到这个里面

要返回 代价，以及相应的恢复方案
**************************************/

struct CS_RecoveryResult
{
	CS_RecoveryResult() : cost(INT_MAX){

	}
	bool canRecovery(){ return cost != INT_MAX; }
	int cost;
	unordered_map<Elem*, int> elemunordered_map; 
	vector<Group*> recoveryGrp; //注意此vector的顺序必须是严格按照数据单元的恢复顺序的（例如两列故障，单元之间的恢复是有先后的）

	//作为不能恢复的标记
	static const CS_RecoveryResult&  getEmptyResult(){
		static CS_RecoveryResult empty;
		return empty;
	}
	bool operator < (const CS_RecoveryResult& rhs) const{
		return cost < rhs.cost;
	}
};


typedef CS_RecoveryResult(*PFuncRecovery)(vector<Point>& points, Elem matrix[][MAX_COL]);

extern CS_RecoveryResult enumerationRecovery(vector<Point>& points, Elem matrix[][MAX_COL], long timeOutSeconds = -1);
//暴力搜索-遍历恢复
extern CS_RecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//每次都是从所有的故障单元的所有校验组中选择一个故障单元，然后再选择下一个故障单元
extern CS_RecoveryResult greedyRecoverySlow(vector<Point>& points, Elem matrix[][MAX_COL]);
extern CS_RecoveryResult greedyRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//用户定义的恢复策略，需要提供类似于ppppqqqqrrrr这样的字符串 recoveryStr
extern CS_RecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);

//算法恢复的结果
extern void
recoveryAlgorithmResult(unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp);


//解码算法的基类
class CS_RecoveryAlgBase
{
public:
	virtual CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]) = 0;
	virtual CS_RecoveryResult tryRecoveryWithTimeOut(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL], long timeOutSeconds = -1){ return CS_RecoveryResult::getEmptyResult(); }
public://给算法设置名字，便于调试
	void setName(const string& name){ _name = name; }
	const string&  getName() const { return _name;}
private:
	string _name;
};


//外部调用的方法
class CS_RecoveryAlgMgr
{
private://no copy
	CS_RecoveryAlgMgr(){
		installDefault();
	}
	void operator=(const CS_RecoveryAlgMgr&);
	CS_RecoveryAlgMgr(const CS_RecoveryAlgMgr&);
public:
	static CS_RecoveryAlgMgr& getMgr(){
		static CS_RecoveryAlgMgr mgr;
		return mgr;
	}
	void installDefault();
	inline void install(CSRecoveryAlgorithmType algtype, CS_RecoveryAlgBase* alg){
		_recoveryAlgMap.insert({ algtype, alg });
	}
	CS_RecoveryAlgBase* getAlg(CSRecoveryAlgorithmType type){
		unordered_map<CSRecoveryAlgorithmType, CS_RecoveryAlgBase*>::iterator it = _recoveryAlgMap.find(type);
		LogDebug("use alg (%d, %s) for recovery", type, getAlgTypeName(type));
		return it->second;
	}
public:
	/****************************************************
	外面调用的故障恢复方法：
	1， CSRecoveryAlgorithmTypeSearch： 先在1分钟之内进行遍历，1分钟到来之后就贪心（先慢速贪心，再随机贪心，再快速贪心）
	2， CSRecoveryAlgorithmTypeGreedy： 先慢速贪心，再随机贪心，再快速贪心
	3， CSRecoveryAlgorithmTypeEnumeration: 采用遍历方案，直到出结果
	4， CSRecoveryAlgorithmTypeGreedyFast： 快速贪心
	5， CSRecoveryAlgorithmTypeGreedySlow： 慢速贪心
	6， CSRecoveryAlgorithmTypeGreedyRandom： 随机贪心
	
	使用方法如下：
	CS_RecoveryAlgMgr& mgr = CS_RecoveryAlgMgr::getMgr();
	CS_RecoveryResult result = mgr.tryRecovery((CSRecoveryAlgorithmType)algorithm, lost_points, matrix);
	****************************************************/
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		unordered_map<CSRecoveryAlgorithmType, CS_RecoveryAlgBase*>::iterator it = _recoveryAlgMap.find(algtype);

		if (it == _recoveryAlgMap.end()){
			LogError("can not find the algorithm for the type: %d\n", algtype, getAlgTypeName(algtype));
			return CS_RecoveryResult::getEmptyResult();
		}
		CS_RecoveryAlgBase* alg = it->second;

		return alg->tryRecovery(algtype, points, matrix);
	}

private:
	unordered_map<CSRecoveryAlgorithmType, CS_RecoveryAlgBase*> _recoveryAlgMap;
};

//解码算法的派生类，遍历类型(外部使用)
class CS_SearchRecoveryEnumeration : public CS_RecoveryAlgBase
{
public:
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype,vector<Point>& points, Elem matrix[][MAX_COL]){
		return enumerationRecovery(points, matrix, -1);
	}
	CS_RecoveryResult tryRecoveryWithTimeOut(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL], long timeOutSeconds = -1){
		return enumerationRecovery(points, matrix, timeOutSeconds);
	}
};
//解码算法的派生类，贪心类型
class CS_SearchRecoveryGreedyFast : public CS_RecoveryAlgBase
{
public:
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype,vector<Point>& points, Elem matrix[][MAX_COL]){
		return greedyRecoveryFast(points, matrix);
	}
};
class CS_SearchRecoveryGreedyRandom : public CS_RecoveryAlgBase
{
public:
	//TODO，自动多次随机
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		return greedyRecoveryFast(points, matrix);
	}
};
class CS_SearchRecoveryGreedySlow : public CS_RecoveryAlgBase
{
public:
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		return greedyRecoverySlow(points, matrix);
	}
};

//解码算法的派生类，贪心类统一(外部使用)
class CS_SearchRecoveryGreedy : public CS_RecoveryAlgBase
{
	/****
	首先使用贪心慢速，再使用随机,再使用贪心快速，以最优结果作为最终结果
	***/
public:
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		CS_RecoveryAlgMgr& mgr = CS_RecoveryAlgMgr::getMgr();
		CSRecoveryAlgorithmType types[] = { CSRecoveryAlgorithmTypeGreedySlow, CSRecoveryAlgorithmTypeGreedyRandom, CSRecoveryAlgorithmTypeGreedyFast };
		CS_RecoveryResult resMin = mgr.getAlg(types[0])->tryRecovery(algtype, points, matrix);
		for (int i = 1; i < sizeof(types) / sizeof(types[0]); ++i)
		{
			CS_RecoveryResult res2 = mgr.getAlg(types[0])->tryRecovery(algtype, points, matrix);
			if (res2 < resMin)
			{
				resMin = res2;
			}
		}
		
		return resMin;
	}
};

//(外部使用)
class CS_SearchRecovery : public CS_RecoveryAlgBase
{
	/**************
	首先尝试使用遍历方法，如果在1分钟之内遍历不出来，那么改用贪心算法
	*****************/
	virtual CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		CS_RecoveryAlgMgr& mgr = CS_RecoveryAlgMgr::getMgr();
		int timeOutSeconds = 60;//60s

		CS_RecoveryResult resMin = mgr.getAlg(CSRecoveryAlgorithmTypeEnumeration)->tryRecoveryWithTimeOut(algtype, points, matrix, timeOutSeconds);
		if (resMin.canRecovery()) {
			return resMin;
		}

		LogNotice("can not recovery using (%d, %s) in %d seconds, begin to try greedy methods instead", CSRecoveryAlgorithmTypeEnumeration, getAlgTypeName(CSRecoveryAlgorithmTypeEnumeration), timeOutSeconds);
		CS_RecoveryResult res2 = mgr.getAlg(CSRecoveryAlgorithmTypeGreedy)->tryRecovery(algtype, points, matrix);
		if (res2 < resMin)
		{
			resMin = res2;
		}
		return resMin;
	}
};
class CS_UniqueRecovery : public CS_RecoveryAlgBase
{
public:
	CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]){
		return userdefinedRecovery(points, matrix);
	}

};


#endif//__CS__RECOVERY__SCHEME__H__