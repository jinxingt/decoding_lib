#ifndef __CS__RECOVERY__SCHEME__H__
#define __CS__RECOVERY__SCHEME__H__

#include "CS_Time.h"
#include "CS_Common.h"
#include "CS_Group.h"

class Elem;

/******************************************
����ʹ��CSRecoveryAlgorithmType = 1�� 3�� 4
1�� ����fast
3�� ̰��Fast
4�� �û��Զ���ָ�����

�㷨�ָ�������������
1�������ࣨ����������̰�ĵȣ��� ����û�ֱ��ָ��CSRecoveryAlgorithmTypeSearch����ô���Զ�ѡ������ʵķ����������û�ǿ��ָ����������̰�ģ���ôֻ��ʹ����Щ����
2���û��Զ�������
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
�ָ������У�ֻ��Ҫ����һ��Elem�����Լ����ϵĵ㣬�Լ����������ͣ��Ϳ����ҵ������Ľ����
���波�Խ�type���ж��ƶ����������

Ҫ���� ���ۣ��Լ���Ӧ�Ļָ�����
**************************************/

struct CS_RecoveryResult
{
	CS_RecoveryResult() : cost(INT_MAX){

	}
	bool canRecovery(){ return cost != INT_MAX; }
	int cost;
	unordered_map<Elem*, int> elemunordered_map; 
	vector<Group*> recoveryGrp; //ע���vector��˳��������ϸ������ݵ�Ԫ�Ļָ�˳��ģ��������й��ϣ���Ԫ֮��Ļָ������Ⱥ�ģ�

	//��Ϊ���ָܻ��ı��
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
//��������-�����ָ�
extern CS_RecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//ÿ�ζ��Ǵ����еĹ��ϵ�Ԫ������У������ѡ��һ�����ϵ�Ԫ��Ȼ����ѡ����һ�����ϵ�Ԫ
extern CS_RecoveryResult greedyRecoverySlow(vector<Point>& points, Elem matrix[][MAX_COL]);
extern CS_RecoveryResult greedyRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]);

//�û�����Ļָ����ԣ���Ҫ�ṩ������ppppqqqqrrrr�������ַ��� recoveryStr
extern CS_RecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]);

//�㷨�ָ��Ľ��
extern void
recoveryAlgorithmResult(unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp);


//�����㷨�Ļ���
class CS_RecoveryAlgBase
{
public:
	virtual CS_RecoveryResult tryRecovery(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL]) = 0;
	virtual CS_RecoveryResult tryRecoveryWithTimeOut(CSRecoveryAlgorithmType algtype, vector<Point>& points, Elem matrix[][MAX_COL], long timeOutSeconds = -1){ return CS_RecoveryResult::getEmptyResult(); }
public://���㷨�������֣����ڵ���
	void setName(const string& name){ _name = name; }
	const string&  getName() const { return _name;}
private:
	string _name;
};


//�ⲿ���õķ���
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
	������õĹ��ϻָ�������
	1�� CSRecoveryAlgorithmTypeSearch�� ����1����֮�ڽ��б�����1���ӵ���֮���̰�ģ�������̰�ģ������̰�ģ��ٿ���̰�ģ�
	2�� CSRecoveryAlgorithmTypeGreedy�� ������̰�ģ������̰�ģ��ٿ���̰��
	3�� CSRecoveryAlgorithmTypeEnumeration: ���ñ���������ֱ�������
	4�� CSRecoveryAlgorithmTypeGreedyFast�� ����̰��
	5�� CSRecoveryAlgorithmTypeGreedySlow�� ����̰��
	6�� CSRecoveryAlgorithmTypeGreedyRandom�� ���̰��
	
	ʹ�÷������£�
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

//�����㷨�������࣬��������(�ⲿʹ��)
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
//�����㷨�������̰࣬������
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
	//TODO���Զ�������
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

//�����㷨�������̰࣬����ͳһ(�ⲿʹ��)
class CS_SearchRecoveryGreedy : public CS_RecoveryAlgBase
{
	/****
	����ʹ��̰�����٣���ʹ�����,��ʹ��̰�Ŀ��٣������Ž����Ϊ���ս��
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

//(�ⲿʹ��)
class CS_SearchRecovery : public CS_RecoveryAlgBase
{
	/**************
	���ȳ���ʹ�ñ��������������1����֮�ڱ�������������ô����̰���㷨
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