#ifndef __CS__CODING_SCHEME__H__
#define __CS__CODING_SCHEME__H__

#include "CS_Group.h"
#include "CS_Config.h"
#include "CS_RecoveryAlgorithm.h"
typedef vector< vector<Point> > CS_CodingSchemeRecoveryValue;

/*****************************
CodingScheme
编码：构造函数
解码：columnsLostRecovery

用户传入相应的数据列，可以使用构建好的恢复策略进行恢复

vector< vector<Point> > 表示的是恢复策略： 内部的一个vector表示一个group
中第一个单元Point为故障单元，其他单元为利用它进行恢复的单元，外部的vector的大小表示有多少个故障单元

每一个coding scheme负责此coding的所有故障恢复策略
*****************************/
extern vector < vector<Point> > _emptyRecoverySchemeValue;
class CS_CodingScheme
{
public:
	CS_CodingScheme(CSCodingSchemeType type, int row, int k, int m, int shorten_row = 0, int shorten_col = 0, int algorithm = 0, char* recoveryStr = NULL) :
		code_type(type), row(row), k(k), m(m), shorten_row(shorten_row), shorten_col(shorten_col), algorithm(algorithm), recoveryStr(recoveryStr)
	{
		set_shorten(shorten_row, shorten_col);
		printf("CS_CodingScheme constructed....\n");
	}
	CS_CodingScheme(CS_CodingSchemeKey& key) : CS_CodingScheme(key.type, key.row, key.k, key.m, key.shorten_row, key.shorten_col, key.algorithm, NULL){

	}
	CS_CodingScheme() { printf("CS_CodingScheme empty consturction called\n"); }
	~CS_CodingScheme(){}
public://下面根据参数构建相应的CodingScheme
	//create a new CodingScheme on the heap, so we can safely return the reference of internal variable (cs).
	static CS_CodingScheme* allocNewCodingScheme(CS_CodingSchemeKey& key){
		CS_CodingScheme *pcs = new CS_CodingScheme(key);
		return pcs;
	}
	//构建具有单故障恢复方案的CodingScheme
	static CS_CodingScheme* allocNewCodingSchemeWithSingleFailure(CS_CodingSchemeKey& key){
		CS_CodingScheme* cs = allocNewCodingScheme(key);
		cs->generateSingleFailureScheme(key);
		return cs;
	}
public:
	
	typedef unordered_map<CS_CodingSchemeRecoveryKey, vector< vector<Point> >, CS_CodingSchemeRecoveryKeyHasher> CS_CodingSchemeRecoveryMap;
	
	//单故障的预先存储
	void generateSingleFailureScheme(CS_CodingSchemeKey& key){
		int algorithm = key.algorithm;//默认采用遍历算法生成单故障最优，后面可根据配置文件进行生成
		for (int i = 0; i < k+m; ++i)
		{
			vector<int> vi{i};	
			CS_CodingSchemeRecoveryValue csr = columnsLostRecoveryScheme(vi, algorithm);
			singleFailure[algorithm][i] = (csr);
		}
	}
	//输入：故障的列，以及算法； 输出具体恢复的策略
	vector< vector<Point> >& recoveryDetailScheme(vector<int>& cols, int algorithm = 0){
		CS_CodingSchemeRecoveryKey key(cols, algorithm);
		return recoveryDetailScheme(key);
	}
	vector< vector<Point> >& recoveryDetailScheme(CS_CodingSchemeRecoveryKey& key){
		vector<int>& cols = key.cols;
		int algorithm = key.algorithm;
		if (cols.size() == 0 ) return _emptyRecoverySchemeValue ;
		if (cols.size() == 1 && (singleFailure[algorithm][cols[0]]).size() > 0) return singleFailure[algorithm][cols[0]];

		CS_CodingSchemeRecoveryMap::iterator it = detailSchemesCache.find(key);
		if (it != detailSchemesCache.end())  return it->second;

		//创建一个新的，插入到map中
		CS_CodingSchemeRecoveryValue csr = columnsLostRecoveryScheme(key);
		if(csr == _emptyRecoverySchemeValue) return csr; 
		detailSchemesCache.insert({ key, csr });
		return csr;
	}
public:
	static void printRecoveryScheme(CS_CodingSchemeRecoveryValue& rv){
		stringstream ss;
		ss << '{' << endl;
		bool isstart = true;
		for (auto vec : rv)
		{
			if (isstart){
				isstart = false;
			}
			else{
				ss << ',' << endl;
			}

			ss << '{';
			for (int i = 0; i < vec.size(); ++i)
			{
				Point& p = vec[i];
				ss << ((i==0) ? "" : ",") << '(' << p.i << ',' << p.j << ')';
			}
			ss << '}';
		}
		ss << endl<<'}' << endl;

		LogNotice("RecoveryScheme: \n%s", ss.str().c_str());
	}
public:
	void reset(CSCodingSchemeType type, int row, int k, int m, int shorten_row = 0, int shorten_col = 0);
	CS_RecoveryResult columnsLostRecovery(CS_CodingSchemeRecoveryKey& key);

	CS_CodingSchemeRecoveryValue columnsLostRecoveryScheme(vector<int>& cols, int algorithm){
		CS_CodingSchemeRecoveryKey key(cols, algorithm);
		return columnsLostRecoveryScheme(key);
	}
	//what to store in cache? maybe group
	CS_CodingSchemeRecoveryValue columnsLostRecoveryScheme(CS_CodingSchemeRecoveryKey& key){
		CS_RecoveryResult res = columnsLostRecovery(key);
		CS_CodingSchemeRecoveryValue csr;
		if (!res.canRecovery()) return csr;

		//单故障的时候估计还行，双故障的话算法需要进行修改，例如遍历的时候并不是每一个单元都可以被修复的
		//要求recoveryGrp的第一个单元是故障单元，其他单元是OK单元
		for (int i = 0; i < res.recoveryGrp.size(); ++i){
			csr.push_back(vector<Point>());
			Group* gp = res.recoveryGrp[i];
			for (auto j : gp->ve) csr[i].push_back({ j->i, j->j });
		}
		return csr;
	}
private:
	void set_shorten(short shorten_row, short shorten_col);
	void set_shorten_std_RDP(short shorten_row, short shorten_col);
	void set_shorten_nonstd_RDP(short shorten_row, short shorten_col);
	void set_shorten_nonstd_RDP_not_squre(short shorten_row, short shorten_col);

	void init(int r, int c);//no need to call (called in set_shorten)
	void set_matrix(short i, short j, short cost, short is_lost, short magic);
	void print_matrix(int islost);
private:
	Elem matrix[MAX_ROW][MAX_COL];
	GroupManager global_mg;
	int row, k, m;
	short shorten_row, shorten_col;
	char algorithm;
	char code_type;
	char * recoveryStr;
private:
	vector< vector<Point> > singleFailure[CSRecoveryAlgorithmTypeMax][MAX_COL];//单故障生成所有方案，直接保存。外部的vector表明那一列发生了故障
	CS_CodingSchemeRecoveryMap detailSchemesCache;
	
};

/******************************************
管理所有的coding scheme，
1，获取coding scheme
2，缓存coding scheme
3，从文件总读取coding scheme
4，写入coding scheme到文件
5，在分布式系统中，一般只会用到一种coding scheme，为了避免每次都需要寻找coding scheme，加入getCurrentCS方法。

Cache使用unordered_map， key是CS_CodingSchemeKey
******************************************/



class CS_CodingSchemeMgr
{
public:
	typedef unordered_map<CS_CodingSchemeKey, CS_CodingScheme*, CS_CodingSchemeKeyHasher> CS_CodingSchemeCacheMap;
	//唯一的暴露给外部的创建对象的接口，外部不能创建对象
	static CS_CodingSchemeMgr& getInstance(){
		static CS_CodingSchemeMgr mgr;
		return mgr;
	}
	~CS_CodingSchemeMgr(){
		for (auto i : m_csCache)
		{
			CS_CodingScheme* pcs = (i.second);
			delete pcs;
		}
	}
private://No copy
	CS_CodingSchemeMgr(){
		//从配置文件中构造coding schemes并设置默认的cs
		for (int i = 0; i < config.coding_schemes.size(); ++i){
			CS_CodingSchemeKey& key = config.coding_schemes[i];
			CS_CodingScheme& cs = getCodingScheme(key);
			if (key.is_default)  setCurrentCS(cs);
		}
	}
	CS_CodingSchemeMgr(CS_CodingSchemeMgr const&);
	void operator= (CS_CodingSchemeMgr const&);
public: //cache的设定
	void intoCache(CS_CodingSchemeKey& key, CS_CodingScheme* cs){
		m_csCache.insert({ key, cs });
	}
	CS_CodingScheme* outFromCache(CS_CodingSchemeKey& key){
		return m_csCache[key];
	}

	//首先查找cache，cache找不到后创建它
	CS_CodingScheme& getCodingScheme(CS_CodingSchemeKey& key){
		CS_CodingSchemeCacheMap::iterator it = m_csCache.find(key);
		if (it != m_csCache.end()) return *(it->second);
		
		//最后由外部配置文件决定，是否初始化并生成故障修复方案
		CS_CodingScheme* pcs = CS_CodingScheme::allocNewCodingSchemeWithSingleFailure(key);
		intoCache(key, pcs);
		return *pcs;
	}
private:
	CS_CodingSchemeCacheMap m_csCache;
public: //默认CodingScheme的设定
	static CS_CodingScheme& getCurrentCS(){
		return m_cs;
	}
	void setCurrentCS(CS_CodingScheme& cs){
		m_cs = cs;
	}
private:
	static CS_CodingScheme m_cs;
	CS_Config config;
};

#endif//__CS__CODING_SCHEME__H__
