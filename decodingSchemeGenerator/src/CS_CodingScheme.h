#ifndef __CS__CODING_SCHEME__H__
#define __CS__CODING_SCHEME__H__

#include "CS_Group.h"
#include "CS_Config.h"
#include "CS_RecoveryAlgorithm.h"
typedef vector< vector<Point> > CS_CodingSchemeRecoveryValue;

/*****************************
CodingScheme
���룺���캯��
���룺columnsLostRecovery

�û�������Ӧ�������У�����ʹ�ù����õĻָ����Խ��лָ�

vector< vector<Point> > ��ʾ���ǻָ����ԣ� �ڲ���һ��vector��ʾһ��group
�е�һ����ԪPointΪ���ϵ�Ԫ��������ԪΪ���������лָ��ĵ�Ԫ���ⲿ��vector�Ĵ�С��ʾ�ж��ٸ����ϵ�Ԫ

ÿһ��coding scheme�����coding�����й��ϻָ�����
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
public://������ݲ���������Ӧ��CodingScheme
	//create a new CodingScheme on the heap, so we can safely return the reference of internal variable (cs).
	static CS_CodingScheme* allocNewCodingScheme(CS_CodingSchemeKey& key){
		CS_CodingScheme *pcs = new CS_CodingScheme(key);
		return pcs;
	}
	//�������е����ϻָ�������CodingScheme
	static CS_CodingScheme* allocNewCodingSchemeWithSingleFailure(CS_CodingSchemeKey& key){
		CS_CodingScheme* cs = allocNewCodingScheme(key);
		cs->generateSingleFailureScheme(key);
		return cs;
	}
public:
	
	typedef unordered_map<CS_CodingSchemeRecoveryKey, vector< vector<Point> >, CS_CodingSchemeRecoveryKeyHasher> CS_CodingSchemeRecoveryMap;
	
	//�����ϵ�Ԥ�ȴ洢
	void generateSingleFailureScheme(CS_CodingSchemeKey& key){
		int algorithm = key.algorithm;//Ĭ�ϲ��ñ����㷨���ɵ��������ţ�����ɸ��������ļ���������
		for (int i = 0; i < k+m; ++i)
		{
			vector<int> vi{i};	
			CS_CodingSchemeRecoveryValue csr = columnsLostRecoveryScheme(vi, algorithm);
			singleFailure[algorithm][i] = (csr);
		}
	}
	//���룺���ϵ��У��Լ��㷨�� �������ָ��Ĳ���
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

		//����һ���µģ����뵽map��
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

		//�����ϵ�ʱ����ƻ��У�˫���ϵĻ��㷨��Ҫ�����޸ģ����������ʱ�򲢲���ÿһ����Ԫ�����Ա��޸���
		//Ҫ��recoveryGrp�ĵ�һ����Ԫ�ǹ��ϵ�Ԫ��������Ԫ��OK��Ԫ
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
	vector< vector<Point> > singleFailure[CSRecoveryAlgorithmTypeMax][MAX_COL];//�������������з�����ֱ�ӱ��档�ⲿ��vector������һ�з����˹���
	CS_CodingSchemeRecoveryMap detailSchemesCache;
	
};

/******************************************
�������е�coding scheme��
1����ȡcoding scheme
2������coding scheme
3�����ļ��ܶ�ȡcoding scheme
4��д��coding scheme���ļ�
5���ڷֲ�ʽϵͳ�У�һ��ֻ���õ�һ��coding scheme��Ϊ�˱���ÿ�ζ���ҪѰ��coding scheme������getCurrentCS������

Cacheʹ��unordered_map�� key��CS_CodingSchemeKey
******************************************/



class CS_CodingSchemeMgr
{
public:
	typedef unordered_map<CS_CodingSchemeKey, CS_CodingScheme*, CS_CodingSchemeKeyHasher> CS_CodingSchemeCacheMap;
	//Ψһ�ı�¶���ⲿ�Ĵ�������Ľӿڣ��ⲿ���ܴ�������
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
		//�������ļ��й���coding schemes������Ĭ�ϵ�cs
		for (int i = 0; i < config.coding_schemes.size(); ++i){
			CS_CodingSchemeKey& key = config.coding_schemes[i];
			CS_CodingScheme& cs = getCodingScheme(key);
			if (key.is_default)  setCurrentCS(cs);
		}
	}
	CS_CodingSchemeMgr(CS_CodingSchemeMgr const&);
	void operator= (CS_CodingSchemeMgr const&);
public: //cache���趨
	void intoCache(CS_CodingSchemeKey& key, CS_CodingScheme* cs){
		m_csCache.insert({ key, cs });
	}
	CS_CodingScheme* outFromCache(CS_CodingSchemeKey& key){
		return m_csCache[key];
	}

	//���Ȳ���cache��cache�Ҳ����󴴽���
	CS_CodingScheme& getCodingScheme(CS_CodingSchemeKey& key){
		CS_CodingSchemeCacheMap::iterator it = m_csCache.find(key);
		if (it != m_csCache.end()) return *(it->second);
		
		//������ⲿ�����ļ��������Ƿ��ʼ�������ɹ����޸�����
		CS_CodingScheme* pcs = CS_CodingScheme::allocNewCodingSchemeWithSingleFailure(key);
		intoCache(key, pcs);
		return *pcs;
	}
private:
	CS_CodingSchemeCacheMap m_csCache;
public: //Ĭ��CodingScheme���趨
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
