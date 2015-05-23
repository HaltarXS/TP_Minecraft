/**
* File : Snake.h
* Author : Vincent Swaenepoel
* Description : Header file of the Snake AI class
*/

#ifndef SNAKE_H
#define SNAKE_H

#include "IABase.h"
#include "Pathfinding.h"
#include "engine/timer.h"
#include <math.h>
#include "Crotte.h"
#include "RessourcesManager.h"

class Snake : public IABase
{
private:

	const float m_moveDuration = 10.0f;
	float m_timerMove = 0.0f;

	int m_life = 100;
	float m_timerSleep = 0.0f;
	float m_sleepDuration = 0.5f;

	int m_size;
	NYWorld* world;

	std::vector<NYVert3Df> m_listPosition;

public:
	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;

	Snake(NYWorld *pWorld, NYVert2Df pos, int size);
	~Snake();

	bool Proximity(NYVert3Df food);

	void UpdateIA();
	void Draw();
	bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

#endif

