/**
* File : Neon.h
* Author : Lucas Le Gouic
* Description : Header file of the Dahut AI class
*/

#ifndef NEON_H
#define NEON_H

#include "engine\timer.h"
#include "IABase.h"
#include "Pathfinding.h"

class Neon : public IABase
{


public:
	Neon(NYWorld *pWorld, NYVert2Df pos);
	~Neon();

	//Override base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
};

#endif