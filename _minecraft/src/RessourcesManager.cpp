#include "RessourcesManager.h"
#include "Herbe.h"
#include "Crotte.h"

//Init singleton instance
RessourcesManager* RessourcesManager::s_pInstance = NULL;

RessourcesManager::RessourcesManager()
{
	for(int i = 0; i < (int)RESSOURCE_NUM; ++i)
	{
		TypeRessource type = (TypeRessource) i;
		m_ressources.insert(std::pair<TypeRessource, RessourceList>(type, RessourceList()));
	}
}

RessourcesManager* RessourcesManager::GetSingleton()
{
	if(!s_pInstance)
	{
		s_pInstance = new RessourcesManager();
	}

	return s_pInstance;
}

void RessourcesManager::Destroy()
{
	SAFEDELETE(s_pInstance);
}

void RessourcesManager::Create(TypeRessource type, NYVert3Df position, int maxQuantity)
{
	Ressource *pRessource = NULL;
	switch(type)
	{
		case HERBE :
		{
			pRessource = new Herbe(position, maxQuantity);
		}
		break;

		case CROTTE :
		{
			pRessource = new Crotte(position, maxQuantity);
		}
		break;

		default :
		{
			std::cout << "Failed to create ressource : unrecognized type" << std::endl;
		}
		break;
	}

	if(pRessource)
	{
		m_ressources[type].push_back(pRessource);
	}
}

RessourceList* RessourcesManager::GetRessourcesByType(TypeRessource type)
{
	if(type >= RESSOURCE_NUM)
	{
		std::cout << "Failed to get ressources : unrecognized type" << std::endl;
		return NULL;
	}

	return &m_ressources[type];
}

void RessourcesManager::Delete(Ressource *pRessource)
{
	auto begin = m_ressources[pRessource->Type].begin();
	auto end = m_ressources[pRessource->Type].end();
	for(auto it = begin; it != end; ++it)
	{
		delete (*it);
		m_ressources[pRessource->Type].erase(it);
	}
}
