//WastedosaureMgr- Guillaume Pastor
#include "WastedosaureManager.h"

WastedosaureManager::WastedosaureManager()
{
	//m_groups.resize(100000);
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

bool WastedosaureManager::IsEveryoneArrived()
{
	for (int i = 0; i < m_wastosaures.size(); ++i)
	{
		if (m_wastosaures[i]->GetState() != STATE_Dead)
		{
			if (!m_wastosaures[i]->isArrived)
				return false;
		}
	}

	return true;
}


void WastedosaureManager::AssignToAGroup(Wastedosaure * entity)
{
	//if (m_group.size() > 1)
	//{
	//	if (m_group[0] == NULL || m_group[0]->GetState() == STATE_Dead)//Si le leader n'est plus là
	//	{
	//		m_group[0] = entity;
	//		for (int i = 1; i < m_group.size(); ++i)
	//		{
	//			m_group[i]->leader = entity;
	//		}
	//	}
	//	else
	//	{
	//		m_group.push_back(entity);
	//		entity->leader = m_group[0];
	//	}
	//}
	//else
	//{
	//	m_group.push_back(entity);
	//}
	entity->groupPosition = m_group.size();
	m_group.push_back(entity);
	if (m_group[0]->GetState() == STATE_Dead)
		m_group[0] = m_group.back();


	entity->leader = m_group[0];
	m_group[0]->leader = NULL;

}

void WastedosaureManager::FindPartner(Wastedosaure * entity)
{
	//cout << entity->GetID() << " Test\n";
	for (int i = 0; i < m_wastosaures.size(); ++i)
	{
		if (m_wastosaures[i]->partner == NULL && 
			entity->partner == NULL && 
			m_wastosaures[i]->GetID() != entity->GetID() &&
			m_wastosaures[i]->GetState() != STATE_Dead &&
			m_wastosaures[i]->GetState() != STATE_Egg)
		{
			entity->partner = m_wastosaures[i];
			entity->partner->partner = entity;
			//m_wastosaures[i]->partner = entity;

			entity->hasPartner = true;
			entity->partner->hasPartner = true;
			//cout << entity->GetID() << " et " << m_wastosaures[i]->GetID() << endl;
			return;
		}
	}
}

void WastedosaureManager::FindReproductionPlace(Wastedosaure * entity1, Wastedosaure * entity2)
{
	if (entity1 != NULL && entity2 != NULL)
	{
		NYVert2Df arrival = NYVert2Df((int)entity1->position.X / NYCube::CUBE_SIZE + rand_a_b(-4, 4), (int)entity1->position.Y / NYCube::CUBE_SIZE + rand_a_b(-4, 4));

		entity1->arrivalPartner = arrival;
		entity2->arrivalPartner = arrival;
	}
	
}

void WastedosaureManager::PrepareAttack(IABase * target)
{
	for (int i = 0; i < m_wastosaures.size(); ++i)
	{
		m_wastosaures[i]->target = target;
		m_wastosaures[i]->PushState(STATE_Attack);
	}
}
