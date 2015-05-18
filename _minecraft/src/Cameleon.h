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
public:

	int m_waypointIndex;
	Pathfinding * m_pathFinding = Pathfinding::GetSingleton();
	bool m_pathFound;
	Path m_path;
	float m_speed = 0.7f;


	NYTimer m_timer;
	NYVert3Df m_destination;

	bool hasTargetedMouch = false;

	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;



	Cameleon(NYWorld * _world, NYVert2Df _positionInitiale);
	~Cameleon();

	void Draw();
	void UpdateIA();
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
	int  findClosestMoucheInRange(int _range);

};


#endif _CAMELEON_H_