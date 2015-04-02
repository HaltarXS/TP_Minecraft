#pragma once
#include "Ressource.h"
class Herbe :
	public Ressource
{
public:

	Herbe(NYVert3Df position, int maxQuantity);
	~Herbe();

	virtual void Update(float deltaTime);
	virtual void Render();


	/**
	* Retourne la quantité manger.
	*/
	//int Use(int neededQuantity);
};

