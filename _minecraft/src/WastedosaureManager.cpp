//WastedosaureMgr- Guillaume Pastor
#include "WastedosaureManager.h"

WastedosaureManager::WastedosaureManager()
{
	m_groups.resize(1000);
}


WastedosaureManager::~WastedosaureManager()
{
}

void WastedosaureManager::AssignToAGroup(Wastedosaure * entity)
{
	int i = 0;
	while (m_groups[i].size() >= m_maxGroupSize)
	{
		++i;
	}

	m_currentGroupIndexWhereSpaceIsAvaliable = i;

	if (m_groups[m_currentGroupIndexWhereSpaceIsAvaliable].size() == 0 || m_groups[m_currentGroupIndexWhereSpaceIsAvaliable][0] == NULL)//On ajoute le premier WS dans le groupe, ce sera le leader
	{
		m_groups[m_currentGroupIndexWhereSpaceIsAvaliable].push_back(entity);
	}
	else //On ajoute les autres
	{
		entity->leader = m_groups[m_currentGroupIndexWhereSpaceIsAvaliable][0];
		entity->groupPosition = m_groups[m_currentGroupIndexWhereSpaceIsAvaliable].size();
		m_groups[m_currentGroupIndexWhereSpaceIsAvaliable].push_back(entity);
	}
}