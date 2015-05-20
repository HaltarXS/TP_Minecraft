/**
 * File : Dahut.h
 * Author : Paul Gerst
 * Description : Header file of the Dahut AI class
 */

#ifndef DAHUT_H
#define DAHUT_H

#include "engine\timer.h"
#include "IABase.h"
#include "Pathfinding.h"
#include "Viewcone.h"

class Dahut : public IABase
{
private :

	//Constant parameters
	const int m_eatingQty = 100;
	const int m_hungerThreshold = 100;
	const int m_smellDistance = 15;
	const float m_pathfindingInterval = 0.5f;

	//Pathfinding singleton
	int m_pathIndex;
	Path m_path;
	Pathfinding *m_pPathfinder;

	//Pathfinding goal
	NYVert2Df m_goalPosition;
	unsigned int m_goalState;

	//Movement parameters
	NYPoint3D m_targetCube;
	NYVert3Df m_targetPosition;
	NYVert3Df m_interPositions[8];
	int m_interIndex;
	int m_interMax;

	//Viewcone
	Viewcone m_viewCone;

	//Entities
	std::map<eTypeCreature, std::vector<IABase*>> *m_pEntities;

	//Timer to determine delta time
	NYTimer m_lastUpdate;
	NYTimer m_lastPathfinding;

public :

	Dahut(NYWorld *pWorld, NYVert2Df pos);
	~Dahut();

	//Init entities
	void SetEntities(std::map<eTypeCreature, std::vector<IABase*>> *pEntities);

	//Overriden base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);

private :

	//Check senses to avoid predators and smell poop
	bool Senses();

	//Helper function
	void SetTargetPosition();

	//Check if adjacent blocks are climbable
	bool CheckClimbable(int x, int y);

	//Movement functions, used in SetTargetPosition
	void UnClimb(const NYVert3Df &from, const NYVert3Df &to);
	void Climb(const NYVert3Df &from, const NYVert3Df &to);
	void MoveUnclimbed(const NYVert3Df &from, const NYVert3Df &to);
	void MoveClimbed(const NYVert3Df &from, const NYVert3Df &to);
};

#endif