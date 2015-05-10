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

	std::vector<std::vector<Wastedosaure *>> m_groups;

public:

	static WastedosaureManager * GetSingleton()
	{
		static WastedosaureManager wm;
		return &wm;
	}

	~WastedosaureManager();

	void AssignToAGroup(Wastedosaure * entity);

};

