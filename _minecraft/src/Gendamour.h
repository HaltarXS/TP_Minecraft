/*  Gendamour / Leroy Nicolas */


#ifndef GENDAMOUR_H
#define GENDAMOUR_H

#include "IABase.h"
#include "Pathfinding.h"

class Gendamour : public IABase
{
public:
	Gendamour(NYWorld *pWorld, NYVert2Df pos);
	~Gendamour();


	//truc a override
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
	virtual void UpdateIA();
};

#endif