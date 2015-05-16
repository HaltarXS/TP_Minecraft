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
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE/2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE/2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;

	m_pPathfinder = Pathfinding::GetSingleton();

	//Init timer
	m_lastUpdate.start();
}

Dahut::~Dahut()
{
}

void Dahut::UpdateIA()
{
	//Update FSM
	Update();

	//Start timer
	m_lastUpdate.start();
}

void Dahut::Draw()
{
	//Basic rendering
	glColor3f(255, 255, 0);
	glPushMatrix();
		glTranslatef(position.X, position.Y, position.Z);
		glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	glPopMatrix();
	
	//Debug path
	/*
	if(m_path.GetSize() > 0)
		m_path.DrawPath();
	*/
}

bool Dahut::States(StateMachineEvent event, MSG_Object *msg, int state)
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
		bool pathFound = false;
		while(!pathFound)
		{
			NYVert3Df newPosition;
			newPosition.X = (int) (positionCube.X + rand()/((float)RAND_MAX) * 50.0f - 25.0f);
			newPosition.Y = (int) (positionCube.Y + rand()/((float)RAND_MAX) * 50.0f - 25.0f);
			newPosition.Z = (int) m_world->_MatriceHeights[(int) newPosition.X][(int) newPosition.Y];

			if(newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
			   newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
			   newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
			{
				pathFound = m_pPathfinder->FindPathDahut(positionCube, newPosition, m_path);
			}
		}
		PushState(STATE_Move);
	}

	State(STATE_Move)
	OnEnter
	{
		path = 0.0f;
	}
		
	OnUpdate
	{
		path += m_lastUpdate.getElapsedSeconds();
		if(path >= m_path.GetSize() - 1.0f)
		{
			PushState(STATE_FindPath);
			path = 0.0f;
		}
		else
		{
			int firstPoint = (int) path;
			int secondPoint = firstPoint + 1;
			float lerp = path - firstPoint;

			NYVert3Df firstVec = m_path.GetWaypoint(firstPoint);
			NYVert3Df secondVec = m_path.GetWaypoint(secondPoint);
			position = firstVec * (1.0f - lerp) + secondVec * lerp;

			positionCube.X = (int) position.X/10.0f;
			positionCube.Y = (int) position.Y/10.0f;
			positionCube.Z = (int) position.Z/10.0f;
		}
	}

	EndStateMachine
}