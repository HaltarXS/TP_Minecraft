#include "RessourcesManager.h"
#include "engine\render\renderer.h"
#include "Herbe.h"
#include "Crotte.h"

//Init singleton instance
RessourcesManager* RessourcesManager::s_pInstance = NULL;

RessourcesManager::RessourcesManager()
{
	for(int i = 0; i < RESSOURCE_NUM; ++i)
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

Ressource* RessourcesManager::Create(TypeRessource type, NYVert3Df position, int maxQuantity)
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

	return pRessource;
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
		if((*it) == pRessource)
		{
			Ressource *pRessource = (*it);
			m_ressources[pRessource->Type].erase(it);
			delete pRessource;
			break;
		}
	}
}

void RessourcesManager::Update()
{
	float delta = NYRenderer::_DeltaTime;
	for(int i = 0; i < RESSOURCE_NUM; ++i)
	{
		TypeRessource type = (TypeRessource) i;
		auto it = m_ressources[type].begin();
		auto end = m_ressources[type].end();
		while(it != end)
		{
			(*it)->Update(delta);
			if((*it)->Quantity <= 0)
			{
				auto prev = it++;
				delete (*prev);
				m_ressources[type].erase(prev);
			}
			else
			{
				it++;
			}
		}
	}
}

void RessourcesManager::Render()
{
	for(int i = 0; i < RESSOURCE_NUM; ++i)
	{
		TypeRessource type = (TypeRessource) i;
		auto begin = m_ressources[type].begin();
		auto end = m_ressources[type].end();
		for(auto it = begin; it != end; ++it)
		{
			(*it)->Render();
		}
	}
}