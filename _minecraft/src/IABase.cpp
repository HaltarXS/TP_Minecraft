#include "IABase.h"


IABase::IABase(NYWorld * world):
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

	m_world = world;
}


IABase::~IABase()
{
}

void IABase::UpdateHunger(float elapsed, float totalTime)
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

void IABase::Manger()
{
	saciete = m_saciete_Time;
	t = 0;
}


