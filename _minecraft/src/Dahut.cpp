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

	//Init pathfinding singleton
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
	if(m_path.GetSize() > 0)
		m_path.DrawPath();
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
		if(m_path.GetSize() > 1)
		{
			m_targetPosition = m_path.GetWaypoint(1);
			direction =  m_targetPosition - position;
			direction.normalize();
			m_pathIndex = 1;
		}
		else
		{
			PushState(STATE_FindPath);
		}
	}
		
	OnUpdate
	{
		position += direction * m_lastUpdate.getElapsedSeconds() * NYCube::CUBE_SIZE;

		NYVert3Df distance = m_targetPosition - position;
		if(distance.scalProd(direction) < 0.0f)
		{
			position = m_targetPosition;

			if(++m_pathIndex < m_path.GetSize())
			{
				m_targetPosition = m_path.GetWaypoint(m_pathIndex);
				direction =  m_targetPosition - position;
				direction.normalize();
			}
			else
			{
				PushState(STATE_FindPath);
			}
		}

		positionCube.X = (int) position.X/NYCube::CUBE_SIZE;
		positionCube.Y = (int) position.Y/NYCube::CUBE_SIZE;
		positionCube.Z = (int) position.Z/NYCube::CUBE_SIZE;
	}

	EndStateMachine
}