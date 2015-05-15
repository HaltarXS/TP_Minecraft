#pragma once
#include "Ressource.h"
class Crotte :
	public Ressource
{
public:

	Crotte(NYVert3Df position, int maxQuantity);
	~Crotte();

	virtual void Update(float deltaTime);
	virtual void Render();
};

