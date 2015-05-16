//WastedosaureMgr- Guillaume Pastor
#include "WastedosaureManager.h"

WastedosaureManager::WastedosaureManager()
{
	m_groups.resize(100000);
	/*for (int i = 0; i < 1000; ++i)
	{
		m_groups[i].resize(10);
	}*/
}


WastedosaureManager::~WastedosaureManager()
{
}

void WastedosaureManager::AddWastedosaure(Wastedosaure * entity)
{
	m_wastosaures.push_back(entity);
}


void WastedosaureManager::AssignToAGroup(Wastedosaure * entity)
{
	int i = 0;
	while (m_groups[i].size() >= m_maxGroupSize)
	{
		++i;
	}

	m_currentGroupIndexWhereSpaceIsAvaliable = i;

	if (m_groups[m_currentGroupIndexWhereSpaceIsAvaliable].size() == 0 /*|| m_groups[m_currentGroupIndexWhereSpaceIsAvaliable][0] == NULL*/)//On ajoute le premier WS dans le groupe, ce sera le leader
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

void WastedosaureManager::FindPartner(Wastedosaure * entity)
{
	for (int i = 0; i < m_wastosaures.size(); ++i)
	{
		if (m_wastosaures[i]->GetID() != entity->GetID() && m_wastosaures[i]->GetState() != STATE_Dead && m_wastosaures[i]->partner == NULL && entity->partner == NULL)
		{
			m_wastosaures[i]->partner = entity;
			entity->partner = m_wastosaures[i];
			return;
		}
	}
}

void WastedosaureManager::FindReproductionPlace(Wastedosaure * entity1, Wastedosaure * entity2)
{
	NYVert2Df arrival = NYVert2Df((int)entity1->position.X / NYCube::CUBE_SIZE + rand_a_b(-5, 5), (int)entity1->position.Y / NYCube::CUBE_SIZE + rand_a_b(-5, 5));
	entity1->arrivalPartner = arrival;
	entity2->arrivalPartner = arrival;
}
