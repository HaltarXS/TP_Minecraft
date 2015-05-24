/*
* Header file : Lemming AI class
* Author : DESHORS Axel
* Date : 17/05/2015
*/


#ifndef _LEMMING_H_
#define _LEMMING_H_


#include "IABase.h"

#include "Viewcone.h"
#include "Path.h"
#include "Pathfinding.h"

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

	void Born();	// Born for a lemming created in game

	bool IsDancing();	// Return true if the lemming is dancing

	void SetEntities(std::map<eTypeCreature, std::vector<IABase *>> * _entities);

	// Variable :
	// Draw/Print debug
	bool m_printDebug = false;
	bool m_drawDebug = false;

private:
	// Variables :
	float m_walkSpeed = 30.f;

	// Gauge limitation
	static const int m_maxHungerPoints = 200;
	static const int m_maxLifePoints = 100;
	static const int m_maxReproductionPoints = 10;

	// Optimisation
	const float m_checkVisibilityTime = 0.5f;
	float m_checkVisibility = 0.f;

	// Real time gauge :
	int m_currentHungerPoints;
	int m_currentReproductionPoints;

	// Timer :
	float m_timeElapsed = 0.f;	// Time elapsed
	float m_tick = 0.f;	// Tick every one second for the gauge
	float m_dancingTime = 0.f;	// Time the lemming passed to dance
	float m_inactiveTimer = 5.f;	// Time during the lemming is following anybody
	NYTimer m_timer;	// Timer of the entity
	NYTimer m_debugTimer;	// Timer to find path!
	// Times :
	const float m_danceTime = 10.f;

	// Perceptions :
	std::vector<IABase*> m_visibleCreatures;	// vector of visible creatures
	IABase * m_followedTarget;	// Target to follow for the lemming
	Lemming * m_partner;	// Partner for the reproduction
	Viewcone m_view;	// View of the creature
	Path m_path;	// Path of the lemming
	NYVert3Df m_destination;	// The destination of the player
	Pathfinding * m_pathfind;	// Pointer on the pathfind singleton
	int m_currentPathIndex;	// Index on the path
	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;	// All entities of the game

	// Functions :
	bool IsHungry();	// If true : the lemming is hungry
	bool CanReproduce();	// If true : the lemming can reproduce
	void GetVisibleCreatures();	// Get the visible creatures (Update the visible creatures vector)
	void Update(float _elapsedTime);	// Update timers and gauges
	void Reproduce();	// Update gauge
	void Eat();	// Eat a piece of snow
	void GetTargetToFollow();	// Take a target to follow
	bool TargetIsVisible();	// If true : the target followed by the Lemming is visible
	NYVert3Df GetMoveVector();	// Get the vector movement
	bool UpdatePosition();	// Update the position for the Lemming (if true : End of the travel)
	bool FindPath(int _x, int _y);	// Find path to destination (update the m_path) return true if the path is possible

};

#endif _LEMMING_H_