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

	//Init position
	positionCube.X = (int) pos.X;
	positionCube.Y = (int) pos.Y;
	positionCube.Z = (int) pWorld->_MatriceHeights[(int) pos.X][(int) pos.Y];
	
	//Init anchor
	m_cubeAnchor.X = (int) pos.X;
	m_cubeAnchor.Y = (int) pos.Y;
	m_cubeAnchor.Z = (int) positionCube.Z - 1;
}

Dahut::~Dahut()
{
}

void Dahut::UpdateIA()
{
	//Update FSM
	Update();
}

void Dahut::Draw()
{

}

bool Dahut::States(StateMachineEvent event, MSG_Object *pMsg, int state)
{
	return true;
}