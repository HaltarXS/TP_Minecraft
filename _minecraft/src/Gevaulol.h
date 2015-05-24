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
	NYTimer m_timer, m_lastbreeding;


public:
	static std::map<eTypeCreature, std::vector<IABase*>> *creatureMap;
	static const float breedingWaitTime, breedingRadius;

	Gevaulol(NYWorld *pWorld, NYVert2Df pos);
	float repulsionFactor, orientationFactor, attractionFactor, attackFactor;
	float repulsionRadius, orientationRadius, attractionRadius, attackRadius;
	float maxSpeed, perceptionRadius;

	inline bool canBreed(){ return m_lastbreeding.getElapsedSeconds() > breedingWaitTime; }
	static void breed(Gevaulol&, Gevaulol&);
	static float mutate(float a, float b);

	//Overriden base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);

	inline static float randFloat(){
		return ((rand() % 4096) - 2048) / 2048.f;
	}
};

#endif