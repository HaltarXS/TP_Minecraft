#pragma once
#include <algorithm>

#include "engine/utils/types_3d.h"
enum TypeRessource{
	None,
	Herbe
};
class Ressource
{
private:
	int MaxQuantity;
public:
	TypeRessource Type;
	static const int ILLIMITED_QUANTITY = -1;
	int Quantity;
	NYVert3Df Position;

	Ressource(NYVert3Df position, int maxQuantity);
	~Ressource();

	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;


	/**
	* Retourne la quantité manger.
	*/
	virtual int Use(int neededQuantity);
	

};

