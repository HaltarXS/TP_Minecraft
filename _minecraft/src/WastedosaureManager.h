//WastedosaureMgr- Guillaume Pastor
#pragma once
#include <iostream>
#include <cstdio>
#include <vector>
#include "Wastedosaure.h"

class Wastedosaure;

class WastedosaureManager
{

private:
	WastedosaureManager();
	std::vector<Wastedosaure *> m_wastosaures; //Tous les wastedosaures en jeu
	std::vector<Wastedosaure *> m_group;//Le groupe des wastedosaures encore vivants


	//--Obsoletes--//
	int m_currentGroupIndexWhereSpaceIsAvaliable = 0; //Index un groupe ayant de la place. Obsolete
	int m_maxGroupSize = 4; //Taille maximale d'un meute

public:
	//Recupere l'instance
	static WastedosaureManager * GetSingleton()
	{
		static WastedosaureManager wm;
		return &wm;
	}

	~WastedosaureManager();//Destructeur

	//Permet d'assigner le Wastedosaure au groupe
	void AssignToAGroup(Wastedosaure * entity);

	//Ajoute le wastedosaure dans la liste de wastedosaure
	void AddWastedosaure(Wastedosaure * entity);

	//Permet à un wastedosaure de trouver un partenaire libre
	void FindPartner(Wastedosaure * entity);

	//Permet de trouver un lieu de reproduction au calme
	void FindReproductionPlace(Wastedosaure * entity1, Wastedosaure * entity2);

	//Renvoie vrai si tout le monde est arrivé. Obsolete
	bool IsEveryoneArrived();

	//Initialise des paramètres pour l'attaque en groupe des wastedosaures
	void PrepareAttack(IABase * target);



	//----  UTILS ----
	//Renvoie un int entre a et b
	inline int rand_a_b(int a, int b){
		return rand() % (b - a) + a;
	}

};

