//WastedosaureMgr- Guillaume Pastor
#pragma once
#include <iostream>
#include <cstdio>
#include <vector>

#include "Wastedosaure.h"

class Wastedosaure;

class WastedosaureManager
{

private:
	WastedosaureManager();

	int m_currentGroupIndexWhereSpaceIsAvaliable = 0;

	int m_maxGroupSize = 4;

	std::vector<Wastedosaure *> m_wastosaures;

	//std::vector<std::vector<Wastedosaure *>> m_groups;
	std::vector<Wastedosaure *> m_group;

	

public:

	static WastedosaureManager * GetSingleton()
	{
		static WastedosaureManager wm;
		return &wm;
	}

	~WastedosaureManager();

	void AssignToAGroup(Wastedosaure * entity);

	void AddWastedosaure(Wastedosaure * entity);

	void FindPartner(Wastedosaure * entity);

	void FindReproductionPlace(Wastedosaure * entity1, Wastedosaure * entity2);

	inline int rand_a_b(int a, int b){
		return rand() % (b - a) + a;
	}

	bool IsEveryoneArrived();

};

