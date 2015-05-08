#include "IABase.h"


IABase::IABase():
StateMachine()
{
	A = 5;
	B = 5;
	t = 0;
	saciete = 0;
	faim = 0;
	position = NYVert3Df(0, 0, 0);
	positionCube = NYVert3Df(0, 0, 0);
	direction = NYVert3Df(0, 0, 0);
	speed = NYVert3Df(0, 0, 0);

	type = BASE;
}


IABase::~IABase()
{
}

void IABase::updateHunger(float elapsed, float totalTime)
{
	if (saciete > 0.0)
	{
		saciete -= elapsed;
	}
	else
	{
		t += elapsed;
	}
	this->faim = A*t + B*t*(cos(totalTime) + 1);
}

void IABase::manger()
{
	saciete = Saciete_Time;
	t = 0;
}


