/*
* Header file : Lemming AI class
* Author : DESHORS Axel
* Date : 16/05/2015
*/


#ifndef _LEMMING_H_
#define _LEMMING_H_


#include "IABase.h"

#include "Viewcone.h"
#include "Path.h"

#include "engine\timer.h"

class Lemming : public IABase
{
public:
	Lemming(NYWorld * _world, NYVert2Df _spawnPos);
	~Lemming();

	// Functions :
	void Draw();	// Draw the lemming
	void UpdateIA();
	bool States(StateMachineEvent event, MSG_Object * msg, int state);


	// Variables :
	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;	// All entities of the game

private:
	// Variables :
	float m_walkSpeed = 0.5f;

	// Gauge limitation
	static const int m_maxHungerPoints = 200;
	static const int m_maxLifePoints = 100;
	static const int m_maxReproductionPoints = 100;

	// Real time gauge :
	int m_currentHungerPoints;
	int m_currentReproductionPoints;
	int m_currentLifePoints;

	// Timer :
	float m_timeElapsed = 0.f;	// Time elapsed reset to 0 every 1 sec (used for the HungerPoints and ReproductionPoints)
	float m_dancingTime = 0.f;	// Time the lemming passed to dance
	float m_randMovingTime = 0.f;	// Time the lemming passed to move around
	NYTimer m_timer;	// Timer of the entity



	// Times :
	const float m_danceTime = 10.f;

	// Perceptions :
	std::vector<IABase*> m_visibleCreatures;	// vector of visible creatures
	IABase * m_target;	// Target to follow for the lemming
	Viewcone m_view;	// View of the creature
	Path m_path;	// Path of the lemming
	NYVert3Df m_moveVector;	// Movement vector

	// Functions :
	bool IsHungry();	// If true : the lemming is hungry
	void GetVisibleCreatures();	// Get the visible creatures (Update the visible creatures vector)
	void Update(float elapsed_time);	// Update timers and gauges
	void Eat();	// Eat a piece of snow

};

#endif _LEMMING_H_