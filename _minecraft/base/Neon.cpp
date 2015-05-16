/**
* File : Dahut.h
* Author : Paul Gerst
* Description : Source file of the Dahut AI class
*/

#include "Neon.h"

Neon::Neon(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = NEON;

	//Init position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;

	//Init pathfinding singleton
	//m_pPathfinder = Pathfinding::GetSingleton();

	//Init timer
	//m_lastUpdate.start();
}


Neon::~Neon()
{

}

void Neon::UpdateIA()
{
	
}

void Neon::Draw()
{
	//Basic rendering
	glColor3f(0, 0, 255);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	glPopMatrix();

}

bool Neon::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

	State(STATE_Initialize)
	OnEnter
	{
		PushState(STATE_FindPath);
	}

	State(STATE_FindPath)
	OnUpdate
	{

	}

	State(STATE_Move)
	OnEnter
	{
		
	}

	OnUpdate
	{
		
	}

	EndStateMachine
}