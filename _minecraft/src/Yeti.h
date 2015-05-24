#pragma once

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"
#include <iostream>

#include "IABase.h"
#include "world.h"
#include "Pathfinding.h"
#include "Viewcone.h"
#include "RessourcesManager.h"

typedef std::vector<IABase*> CreatureVector;

class Yeti :public IABase
{
private:

	float m_speed = 50.0f + 0.05f*GetID();

	//Timers

	//Wandering/Attacking
	const float m_durationWandering = 4.0f;
	float m_timerWandering = 0.0f;
	const float m_initialWait = 5.0f;
	 float m_initialDuration = 0.0f;


	//Attack 
	const float m_durationAttack = 10.0f;
	float m_timerAttack = 0.0f;

	const float m_timeBetween2Attacks = 1.0f;
	float m_timeElapsedBetween2Attacks = 0.0f;
	const int m_damages = 1;
	
	//Path fo PF
	Pathfinding * m_pf = Pathfinding::GetSingleton();
	Path m_path;
	int m_currentIndex = 0;

	float m_timeTryFindPath = 0.5f;
	float m_timerTryFindPath = 0.0f;

	float m_currentSize = 25.0f;

	//View Cone
	const float m_viewDistance = 30.0f;
	const float m_viewAngle = 80.0f;

	//Distance to target
	float m_distanceToTarget = 0.0f;
	bool m_pathAttackFound = true;
	
	NYTimer m_lastUpdate;
	
	bool HasAPath();
	int FindClosestWaypoint(Path _path);

	Viewcone m_cone;
	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu
	std::vector<IABase*> m_creaturesInSight;//Les créatures dans le champ visuel

	void GetCreaturesInSight();

	bool m_debugDraw = true;

	NYVert3Df FindClosestCubeSnow();
	vector<NYVert2Df> FindClosestCubesSnow(NYVert2Df pos);

	void UpdateTimers();

	void PrepareAttack(IABase * target);

	inline int rand_a_b(int a, int b){
		int result;


		if (b < 1)
			result = rand() % (1) + 1;
		
		else if (a > 0)
			result = rand() % (b - a) + a;
		else 
			result = rand() % (b)+ 1;

		return result;

			
	}

	//Check senses to avoid predators and smell poop
	bool Senses();
	float m_smellDistance = 20.0f;
	bool fuite = false;
public:
	Yeti(NYWorld * _world, NYVert2Df _positionInitiale);
	~Yeti();

	IABase * target= NULL; //Cible du Yeti

	bool isArrived = true;

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);

	Path GetPath();

	virtual void UpdateIA();
	virtual void Draw();

	//The method from the StateMachine class to override
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);

};