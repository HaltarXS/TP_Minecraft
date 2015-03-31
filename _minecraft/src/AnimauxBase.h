#pragma once
#include <math.h>

#include "engine/utils/types_3d.h"
#include "cube.h"

using namespace std;

class AnimauxBase
{
private:
	float A;
	float B;
	float t;
	float Saciete_Time = 10.0;
	
public:
	NYVert3Df Position;
	NYVert3Df PositionCube;
	NYVert3Df Direction;
	NYVert3Df Speed;
	float Faim;
	float Saciete;
	AnimauxBase();
	~AnimauxBase();

	void update(float elapsed, float totalTime);
	void manger();
	void moveTo(NYVert3Df destinationCube, NYCube* cubes);
};

