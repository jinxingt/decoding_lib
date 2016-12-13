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
功能：打印最优结果
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

		//打印恢复过程中所使用的校验列
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

		//打印复用次数为1，2，3的数据块个数
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

		//打印每一个数据块的复用矩阵，矩阵元素的数字为复用的次数
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

		//打印每一列所使用的元素个数，负载均衡可能会有用
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
功能：遍历恢复寻找最优结果，使用的是递归程序

没优化之前：
rdor.exe 8 3 0 0 0 0   运行时间3.213s
第一次优化，将所有的map修改为unordered_map，运行时间变为 2.556 s
第二次优化，将函数中的参数减少两个(minMap, minGroup)，不再使用它，运行时间变为2.17s(未采用)
第三次优化，删除lost_num的排错型判别，运行时间为2.185
第四次优化，将从map元素中取出elem的操作过程修改一下，利用了iterator，运行时间变为1.50s 降低一半
*/
int _enumerationRecoverySearching(vector<Point>& points, int i, unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp, int& minCost,
	unordered_map<Elem*, int>& minunordered_map, vector<Group*>& minGroup, Elem matrix[][MAX_COL], struct timeval begin, long timeOutSeconds){
	//construct an array of identiy which parity is used something like "ppppqqqrrr"

	//这会调用的极其频繁
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
//暴力搜索-遍历恢复
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
功能：遍历恢复寻找最优结果，使用的是递归程序
在上面的代码优化基础上(也就提升1倍而已)，进一步优化
使用unordered_map                 120 min
unordered_map修改为数组结构        5 min
使用中间mincost剪枝之后            1 min
gcc -O3优化                        10s
关闭gcc -g                         10s
使用初始minCost剪枝        10s 意义不大，忽略，而且说服力不行
还有一种剪枝方案：利用当前的cost+剩余的最优cost-重叠的最大值 > mincost 意义不大
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
			//对结果进行剪枝
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
//暴力搜索-遍历恢复
CS_RecoveryResult enumerationRecoveryFast(vector<Point>& points, Elem matrix[][MAX_COL]){
	unordered_map<Elem*, int> elemunordered_map;
	vector<Group*> recoveryGrp;
	int curCost = 0;
	//初始化剪枝，遍历最少可以达到0.3的节省，没什么提升，不再使用
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
//贪心算法，每次选取最少开销的一个单元，全部选完之后，再继续一下轮，直到不再减少开销为止
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

	//选择最优后，删除该点，继续处理下一个点,直到全部处理完毕
	char pointsLabel[1000];
	memset(pointsLabel, 0, sizeof(pointsLabel));
	for (int m = points.size() - 1; m >= 0; --m)
	{
		//对所有的点进行遍历，寻找一个最少代价的点
		obj.minCost = INT_MAX;
		for (int k = 0; k < points.size(); ++k)
		{
			if (pointsLabel[k])//跳过处理过的点
				continue;
			Point point = points[k];
			//计算恢复当前point对应的Elem的最小代价
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
						//如果此元素不在unordered_map中，那么对其代价增1
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

				//此方法后面逐步调整，此处开始进行调整
				if (grpCost[j] == 0){//表明此校验组上次已经被选择，重新计算其代价
					for (int i = 0; i < ve.size(); ++i)
					{
						Elem * e = ve[i];
						if (elemunordered_map.find(e) != elemunordered_map.end() && elemunordered_map[e] == 1){//==1,表明上次是选择的这个校验组
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
			//再次确定是否正确
		}
		//判断这次的选择和上次的选择是否一致
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

		//取消上次的选择
		if (lastGp != NULL){
			vector<Elem*>& ve = lastGp->ve;
			for (int i = 0; i < ve.size(); ++i)
			{
				Elem * e = ve[i];
				if (e->is_lost){
				}
				else{
					//取消上次的选择，将上次的所有元素减1 
					assert(elemunordered_map[e] >= 1);
					elemunordered_map[e]--;
					if (elemunordered_map[e] == 0){
						elemunordered_map.erase(e);
					}
				}
			}
		}

		//确定选择此点
		recoveryGrp[obj.whichPoint] = gp; //表明恢复第whichPoint个故障单元，使用此gp
		pointsLabel[obj.whichPoint] = 1;
		vector<Elem*>& ve = gp->ve;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (e->is_lost){
			}
			else{
				//如果此元素不在unordered_map中，那么对其代价增1
				elemunordered_map[e]++;
			}
		}
	}

	return elemunordered_map.size();
}
//每次都是从所有的故障单元的所有校验组中选择一个故障单元，然后再选择下一个故障单元
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
			//得到贪心下的最优
			recoveryAlgorithmResult(elemunordered_map, recoveryGrp);
			return result;
		}
		minCost = cost;
	}
}

//每次只针对一个故障单元进行选择最优
int _greedyComputeCostDescentFast(vector<Point> points, unordered_map<Elem*, int>& elemunordered_map, vector<Group*>& recoveryGrp ,Elem matrix[][MAX_COL]){
	//对所有的点进行遍历，寻找一个最少代价的点
	for (int k = 0; k < points.size(); ++k)
	{
		Point point = points[k];
		//计算恢复当前point对应的Elem的最小代价
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
					//如果此元素不在unordered_map中，那么对其代价增1
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

			//此方法后面逐步调整，此处开始进行调整
			if (grpCost[j] == 0){//表明此校验组已经被选择，重新计算其代价
				for (int i = 0; i < ve.size(); ++i)
				{
					Elem * e = ve[i];
					if (elemunordered_map.find(e) != elemunordered_map.end() && elemunordered_map[e] == 1){//==1,表明上次是选择的这个校验组
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

		//判断这次的选择和上次的选择是否一致
		Group* gp = e->ownerGrp[whichGrp];
		Group* lastGp = recoveryGrp[k];
		if (lastGp == gp)
		{
			//printf("last group is the same as this gp %s for point %d\n", gp->name.c_str(), k);
			continue;
		}

		//取消上次的选择
		if (lastGp != NULL){
			vector<Elem*>& ve = lastGp->ve;
			for (int i = 0; i < ve.size(); ++i)
			{
				Elem * e = ve[i];
				if (e->is_lost){
				}
				else{
					//取消上次的选择，将上次的所有元素减1 
					assert(elemunordered_map[e] >= 1);
					elemunordered_map[e]--;
					if (elemunordered_map[e] == 0){
						elemunordered_map.erase(e);
					}
				}
			}
		}
		//确定选择此点
		recoveryGrp[k] = gp; //表明恢复第m个故障单元，使用此gp
		vector<Elem*>& ve = gp->ve;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (e->is_lost){
			}
			else{
				//如果此元素不在unordered_map中，那么对其代价增1
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

//用户定义的恢复策略，需要提供类似于ppppqqqqrrrr这样的字符串 recoveryStr
CS_RecoveryResult userdefinedRecovery(vector<Point>& points, Elem matrix[][MAX_COL]){
	extern char* g_RecoveryStr;
	char* recoveryStr = g_RecoveryStr;

	CS_RecoveryResult result;

	int &minCost = result.cost, curCost = 0;
	//初始化剪枝，遍历最少可以达到0.3的节省，没什么提升，不再使用
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
RecoveryMgr类的方法
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