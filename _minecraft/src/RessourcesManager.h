/**
* File : RessourcesManager.h
* Author : Paul Gerst
* Description : Header file of the Resource Manager class
* This class is a very basic container, and could be improved
* to order Resources considering spatial repartition, etc.
*/

#ifndef RESSOURCESMANAGER_H
#define RESSOURCESMANAGER_H

#include <list>

#include "Ressource.h"

//Typedef defining a ressources array
typedef std::list<Ressource*> RessourceList;

class RessourcesManager
{
private :

	//Singleton instance
	static RessourcesManager* s_pInstance;

	//Ressources container
	std::map<TypeRessource, RessourceList> m_ressources;

public :

	//Singleton getter / destructor
	static RessourcesManager* GetSingleton();
	static void Destroy();

	//Create, get or delete ressources
	Ressource* Create(TypeRessource type, NYVert3Df position, int maxQuantity);
	RessourceList* GetRessourcesByType(TypeRessource type);
	void Delete(Ressource *pRessource);

	//Update, render ressources
	void Update();
	void Render();

private :

	RessourcesManager();
	~RessourcesManager(){}
};

#endif