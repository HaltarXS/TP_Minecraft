#pragma once
#include "IABase.h"
#include "engine\timer.h"

class Crabe :
	public IABase
{
public:
	Crabe(NYWorld* world,NYVert2Df spawnPos);
	Crabe(NYWorld* world, NYVert2Df spawnPos,bool axeX);
	~Crabe();
	void Draw();
	void UpdateIA();
	bool States(StateMachineEvent event, MSG_Object * msg, int state);
	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;	// All entities of the game
	
private:
	float Size;
	float Speed;
	float Reproduction = 60.0f;
	float MaxTimeReprod;
	NYTimer m_timer;

	bool LeftToRight;
	bool AxeX;
	void Update(float _elapsedTime);

	void Manger();
};

