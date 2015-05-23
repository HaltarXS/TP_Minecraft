/**
* File : Gevaulol.h
* Author : Lucien Catonnet
* Description : Header file of the Gevaulol AI class
*/

#ifndef GEVAULOL_H
#define GEVAULOL_H

#include "engine\timer.h"
#include "IABase.h"

class Gevaulol : public IABase
{
private:
	NYTimer m_timer;


public:
	static std::map<eTypeCreature, std::vector<IABase*>> *creatureMap;

	Gevaulol(NYWorld *pWorld, NYVert2Df pos);
	float repulsionFactor, orientationFactor, attractionFactor;
	float repulsionRadius, orientationRadius, attractionRadius;
	float maxSpeed, perceptionRadius;

	//Overriden base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
};

#endif