/**
* File : GlaceGouille.h
* Author : Judicael Abecassis
* Description : Header file of the GlaceGouille AI class
*/

#ifndef GLACEGOUILLE_H
#define GLACEGOUILLE_H

#include "IABase.h"
#include "Pathfinding.h"
#include "Viewcone.h"

typedef std::vector<IABase*> CreatureVector;

class GlaceGouille : public IABase
{
private :

	//Pathfinding
	Path m_path;
	Pathfinding *m_pathfinding;

	float m_eggTime = 3.0f;
	float m_eggTimer;
	float m_speed = 2.0f;
	
	int m_currentPathIndex;

	bool m_pathFound;
	bool m_reproduce = false;
	
	bool m_debugDraw = true;

	Viewcone m_cone;

	


	std::vector<IABase*> m_creaturesInSight;//Les créatures dans le champ visuel

	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu

	void GetCreaturesInSight();

public:
	GlaceGouille(NYWorld *pWorld, NYVert2Df pos);
	~GlaceGouille();

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);

	//Override base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);

};

#endif

