#pragma once
#include "statemch.h"
#include <math.h>
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "world.h"

using namespace std;

enum eTypeCreature
{
	BASE = 0,
	TEST_STATEMACHINE,
	WASTEDOSAURE,
	DAHUT,
	CAMELEON,
	MOUCHE,
	LEMMING,
	GLACEGOUILLE,
	GENDAMOUR,
	SNAKE,
	NEON,
	GRIFFONKITU,
	FURZ,
	CRABE,
	PARASITE,
	VAUTOUR,
	GEVAULOL,
	NEON_BLEU,
	BIXI,
	NOKIA,
	YETI,
	RADIATOSAURE,
	//Cet identifiant doit rester à la fin de l'enumeration
	CREATURE_NUM
};

//Les créatures vont devoir hériter de cette classe
class IABase: public StateMachine
{
private:
	float A;
	float B;
	float t;
	float m_saciete_Time = 10.0f;

	
	
public:
	IABase(NYWorld * world);
	virtual ~IABase();

	NYVert3Df position;
	NYVert3Df positionCube;
	NYVert3Df direction;
	NYVert3Df speed;
	float faim;
	float saciete;

	bool infected = false; //If the entity is infected infected

	int life = 100;

	NYWorld * m_world;

	//Useful to know the type of the entity !
	eTypeCreature type;

	void UpdateHunger(float elapsed, float totalTime);
	void Manger();

	//Get the Object ID. Useful to send messages to other entities !
	inline objectID GetID()
	{
		return this->GetOwner()->GetID();
	}
	//Get a string version of the type of the creature
	const char* GetName() const;

	//Override this method !
	virtual void UpdateIA() = 0;

	//Override this method !
	virtual void Draw() = 0;

	//Override this method !
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state) = 0;

};

