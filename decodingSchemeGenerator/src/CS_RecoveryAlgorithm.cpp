#include "CS_RecoveryAlgorithm.h"
#include "CS_Group.h"


static vector<char> constructParityVec(int i)
{
	int row = 13 - 1;
	vector<char> parityVec(row);
	//first average distributed
	int p = 0, q = 0, r = 0, j = 0;
	r = row / 3;
	q = (row - r) / 2;
	p = row - r - q;

	j = (row - i) % row;
	for (int k = 0; k < p; k++)
		parityVec[(j++) % row] = 'p';
	for (int k = 0; k < q; k++)
		parityVec[(j++) % row] = 'q';
	for (int k = 0; k < r; k++)
		parityVec[(j++) % row] = 'r';

	return parityVec;
}
/*
���ܣ���ӡ���Ž��
*/
void recoveryAlgorithmResult(unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp){
#if ENABLE_DEBUG
	if (loglevel == 0)
		return;//no log

	int col = k + m, fail_tol = m;
	if (elemunordered_map.size() >= 0){
		printf("brief information...\n");
		int p = 0, q = 0, r = 0;
		for (int i = 0; i < recoveryGrp.size(); ++i){//use row = 12
			Group * gp = recoveryGrp[i];
			const char* name = gp->name.c_str();
			if (name[0] == 'p') p++;
			if (name[0] == 'q') q++;
			if (name[0] == 'r') r++;
		}
		int columnElemCount[100] = {0};
		for (unordered_map<Elem*, int>::iterator it = elemunordered_map.begin(); it != elemunordered_map.end(); ++it){
			Elem* e = it->first;
			columnElemCount[e->j]++;
		}

		//��ӡ�ָ���������ʹ�õ�У����
		for (int i = 0; i < recoveryGrp.size(); ++i){
			Group * gp = recoveryGrp[i];
			const char* name = gp->name.c_str();
			if (name[0] == 'p') p++;
			if (name[0] == 'q') q++;
			if (name[0] == 'r') r++;
			printf("%s ", gp->name.c_str());
		}
		printf("(%d, %d, %d)", p, q, r);
		printf("\n");

		//��ӡ���ô���Ϊ1��2��3�����ݿ����
		int rdpcost = (3 * (row-1 - short_row)*(k - short_col)) / 4;
		int origincost = (row-1 - short_row)*(k - short_col);
		printf("cost: %d, rdpcost:%d origin:%d rate:%.4f ", elemunordered_map.size(), rdpcost, origincost, (origincost - elemunordered_map.size())*1.0 / origincost);
		int replicas[4];
		memset(replicas, 0, 4 * 4);
		for (unordered_map<Elem*, int>::iterator it = elemunordered_map.begin(); it != elemunordered_map.end(); ++it){
			Elem* e = it->first;
			int rep = it->second;
			replicas[rep]++;
		}
		printf("(%d, %d, %d, total:%d, copy:%d)", replicas[1], replicas[2], replicas[3], replicas[1] + replicas[2] + replicas[3], replicas[2] + replicas[3]);
		printf("\n");

		//��ӡÿһ�����ݿ�ĸ��þ��󣬾���Ԫ�ص�����Ϊ���õĴ���
		printf("matrix...\n");
		for (int j = 0; j < k; ++j) printf("%d ", j);
		printf(" p q r \n");
		for (int j = 0; j < k+m; ++j) printf("__");
		printf("\n");

		for (int i = 0; i < row; ++i){
			for (int j = 0; j < k+m; ++j){
				Elem* e = &matrix[i][j];
				if (e->is_lost) printf("x ");
				else{
					unordered_map<Elem*, int>::iterator it = elemunordered_map.find(e);
					if (it == elemunordered_map.end()) printf("0 ");
					else printf("%d ", it->second);
				}
				if (j == col - fail_tol - 1) printf(" ");


			}
			printf("\n");
		}

		//��ӡÿһ����ʹ�õ�Ԫ�ظ��������ؾ�����ܻ�����
		printf("columns cost:\n");
		for (int i = 0; i < col; ++i){
			printf("%2d ", i);
		}
		printf("\n");
		for (int i = 0; i < col; ++i){
			printf("%2d ", columnElemCount[i]);
		}
		printf("\n");
	}
#endif// ENABLE_DEBUG
}

/*
���ܣ������ָ�Ѱ�����Ž����ʹ�õ��ǵݹ����

û�Ż�֮ǰ��
rdor.exe 8 3 0 0 0 0   ����ʱ��3.213s
��һ���Ż��������е�map�޸�Ϊunordered_map������ʱ���Ϊ 2.556 s
�ڶ����Ż����������еĲ�����������(minMap, minGroup)������ʹ����������ʱ���Ϊ2.17s(δ����)
�������Ż���ɾ��lost_num���Ŵ����б�����ʱ��Ϊ2.185
���Ĵ��Ż�������mapԪ����ȡ��elem�Ĳ��������޸�һ�£�������iterator������ʱ���Ϊ1.50s ����һ��
*/
int _enumerationRecoverySearching(vector<Point>& points, int i, unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp, int& minCost,
	unordered_map<Elem*, int>& minunordered_map, vector<Group*>& minGroup, Elem matrix[][MAX_COL], struct timeval begin, long timeOutSeconds){
	//construct an array of identiy which parity is used something like "ppppqqqrrr"

	//�����õļ���Ƶ��
	if (timeOutSeconds != -1)
	{
		struct timeval end;
		gettimeofday(&end, NULL);
		if (timeDiffSeconds(begin, end) >= timeOutSeconds)
		{
			minCost = INT_MAX;
			return INT_MAX;
		}
	}

	if (i == points.size()){	
		if (minCost > elemunordered_map.size()){
			minCost = elemunordered_map.size();
			minunordered_map = elemunordered_map;
			minGroup = recoveryGrp;
		}
		return minCost;
	}
	//process points[i];
	Elem *e = &matrix[points[i].i][points[i].j];
	for (int j = 0; j < e->ownerGrp.size(); ++j){
		//use it for recovery
		Group* gp = e->ownerGrp[j];
		vector<Elem*>& ve = gp->ve;

		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (!e->is_lost){
				elemunordered_map[e]++;
			}
		}
		recoveryGrp.push_back(gp);
		//next iteration recursive
		_enumerationRecoverySearching(points, i + 1, elemunordered_map, recoveryGrp, minCost, minunordered_map, minGroup, matrix, begin, timeOutSeconds);
		//not use it for recovery
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (!e->is_lost){
				unordered_map<Elem*, int>::iterator it = elemunordered_map.find(e);
				it->second--;
				if (it->second == 0){
					elemunordered_map.erase(it);
				}
				//elemunordered_map[e]--;
				//if (elemunordered_map[e] == 0){
				//      elemunordered_map.erase(e);
				//}
			}
		}
		recoveryGrp.pop_back();
	}
}
//��������-�����ָ�
CS_RecoveryResult enumerationRecovery(vector<Point>& points, Elem matrix[][MAX_COL], long timeOutSeconds){
	unordered_map<Elem*, int> elemunordered_map;
	vector<Group*> recoveryGrp;
	CS_RecoveryResult result;

	struct timeval s;
	gettimeofday(&s, NULL);
	_enumerationRecoverySearching(points, 0, elemunordered_map, recoveryGrp, result.cost, result.elemunordered_map, result.recoveryGrp, matrix, s, timeOutSeconds);
	recoveryAlgorithmResult(result.elemunordered_map, result.recoveryGrp);
	return result;
}
/*
���ܣ������ָ�Ѱ�����Ž����ʹ�õ��ǵݹ����
������Ĵ����Ż�������(Ҳ������1������)����һ���Ż�
ʹ��unordered_map                 120 min
unordered_map�޸�Ϊ����ṹ        5 min
ʹ���м�mincost��֦֮��            1 min
gcc -O3�Ż�                        10s
�ر�gcc -g                         10s
ʹ�ó�ʼminCost��֦        10s ���岻�󣬺��ԣ�����˵��������
����һ�ּ�֦���������õ�ǰ��cost+ʣ�������cost-�ص������ֵ > mincost ���岻��
*/
int _enumerationRecoverySearchingFast(vector<Point>& points, int i, int matrixCost[][MAX_COL], vector<Group*>& recoveryGrp,
	int& minCost, int curCost, vector<Group*>& minGroup, Elem matrix[][MAX_COL]){
	//construct an array of identiy which parity is used something like "ppppqqqrrr"

	if (i == points.size()){
		if (minCost > curCost){
			minCost = curCost;
			minGroup = recoveryGrp;
		}
		return minCost;
	}
	//process points[i];
	Elem *e = &matrix[points[i].i][points[i].j];
	for (int j = 0; j < e->ownerGrp.size(); ++j){
		//use it for recovery
		Group* gp = e->ownerGrp[j];
		vector<Elem*>& ve = gp->ve;

		int tc;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (!e->is_lost){
				matrixCost[e->i][e->j]++;
				if (matrixCost[e->i][e->j] == 1)
					curCost++;
			}
		}
		recoveryGrp.push_back(gp);

		//next iteration recursive
		if (curCost < minCost)
		{
			//�Խ�����м�֦
			_enumerationRecoverySearchingFast(points, i + 1, matrixCost, recoveryGrp, minCost, curCost, minGroup, matrix);
		}
		//not use it for recovery
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (!e->is_lost){
				matrixCost[e->i][e->j]--;
				if (matrixCost[e->i][e->j] == 0)
					curCost--;
			}
		}
		recoveryGrp.pop_back();
	}
}
//��������-�����ָ�
CS_RecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]){
	unordered_map<Elem*, int> elemunordered_map;
	vector<Group*> recoveryGrp;
	int curCost = 0;
	//��ʼ����֦���������ٿ��Դﵽ0.3�Ľ�ʡ��ûʲô����������ʹ��
	//minCost = (1 - 0.3) * (row - short_row) * (row - short_col);
	int matrixCost[MAX_ROW][MAX_COL];
	memset(matrixCost, 0, sizeof(matrixCost));

	CS_RecoveryResult result;
	

	_enumerationRecoverySearchingFast(points, 0, matrixCost, recoveryGrp, result.cost, curCost, result.recoveryGrp, matrix);
	
	vector<Group*> &minGroup = result.recoveryGrp;
	for (int i = 0; i < minGroup.size(); ++i)
	{
		Group* gp = minGroup[i];
		for (int j = 0; j < gp->ve.size(); ++j)
		{
			Elem* e = gp->ve[j];
			if (!e->is_lost)
				result.elemunordered_map[e]++;
		}
	}
	recoveryAlgorithmResult(result.elemunordered_map, result.recoveryGrp);
	return result;
}
//̰���㷨��ÿ��ѡȡ���ٿ�����һ����Ԫ��ȫ��ѡ��֮���ټ���һ���֣�ֱ�����ټ��ٿ���Ϊֹ
int _greedyComputeCostForOneCycle(vector<Point> points, unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp, Elem matrix[][MAX_COL])
{
	class MinCostObj{
	public:
		int minCost;
		int whichPoint;
		int whichGrp;
	public:
		MinCostObj(int m, int w1, int w2) : minCost(m), whichPoint(w1), whichGrp(w2){}
	};
	MinCostObj obj(INT_MAX, -1, -1);

	//ѡ�����ź�ɾ���õ㣬����������һ����,ֱ��ȫ���������
	char pointsLabel[1000];
	memset(pointsLabel, 0, sizeof(pointsLabel));
	for (int m = points.size() - 1; m >= 0; --m)
	{
		//�����еĵ���б�����Ѱ��һ�����ٴ��۵ĵ�
		obj.minCost = INT_MAX;
		for (int k = 0; k < points.size(); ++k)
		{
			if (pointsLabel[k])//����������ĵ�
				continue;
			Point point = points[k];
			//����ָ���ǰpoint��Ӧ��Elem����С����
			Elem *e = &matrix[point.i][point.j];
			vector<int> grpCost(e->ownerGrp.size());

			for (int j = 0; j < e->ownerGrp.size(); ++j){
				grpCost[j] = 0;
				//use it for recovery
				Group* gp = e->ownerGrp[j];
				vector<Elem*>& ve = gp->ve;

				int lostNum = 0;
				for (int i = 0; i < ve.size(); ++i)
				{
					Elem * e = ve[i];
					if (e->is_lost){
						lostNum++;
					}
					else{
						//�����Ԫ�ز���unordered_map�У���ô���������1
						unordered_map<Elem*, int>::iterator it = elemunordered_map.find(e);
						if (it == elemunordered_map.end()){
							grpCost[j]++;
						}
					}
				}
				if (lostNum != 1)
				{
					cout << "Error occur" << endl;
				}

				//�˷��������𲽵������˴���ʼ���е���
				if (grpCost[j] == 0){//������У�����ϴ��Ѿ���ѡ�����¼��������
					for (int i = 0; i < ve.size(); ++i)
					{
						Elem * e = ve[i];
						if (elemunordered_map.find(e) != elemunordered_map.end() && elemunordered_map[e] == 1){//==1,�����ϴ���ѡ������У����
							grpCost[j]++;
						}
					}
					if (recoveryGrp[k] != NULL && recoveryGrp[k] != gp){
						printf("Error occur, last cycle gp of %s is selected, but this cycle check error with gp of %s\n", (recoveryGrp[k])->name.c_str(), gp->name.c_str());
					}
				}
			}

			for (int i = 0; i < grpCost.size(); ++i)
			{
				if (grpCost[i] < obj.minCost){
					obj.minCost = grpCost[i];
					obj.whichPoint = k;
					obj.whichGrp = i;
				}
			}
			//�ٴ�ȷ���Ƿ���ȷ
		}
		//�ж���ε�ѡ����ϴε�ѡ���Ƿ�һ��
		Point point = points[obj.whichPoint];
		Elem *e = &matrix[point.i][point.j];
		Group* gp = e->ownerGrp[obj.whichGrp];
		Group* lastGp = recoveryGrp[obj.whichPoint];
		if (lastGp == gp)
		{
			//printf("last group is the same as this gp %s for point %d\n", gp->name.c_str(), obj.whichPoint);
			pointsLabel[obj.whichPoint] = 1;
			continue;
		}

		//ȡ���ϴε�ѡ��
		if (lastGp != NULL){
			vector<Elem*>& ve = lastGp->ve;
			for (int i = 0; i < ve.size(); ++i)
			{
				Elem * e = ve[i];
				if (e->is_lost){
				}
				else{
					//ȡ���ϴε�ѡ�񣬽��ϴε�����Ԫ�ؼ�1 
					assert(elemunordered_map[e] >= 1);
					elemunordered_map[e]--;
					if (elemunordered_map[e] == 0){
						elemunordered_map.erase(e);
					}
				}
			}
		}

		//ȷ��ѡ��˵�
		recoveryGrp[obj.whichPoint] = gp; //�����ָ���whichPoint�����ϵ�Ԫ��ʹ�ô�gp
		pointsLabel[obj.whichPoint] = 1;
		vector<Elem*>& ve = gp->ve;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (e->is_lost){
			}
			else{
				//�����Ԫ�ز���unordered_map�У���ô���������1
				elemunordered_map[e]++;
			}
		}
	}

	return elemunordered_map.size();
}
//ÿ�ζ��Ǵ����еĹ��ϵ�Ԫ������У������ѡ��һ�����ϵ�Ԫ��Ȼ����ѡ����һ�����ϵ�Ԫ
CS_RecoveryResult greedyRecoverySlow(vector<Point>& points, Elem matrix[][MAX_COL]){
	CS_RecoveryResult result;
	unordered_map<Elem*, int> &elemunordered_map = result.elemunordered_map;
	int minCost = result.cost;

	result.recoveryGrp.assign(points.size(), 0);
	vector<Group*> recoveryGrp = result.recoveryGrp;
	int cycle = 0;
	while (1){
		cycle++;
		int cost = _greedyComputeCostForOneCycle(points, elemunordered_map, recoveryGrp, matrix);
#if ENABLE_DEBUG
		if (loglevel >= 2)
			printf("greedyRecoverySlow: cycle: %d, cost:%d\n", cycle, cost);
#endif

		if (cost == minCost){
			//�õ�̰���µ�����
			recoveryAlgorithmResult(elemunordered_map, recoveryGrp);
			return result;
		}
		minCost = cost;
	}
}

//ÿ��ֻ���һ�����ϵ�Ԫ����ѡ������
int _greedyComputeCostDescentFast(vector<Point> points, unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp ,Elem matrix[][MAX_COL]){
	//�����еĵ���б�����Ѱ��һ�����ٴ��۵ĵ�
	for (int k = 0; k < points.size(); ++k)
	{
		Point point = points[k];
		//����ָ���ǰpoint��Ӧ��Elem����С����
		Elem *e = &matrix[point.i][point.j];
		vector<int> grpCost(e->ownerGrp.size());

		for (int j = 0; j < e->ownerGrp.size(); ++j){
			grpCost[j] = 0;
			//use it for recovery
			Group* gp = e->ownerGrp[j];

			vector<Elem*>& ve = gp->ve;

			int lostNum = 0;
			for (int i = 0; i < ve.size(); ++i)
			{
				Elem * e = ve[i];
				if (e->is_lost){
					lostNum++;
				}
				else{
					//�����Ԫ�ز���unordered_map�У���ô���������1
					unordered_map<Elem*, int>::iterator it = elemunordered_map.find(e);
					if (it == elemunordered_map.end()){
						grpCost[j]++;
					}
				}
			}
			if (lostNum != 1)
			{
				cout << "Error occur" << endl;
			}

			//�˷��������𲽵������˴���ʼ���е���
			if (grpCost[j] == 0){//������У�����Ѿ���ѡ�����¼��������
				for (int i = 0; i < ve.size(); ++i)
				{
					Elem * e = ve[i];
					if (elemunordered_map.find(e) != elemunordered_map.end() && elemunordered_map[e] == 1){//==1,�����ϴ���ѡ������У����
						grpCost[j]++;
					}
				}
				if (recoveryGrp[k] != NULL && recoveryGrp[k] != gp){
					printf("Error occur, last cycle gp of %s is selected, but this cycle check error with gp of %s\n", (recoveryGrp[j])->name.c_str(), gp->name.c_str());
				}
			}
		}

		int  whichGrp, minCost = INT_MAX;
		for (int i = 0; i < grpCost.size(); ++i)
		{
			if (grpCost[i] < minCost){
				minCost = grpCost[i];
				whichGrp = i;
			}
		}

		//�ж���ε�ѡ����ϴε�ѡ���Ƿ�һ��
		Group* gp = e->ownerGrp[whichGrp];
		Group* lastGp = recoveryGrp[k];
		if (lastGp == gp)
		{
			//printf("last group is the same as this gp %s for point %d\n", gp->name.c_str(), k);
			continue;
		}

		//ȡ���ϴε�ѡ��
		if (lastGp != NULL){
			vector<Elem*>& ve = lastGp->ve;
			for (int i = 0; i < ve.size(); ++i)
			{
				Elem * e = ve[i];
				if (e->is_lost){
				}
				else{
					//ȡ���ϴε�ѡ�񣬽��ϴε�����Ԫ�ؼ�1 
					assert(elemunordered_map[e] >= 1);
					elemunordered_map[e]--;
					if (elemunordered_map[e] == 0){
						elemunordered_map.erase(e);
					}
				}
			}
		}
		//ȷ��ѡ��˵�
		recoveryGrp[k] = gp; //�����ָ���m�����ϵ�Ԫ��ʹ�ô�gp
		vector<Elem*>& ve = gp->ve;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (e->is_lost){
			}
			else{
				//�����Ԫ�ز���unordered_map�У���ô���������1
				elemunordered_map[e]++;
			}
		}
	}
	return elemunordered_map.size();
}
CS_RecoveryResult greedyRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]){
	CS_RecoveryResult result;
	unordered_map<Elem*, int> &elemunordered_map = result.elemunordered_map;
	int &minCost = result.cost;

	result.recoveryGrp.assign(points.size(), 0);
	vector<Group*> &recoveryGrp = result.recoveryGrp;
	int cycle = 0;
	while (1){
		cycle++;
		int cost = _greedyComputeCostDescentFast(points, elemunordered_map, recoveryGrp,matrix);
#if ENABLE_DEBUG
		if (loglevel >= 2)
			printf("greedyRecoveryFast: cycle: %d, cost:%d\n", cycle, cost);
#endif
		if (cost == minCost){
			recoveryAlgorithmResult(elemunordered_map, recoveryGrp);
			return result;
		}
		minCost = cost;
	}
}

//�û�����Ļָ����ԣ���Ҫ�ṩ������ppppqqqqrrrr�������ַ��� recoveryStr
CS_RecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]){
	extern char* g_RecoveryStr;
	char* recoveryStr = g_RecoveryStr;

	CS_RecoveryResult result;

	int &minCost = result.cost, curCost = 0;
	//��ʼ����֦���������ٿ��Դﵽ0.3�Ľ�ʡ��ûʲô����������ʹ��
	//minCost = (1 - 0.3) * (row - short_row) * (row - short_col);
	int matrixCost[MAX_ROW][MAX_COL];
	memset(matrixCost, 0, sizeof(matrixCost));

	unordered_map<Elem*, int> &minunordered_map = result.elemunordered_map;
	vector<Group*> &minGroup = result.recoveryGrp;

	if (points.size() != strlen(recoveryStr)){
		printf("Error: the number of lost points %d is not the same of user defined recovery group (%d:%s)\n", points.size(), recoveryStr, strlen(recoveryStr));
		exit(-1);
	}
	for (int i = 0; i < points.size(); ++i)
	{
		char c = recoveryStr[i];
		int j = 0;
		if (c == 'p')  j = 0;
		else if (c == 'q') j = 1;
		else if (c == 'r') j = 2;
		else{
			printf("Error: unrecognized user defined recovery char (%c), it should be one of p, q, r\n", c);
			exit(-1);
		}
		//use it for recovery
		Point point = points[i];
		Elem *e = &matrix[point.i][point.j];
		Group* gp = e->ownerGrp[j];
		minGroup.push_back(gp);
	}

	for (int i = 0; i < minGroup.size(); ++i)
	{
		Group* gp = minGroup[i];
		for (int j = 0; j < gp->ve.size(); ++j)
		{
			Elem* e = gp->ve[j];
			if (!e->is_lost)
				minunordered_map[e]++;
		}
	}
	recoveryAlgorithmResult(minunordered_map, minGroup);
	return result;
}


/*************************************
RecoveryMgr��ķ���
**********************************************/
void CS_RecoveryAlgMgr::installDefault()
{
		install(CSRecoveryAlgorithmTypeSearch, new CS_SearchRecovery());
		install(CSRecoveryAlgorithmTypeEnumeration, new CS_SearchRecoveryEnumeration());

		install(CSRecoveryAlgorithmTypeGreedy, new CS_SearchRecoveryGreedy());
		install(CSRecoveryAlgorithmTypeGreedySlow, new CS_SearchRecoveryGreedySlow());
		install(CSRecoveryAlgorithmTypeGreedyFast, new CS_SearchRecoveryGreedyFast());
		install(CSRecoveryAlgorithmTypeGreedyRandom, new CS_SearchRecoveryGreedyRandom());

		install(CSRecoveryAlgorithmTypeUserDefined, new CS_UniqueRecovery());
}

//TODO
const char* getAlgTypeName(CSRecoveryAlgorithmType type){
	switch (type){
	case CSRecoveryAlgorithmTypeSearch: return "CSRecoveryAlgorithmTypeSearch";
	case CSRecoveryAlgorithmTypeEnumeration: return "CSRecoveryAlgorithmTypeEnumeration";
	case CSRecoveryAlgorithmTypeGreedy: return "CSRecoveryAlgorithmTypeGreedy";
	case CSRecoveryAlgorithmTypeGreedyFast: return "CSRecoveryAlgorithmTypeGreedyFast";
	case CSRecoveryAlgorithmTypeGreedyRandom: return  "CSRecoveryAlgorithmTypeGreedyRandom";
	case CSRecoveryAlgorithmTypeGreedySlow: return "CSRecoveryAlgorithmTypeGreedySlow";
	case CSRecoveryAlgorithmTypeUserDefined: return "CSRecoveryAlgorithmTypeUserDefined";
	default:
		return "Unknown ALG type";
	}
}