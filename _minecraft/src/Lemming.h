/*
 * Header file : Lemming AI class
 * Author : DESHORS Axel
 * Date : 15/05/2015
 */


#ifndef _LEMMING_H_
#define _LEMMING_H_

#include "IABase.h"
class Lemming : public IABase
{
public:
	Lemming(NYWorld * _world, NYVert2Df _spawnPos);
	~Lemming();


private:
	// Variables :
	float m_walkSpeed = 0.5f;


	// Gauge limitation
	const int m_maxHungerPoints = 200;
	const int m_maxLifePoints = 100;
	const int m_maxReproductionPoints = 100;

	// Real time gauge :
	int m_currentHungerPoints;
	int m_currentReproductionPoints;
	int m_currentLifePoints;

	// Timer :
	float m_timeElapsed = 0.f;

	// Functions :
	bool IsHungry();	// If true : the lemming is hungry

};

#endif _LEMMING_H_