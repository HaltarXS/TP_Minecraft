#include "AnimauxBase.h"


AnimauxBase::AnimauxBase()
{
	A = 5;
	B = 5;
	t = 0;
	Saciete = 0;
	Faim = 0;
	Position = NYVert3Df(0, 0, 0);
	PositionCube = NYVert3Df(0, 0, 0);
	Direction = NYVert3Df(0, 0, 0);
	Speed = NYVert3Df(0, 0, 0);
}


AnimauxBase::~AnimauxBase()
{
}

void AnimauxBase::update(float elapsed, float totalTime)
{
	if (Saciete > 0.0)
	{
		Saciete -= elapsed;
	}
	else
	{
		t += elapsed;
	}
	this->Faim = A*t + B*t*(cos(totalTime) + 1);
}

void AnimauxBase::manger()
{
	Saciete = Saciete_Time;
	t = 0;
}

void AnimauxBase::moveTo(NYVert3Df destinationCube, NYCube* cubes)
{
	float estimatedCost = (destinationCube - PositionCube).getSize();
	
}
