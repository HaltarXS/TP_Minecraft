#include "ProximityChecker.h"
#include "Parasite.h"

//Init singleton instance
ProximityChecker* ProximityChecker::s_pcInstance = NULL;

ProximityChecker::ProximityChecker()
{

}

ProximityChecker::~ProximityChecker()
{

}

float getSquarredDistance(NYVert3Df* p1, NYVert3Df* p2)
{
	return (p1->X - p2->X)*(p1->X - p2->X) + (p1->Y - p2->Y)*(p1->Y - p2->Y) + (p1->Z - p2->Z)*(p1->Z - p2->Z);
}

ProximityChecker* ProximityChecker::GetSingleton()
{
	if (!s_pcInstance)
	{
		s_pcInstance = new ProximityChecker();
	}

	return s_pcInstance;
}

vector<IABase*> ProximityChecker::checkParasiteProximityCreatures(Parasite* callerEntity, float sizeArea)
{
	callerEntity->m_creaturesNear.clear(); //Nettoyage de la liste des créatures à proximité

	//Parcourt de la liste des créatures existantes
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		if (type != eTypeCreature::DAHUT) { //Toutes sauf les parasites qui n'ont pas besoin de se parasiter entre eux...
			for (int j = 0; j < (*m_entities)[type].size(); ++j)
			{
				//callerEntity->position;
				//(*m_entities)[type][j]->position;

				float distanceSquarred = getSquarredDistance(callerEntity->position, (*m_entities)[type][j]->position);
				if (getSquarredDistance(callerEntity->position, (*m_entities)[type][j]->position) < (sizeArea*sizeArea) && callerEntity->target->GetID() != (*m_entities)[type][j]->GetID() && !(*m_entities)[type][j]->infected) {
					callerEntity->m_creaturesNear.push_back((*m_entities)[type][j]);
				}
				//IF Distance between caller and (*m_entities)[type][j] < sizeArea && (*m_entities)[type][j] != caller.target
				//}
			}
		}
	}
	return callerEntity->m_creaturesNear;
}