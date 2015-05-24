/*
* Header file : Cameleon AI class
* Author : LE MARCHAND Pierre
* Date : 16/05/2015
*/

#ifndef _CAMELEON_H_
#define _CAMELEON_H_



#include "IABase.h"
#include <stdlib.h>   
#include "Viewcone.h"
#include "Path.h"
#include "Pathfinding.h"

#include "engine\timer.h"
class Cameleon :public IABase
{
private:
	//----- attributes -----//
	bool m_debug = false;

	NYTimer m_timer;

	// movement management
	bool m_pathFound; 
	Path m_path;
	NYVert3Df m_destination;
	Pathfinding * m_pathFinding = Pathfinding::GetSingleton();
	int pathFindingRange = 10; // number of cube around the cameleon in which he will choose a new destination
	int m_waypointIndex; // index of the current waypoint of the path leading to destination
	int m_speed =2; // speed in cube/s
	float m_pathFindingClock = 0; // clock to manage the time between each pathfinding
	float m_pathFindingClockStep = 0.5f; // time in seconds between each pathfinding

	// perception management
	int m_moucheFindingRange = 5; //  number of cube around the cameleon in which he will look for a Mouche
	float m_mouchFindingClock = 0; // clock to manage the time between each moucheFinding
	float m_moucheFindingClockStep = 1.0f; // time in seconds between each moucheFinding
	int m_moucheEatingRange = 1; // distance in cube where a Mouche can be eaten

	//hunger management
	int m_hunger = 0; 
	float m_starvationLimit = 100; // limit for hunger to reach to cause the death of the cameleon
	float m_hungerClock = 0; // clock to manage the incrementation of hunger
	float m_hungerClockStep = 1.0f; // time in seconds between each incrementation of m_hunger

	// reproduction management
	int m_reproductionClock = 0; // clock to manage the time of reproduction
	int m_reproductionClockStep = 120; // time in seconds between each reproduction phases
	
	//-------methods--------//

	// find the index in (*m_entities)[MOUCHE] of the closest Mouche in _range distance
	int  MoucheFinding(int _range);

public:
	//----- attributes -----//

	// all the entities of the world
	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;


	//-------methods--------//
	Cameleon(NYWorld * _world, NYVert2Df _positionInitiale);
	~Cameleon();

	void Draw();
	void UpdateIA();
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);



};


#endif _CAMELEON_H_