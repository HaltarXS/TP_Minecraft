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
#include "engine/timer.h"

typedef std::vector<IABase*> CreatureVector;

class GlaceGouille : public IABase
{
private :

	//Pathfinding
	Path m_path;
	Pathfinding *m_pathfinding;

	float m_speed = 2.0f *  60;
	float m_eggTime = 3.0f * 60;
	float m_reproductionTime = 5.0f * 60;
	float m_starvationTime = 4.0f * 60;

	NYTimer m_LastUpdateTimer;
	float m_eggTimer = 0;
	float m_reproductionTimer = 0;
	float m_starvationTimer = 0;

	int m_currentPathIndex;

	bool m_pathFound;
	bool m_reproduce = false;
	
	bool m_debugDraw = true;

	objectID m_targetID;

	Viewcone m_cone;

	


	std::vector<IABase*> m_creaturesInSight;//Les créatures dans le champ visuel

	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu

	void GetCreaturesInSight();
	//void UpdateTimer();

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

