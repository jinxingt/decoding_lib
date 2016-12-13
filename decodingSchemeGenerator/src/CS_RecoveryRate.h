#ifndef _CS_RECOVERY_RATE__H__
#define _CS_RECOVERY_RATE__H__

#include "CS_Group.h"
#include "CS_CodingScheme.h"
#include "CS_RecoveryAlgorithm.h"

struct RecoveryRateResult
{
	int total; //��������
	int recovery_total; //�ָ�����
	double total_cost; //�ܹ��Ĵ���
	double rate; //�ָ��ı���
	double average_cost;//ƽ������

	void calculate(){
		rate = recovery_total - average_cost;
		average_cost = total_cost / recovery_total;
	}
};
class CSRecoveryRate
{
public:
	CSRecoveryRate(CSCodingSchemeType type, int row, int k, int m, int shorten_row = 0, int shorten_col = 0, int algorithm = 0, char* recoveryStr = NULL) :coding_scheme(type,row,k,m,shorten_row,shorten_col,algorithm,recoveryStr)
	{
		this->shorten_col = shorten_col;
		this->row = row; this->k = k; this->m = m; this->shorten_row = shorten_row; this->shorten_col = shorten_col;
	}
public:
	//�ָ���:���ݹ����������������еĹ��������include_parity��ʾ�Ƿ����У���еĹ��ϣ���Ȼ������ָ�
	RecoveryRateResult recoveryRate(short lost_num, bool include_parity = false);
private:
	int doRecovery(vector<int> cols, RecoveryRateResult& result);
private:
	int row, k, m;
	short shorten_row, shorten_col;
	//ʹ�õĻָ�����
	CS_CodingScheme coding_scheme;
};

int CSRecoveryRate::doRecovery(vector<int> cols, RecoveryRateResult& result){
	//to do. use cache to return a scheme. whether to use group*?
	CS_CodingSchemeRecoveryKey key(cols, 0);
	CS_RecoveryResult res = coding_scheme.columnsLostRecovery(key);
	if (res.canRecovery()) result.recovery_total++;
	result.total++;
	result.total_cost += res.cost;

	return res.cost;
}
RecoveryRateResult CSRecoveryRate::recoveryRate(short lost_num, bool include_parity)
{
	//there are <lost_num> columns lost in (col-shorten_col)
	RecoveryRateResult result;
	int col = k + m;

	short i, j, k, m, start = shorten_col;
	if (lost_num == 1){
		for (i = start; i < col; ++i) doRecovery({ i }, result);
	}
	else if (lost_num == 2){
		for (i = start; i < col; ++i)
		{
			for (j = i + 1; j < col; ++j)doRecovery({ i, j }, result);
		}
	}
	else if (lost_num == 3){
		for (i = start; i < col; ++i)
		{
			for (j = i + 1; j < col; ++j)
			{
				for (k = j + 1; k < col; ++k) doRecovery({ i,j, k }, result);
			}
		}
	}
	else if (lost_num == 4){
		for (i = start; i < col; ++i)
		{
			for (j = i + 1; j < col; ++j)
			{
				for (k = j + 1; k < col; ++k)
				{
					for (m = k + 1; m < col; ++m)doRecovery({ i, j, k, m }, result);
				}
			}
		}
	}
	else{
		printf("error: unsupported for 5 failures\n");
		exit(-1);
	}


	return result;
}

#endif//_CS_RECOVERY_RATE__H__
