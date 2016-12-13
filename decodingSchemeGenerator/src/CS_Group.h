#ifndef __CS__GROUP__H__
#define __CS__GROUP__H__

#include "CS_Common.h"

struct Lost{
	short i, j, k, m, lost_num;
	Lost(int _i, int _j, int _k, int _m, int _lost) :i(_i), j(_j), k(_k), m(_m), lost_num(_lost){}
	Lost() : i(-1), j(-1), k(-1), m(-1), lost_num(0){}
	vector<int> columns(){
		vector<int> cols;
		if (i != -1) cols.push_back(i);
		if (j != -1) cols.push_back(j);
		if (k != -1) cols.push_back(k);
		if (m != -1) cols.push_back(m);
	}
};

class Group;
struct Elem{
	short i;
	short j;
	short cost;
	short is_lost;
	short magic;
	vector<Group*> ownerGrp;
	void addOwnerGrp(Group* p){
		ownerGrp.push_back(p);
	}
	void serialize(ostream& os){
		os << i << "," << j;
	}
	void unserialize(istream& is){
		is >> i >> "," >> j;
	}
};
struct Point{
	int i;
	int j;
	Point(int _i, int _j) :i(_i), j(_j){}
	Point(){}
	bool operator==(const Point& rhs) const {return i == rhs.i && j == rhs.j;}

};

class Group
{
public:
	vector<Elem*> ve;
	int lostNum;
	string name;
public:
	Group(){
	}
public:
	void addElem(Elem* e){
		if (e->cost != 0){
			ve.push_back(e);
			e->addOwnerGrp(this);
		}
	}
	void setName(const char* name, int index){
		static char buf[128];
		sprintf(buf, "%s%d", name, index);
		this->name = buf;
	}
	void print(){
		printf("%p:%s: [ ", this, name.c_str());
		int lost = 0;
		for (auto e : ve){
			if (e->is_lost){
				printf("(%d,%d)L ", e->i, e->j);
				lost++;
			}
			else
				printf("(%d,%d)  ", e->i, e->j);
		}
		printf("]");
		if (lost > 0){
			printf(" Lost:%d", lost);
		}
	}
	void serialize(ostream& os){
		os << ve.size() << endl;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem* e = ve[i];
			e->serialize(os);
		}
	}
	void unserialize(istream& is){
		int size;
		is >> size;
		for (int i = 0; i < size;++i)
		{
			Elem* e = new Elem();
			e->unserialize(is);
			ve.push_back(e);
		}
	}

public:
	int calculate(){
		lostNum = 0;
		int index = -1;
		for (int i = 0; i < ve.size(); ++i)
		{
			Elem * e = ve[i];
			if (e->is_lost){
				lostNum++;
				index = i;
			}
		}
		return index;
	}
	//return the cost for recovery. 
	int doRecovery(){
		int index = calculate();
		if (lostNum > 1){
			return -1;//can not recovery
		}
		else if (lostNum == 1){//recover the index
#if ENABLE_DEBUG
			printf("%p:%s recover: (%d,%d) cost:%d\n", this, name.c_str(), ve[index]->i, ve[index]->j, ve.size() - 1);
#endif
			ve[index]->is_lost = 0;
			return ve.size() - 1;
		}
		else{// all good, no need to recovery
			return 0;
		}
	}
};

class GroupManager
{
private:
	Group grp[10000];
	int grp_index;
	bool is_first_recovery;
private:
	list<Group*> backUp;
	list<Group*> vg;
public:
	GroupManager(){
		grp_index = 0;
		is_first_recovery = true;
	}
public:
	Group* newGroup(){
		assert(grp_index < 10000);
		Group* pg = grp + grp_index;
		vg.push_back(pg);
		++grp_index;
		return pg;
	}
public:
	void print(){
		printf("Group count:%d\n", vg.size());
		for (auto pg : vg){
			printf("    ");
			pg->print();
			printf("\n");
		}
	}
public:
	//current use this one.
	int recovery(){
		if (is_first_recovery){
			backUp = vg;
			is_first_recovery = false;
		}
		else{
			vg = backUp;
		}
#if ENABLE_DEBUG
		printf("Recovery Circumstances...\n");
#endif
		int cost = 0;
		int lastSize = -1;
		int totalCost = 0;

		while (vg.size() != 0){
			for (auto it = vg.begin(); it != vg.end();)
			{
				Group* pg = *it;
				cost = pg->doRecovery();
				if (cost == -1){
					++it;
				}
				else{
					totalCost += cost;
					it = vg.erase(it);
				}
			}

			if (lastSize == vg.size()){
				return -1;//can not recovery.
			}
			lastSize = vg.size();
		}

		return totalCost;
	}
};


#endif//__CS__GROUP__H__
