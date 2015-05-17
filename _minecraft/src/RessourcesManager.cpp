#include "RessourcesManager.h"

//Init singleton instance
RessourcesManager* RessourcesManager::s_pInstance = NULL;

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
	//Create a resource
}

RessourceVector* RessourcesManager::GetRessourcesByType(TypeRessource type)
{
	//Get resource vector
	return NULL;
}

void Delete(Ressource *pRessource)
{
	//Delete a resource
}
