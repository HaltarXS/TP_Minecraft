/**
 * File : Dahut.h
 * Author : Paul Gerst
 * Description : Source file of the Dahut AI class
 */

#include "Dahut.h"

Dahut::Dahut(NYWorld *pWorld, NYVert2Df pos):
IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = DAHUT;

	//Init transform
	position.X = pos.X;
	position.Y = pos.Y;
	position.Z = pWorld->_MatriceHeights[(int) pos.X][(int) pos.Y];
}

Dahut::~Dahut()
{
}