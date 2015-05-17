//Wastedosaure- Guillaume Pastor
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

#include "WastedosaureManager.h"

#include "Viewcone.h"

//using namespace std;

typedef std::vector<IABase*> CreatureVector;
#define MAX_WASTEDOSAURE 25

class Wastedosaure :public IABase
{
private:

	float m_speed = 30.30f + 0.05f*GetID();

	//Timers
	//Egg
	const float m_timeEgg = 1.0f;
	float m_timerEgg = 0.0f;

	//Wandering/Attacking
	const float m_durationWandering = 60.0f /*+ WastedosaureManager::GetSingleton()->rand_a_b(-2,2)*/;
	float m_timerWandering = 0.0f;

	//Reproduction
	const float m_durationReproduction = 10.0f /* + WastedosaureManager::GetSingleton()->rand_a_b(-2, 2)*/;
	float m_timerReproduction = 0.0f;

	//Reproduction
	bool m_canReproduce = true;
	int m_counterReproduction = 0;
	int m_maxReproductions = 2;
	bool m_needPartner = false;

	//Attack 
	const float m_durationAttack = 10.0f;
	float m_timerAttack = 0.0f;

	const float m_timeBetween2Attacks = 3.0f;
	float m_timeElapsedBetween2Attacks = 0.0f;
	const int m_damages = 1;
	

	bool m_isMale = false;

	//Path fo PF
	Pathfinding * m_pf = Pathfinding::GetSingleton();
	Path m_path;
	int m_currentIndex = 0;
	//NYVert2Df arrivalPartner;

	float m_timeTryFindPath = 5.0f;
	float m_timerTryFindPath = 0.0f;

	float m_minSize = 2.0f;
	float m_maxSize = 9.0f;
	float m_currentSize = m_minSize;
	float m_durationGrow = 40.0f;
	float m_timerGrow = 0.0f;

	//View Cone
	const float m_viewDistance = 100.0f;
	const float m_viewAngle = 50.0f;

	//Distance to target
	float m_distanceToTarget = 0.0f;
	bool m_pathAttackFound = true;
	

	bool IsEveryoneArrived();

	NYTimer m_lastUpdate;
	

	bool HasAPath();

	int FindClosestWaypoint(Path _path);

	Viewcone m_cone;

	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu

	std::vector<IABase*> m_creaturesInSight;//Les créatures dans le champ visuel

	void GetCreaturesInSight();

	bool m_debugDraw = false;

	

	NYVert3Df m_cubeWater;//Sert juste dans FindClosestCubeWater. A delete
	NYVert3Df FindClosestCubeWater();

	void UpdateTimers();



	
	

public:
	Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale);
	~Wastedosaure();

	Wastedosaure * leader = NULL;//Leader d'un groupe
	Wastedosaure * partner = NULL; //Partenaire du wastedosaure
	bool hasPartner = false;
	int groupPosition = 0;

	IABase * target= NULL; //Cible du Wastedosaure

	NYVert2Df arrivalPartner;

	bool isArrived = true;

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);

	Path GetPath();

	virtual void UpdateIA();

	virtual void Draw();

	//The method from the StateMachine class to override
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);

};