#pragma once
#include "Ressource.h"
class Herbe :
	public Ressource
{
public:
	Herbe(NYVert3Df position, int maxQuantity);
	~Herbe();
};

