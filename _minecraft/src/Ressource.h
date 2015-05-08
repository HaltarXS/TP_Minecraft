#pragma once
#include <algorithm>
#include <map>
#include "gl/glew.h"
#include "gl/freeglut.h" 
#include "engine/utils/types_3d.h"





enum TypeRessource
{
	NONE,
	HERBE
};


class Ressource
{
protected:
	int MaxQuantity;
public:
	TypeRessource Type;
	static const int ILLIMITED_QUANTITY = -1;
	int Quantity;
	NYVert3Df Position;
	NYVert3Df CubePostion;

	Ressource();
	Ressource(NYVert3Df position, int maxQuantity);
	~Ressource();

	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;


	/**
	* Retourne la quantité manger.
	*/
	virtual int Use(int neededQuantity);
	

};
