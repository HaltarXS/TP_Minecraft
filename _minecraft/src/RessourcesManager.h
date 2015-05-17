#pragma once
#include "Ressource.h"

//Typedef defining a ressources array
typedef std::vector<Ressource> RessourceVector;

class RessourcesManager
{
private :

	//Singleton instance
	static RessourcesManager* s_pInstance;

	//Ressources container
	std::map<TypeRessource, RessourceVector> m_ressources;

public :

	//Singleton getter / destructor
	static RessourcesManager* GetSingleton();
	static void Destroy();

	//Create, get or delete ressources
	void Create(TypeRessource type, NYVert3Df position, int maxQuantity);
	RessourceVector* GetRessourcesByType(TypeRessource type);
	void Delete(Ressource *pRessource);

private :

	RessourcesManager(){}
	~RessourcesManager(){}
};

//res * tmp = createT("nom");
//res * tmp = createT<nom>