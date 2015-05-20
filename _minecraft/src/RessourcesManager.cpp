/**
* File : RessourcesManager.cpp
* Author : Paul Gerst
* Description : Source file of the Resource Manager class
* This class is a very basic container, and could be improved
* to order Resources considering spatial repartition, etc.
*/

#include "RessourcesManager.h"
#include "engine\render\renderer.h"
#include "Herbe.h"
#include "Crotte.h"

//Init singleton instance
RessourcesManager* RessourcesManager::s_pInstance = NULL;

RessourcesManager::RessourcesManager()
{
	//Init map with all resources types
	for(int i = 0; i < RESSOURCE_NUM; ++i)
	{
		TypeRessource type = (TypeRessource) i;
		m_ressources.insert(std::pair<TypeRessource, RessourceList>(type, RessourceList()));
	}
}

RessourcesManager* RessourcesManager::GetSingleton()
{
	//Allocate instance if not already created
	if(!s_pInstance)
	{
		s_pInstance = new RessourcesManager();
	}

	return s_pInstance;
}

void RessourcesManager::Destroy()
{
	//Delete instance
	SAFEDELETE(s_pInstance);
}

Ressource* RessourcesManager::Create(TypeRessource type, NYVert3Df position, int maxQuantity)
{
	//Instantiate resource considering its type
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

	//Store it in the container if created
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

	//Return a pointer to the resource list
	return &m_ressources[type];
}

void RessourcesManager::Delete(Ressource *pRessource)
{
	//Delete a resource
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
	//Update resources
	float delta = NYRenderer::_DeltaTime;
	for(int i = 0; i < RESSOURCE_NUM; ++i)
	{
		TypeRessource type = (TypeRessource) i;
		auto it = m_ressources[type].begin();
		auto end = m_ressources[type].end();
		while(it != end)
		{
			(*it)->Update(delta);

			//If there is no resource left
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
	//Render all resources
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