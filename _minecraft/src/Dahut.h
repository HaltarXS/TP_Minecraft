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

class Dahut : public IABase
{
private :

	//TEMP
	float path;

	//Pathfinding singleton
	Path m_path;
	Pathfinding *m_pPathfinder;

	//Determine the cube the Dahut is climbing/walking on
	NYPoint3D m_cubeAnchor;

	//Timer to determine delta time
	NYTimer m_lastUpdate;

public :

	Dahut(NYWorld *pWorld, NYVert2Df pos);
	~Dahut();

	//Override base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
};

#endif