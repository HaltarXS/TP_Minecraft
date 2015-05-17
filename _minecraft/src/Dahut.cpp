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
}

bool Dahut::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine
	
	//Init FSM
	State(STATE_Initialize)
	OnEnter
	{
		PushState(STATE_FindPath);
	}
	
	State(STATE_FindPath)
	OnUpdate
	{
		//Look for a path
		bool pathFound = false;
		while(!pathFound)
		{
			//Randomize a new path
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

		//Move to the new path
		PushState(STATE_Move);
	}

	State(STATE_Move)
	OnEnter
	{
		//Init indexes
		if(m_path.GetSize() > 1)
		{
			m_pathIndex = 1;
			SetTargetPosition();
			direction = m_interPositions[m_interIndex] - position;
			direction.normalize();
		}
		else
		{
			PushState(STATE_FindPath);
		}
	}

	OnUpdate
	{
		//Update position
		position += direction * m_lastUpdate.getElapsedSeconds() * NYCube::CUBE_SIZE;

		//Check if we reached the next position
		NYVert3Df distance = m_interPositions[m_interIndex] - position;
		if(distance.scalProd(direction) < 0.0f)
		{
			position = m_interPositions[m_interIndex];
			if(++m_interIndex >= m_interMax)
			{
				if(++m_pathIndex < m_path.GetSize())
				{
					//Set new target positions
					SetTargetPosition();
				}
				else
				{
					//Find another path
					PushState(STATE_FindPath);
				}
			}

			//Update direction
			direction = m_interPositions[m_interIndex] - position;
			direction.normalize();
		}

		//Update cube position
		positionCube.X = (int) position.X/NYCube::CUBE_SIZE;
		positionCube.Y = (int) position.Y/NYCube::CUBE_SIZE;
		positionCube.Z = (int) position.Z/NYCube::CUBE_SIZE;
	}

	EndStateMachine
}

void Dahut::SetTargetPosition()
{
	//Init points indexes to travel through
	m_interIndex = 0;
	m_interMax = 0;

	//Init target position
	m_targetPosition = m_path.GetWaypoint(m_pathIndex);
	m_targetCube.X = m_targetPosition.X / NYCube::CUBE_SIZE;
	m_targetCube.Y = m_targetPosition.Y / NYCube::CUBE_SIZE;
	m_targetCube.Z = m_targetPosition.Z / NYCube::CUBE_SIZE;

	//Set climbing state
	bool climbing = (abs(position.Z / NYCube::CUBE_SIZE - (int) (position.Z / NYCube::CUBE_SIZE)) > 0.01f);
	bool climbable = false;
	bool checkClimbable = (m_pathIndex+1 != m_path.GetSize() &&
						   m_path.GetWaypoint(m_pathIndex+1).Z == m_targetPosition.Z);
	
	//Check if target blocks are climbable, only if we are not about
	//to change latitude
	if(checkClimbable &&
	   (CheckClimbable(1, 0) ||
	    CheckClimbable(0, 1) ||
	    CheckClimbable(-1, 0) ||
	    CheckClimbable(0, -1)))
	{
		climbable = true;
	}
	else
	{
		m_targetPosition.Z -= NYCube::CUBE_SIZE*0.5f;
	}

	//Considering the state (climbing and if next block is climbable),
	//call adequate movement functions
	if(!climbing && !climbable)
	{
		//Move on the ground
		MoveUnclimbed(position, m_targetPosition);
	}
	else if(!climbing && climbable)
	{
		//Move on the ground, then climb
		NYVert3Df center(m_path.GetWaypoint(m_pathIndex));
		center.Z -= NYCube::CUBE_SIZE/2.0f;
		MoveUnclimbed(position, center);
		Climb(center, m_targetPosition);
	}
	else if(climbing && !climbable)
	{
		//Unclimb, then move on the ground
		NYVert3Df center = m_path.GetWaypoint(m_pathIndex-1);
		center.Z -= NYCube::CUBE_SIZE/2.0f;
		UnClimb(position, center);
		MoveUnclimbed(center, m_targetPosition);
	}
	else
	{
		//If we are not climbing to the same latitude
		if(m_path.GetWaypoint(m_pathIndex-1).Z != m_path.GetWaypoint(m_pathIndex).Z)
		{
			//Unclimb, then move on the ground and climb again
			NYVert3Df centerA = m_path.GetWaypoint(m_pathIndex-1);
			centerA.Z -= NYCube::CUBE_SIZE/2.0f;
			NYVert3Df centerB(m_path.GetWaypoint(m_pathIndex));
			centerB.Z -= NYCube::CUBE_SIZE/2.0f;
			UnClimb(position, centerA);
			MoveUnclimbed(centerA, centerB);
			Climb(centerB, m_targetPosition);
		}
		else
		{
			//Move while climbing
			MoveClimbed(position, m_targetPosition);
		}
	}

	//Reset index
	m_interIndex = 0;
}

//Check if adjacent blocks are climbable, and update target position accordingly
bool Dahut::CheckClimbable(int x, int y)
{
	NYCubeType type = m_world->getCube(m_targetCube.X + x, m_targetCube.Y + y, m_targetCube.Z)->_Type;
	if(type == CUBE_TERRE || type == CUBE_HERBE)
	{
		m_targetPosition.X += NYCube::CUBE_SIZE*0.5f*x;
		m_targetPosition.Y += NYCube::CUBE_SIZE*0.5f*y;

		return true;
	}

	return false;
}

//Climbing movement from climbing position back to the ground
void Dahut::UnClimb(const NYVert3Df &from, const NYVert3Df &to)
{
	m_interPositions[m_interIndex] = from;
	m_interPositions[m_interIndex].Z = to.Z;
	m_interIndex++;
	m_interMax++;

	m_interPositions[m_interIndex] = to;
	m_interIndex++;
	m_interMax++;
}

//Climbing movement from the ground to climbing position
void Dahut::Climb(const NYVert3Df &from, const NYVert3Df &to)
{
	m_interPositions[m_interIndex] = to;
	m_interPositions[m_interIndex].Z = from.Z;
	m_interIndex++;
	m_interMax++;

	m_interPositions[m_interIndex] = to;
	m_interIndex++;
	m_interMax++;
}

//Movement from one block to another, considering the Dahut's not climbing
void Dahut::MoveUnclimbed(const NYVert3Df &from, const NYVert3Df &to)
{
	if(from.Z != to.Z)
	{
		m_interPositions[m_interIndex] = from;
		m_interPositions[m_interIndex].X += (to.X - from.X)/2.0f;
		m_interPositions[m_interIndex].Y += (to.Y - from.Y)/2.0f;
		m_interIndex++;
		m_interMax++;

		m_interPositions[m_interIndex] = m_interPositions[m_interIndex-1];
		m_interPositions[m_interIndex].Z = to.Z;
		m_interIndex++;
		m_interMax++;
	}

	m_interPositions[m_interIndex] = to;
	m_interIndex++;
	m_interMax++;
}

//Movement from one block to another, considering the Dahut's climbing
void Dahut::MoveClimbed(const NYVert3Df &from, const NYVert3Df &to)
{
	NYVert3Df direction = m_targetPosition - position;
	float xDiff = position.X/NYCube::CUBE_SIZE - (int)(position.X/NYCube::CUBE_SIZE);
	if(abs(xDiff) > 0.01f)
	{
		if(abs(direction.X) > 0.01f)
		{
			m_interPositions[m_interIndex] = from;
			m_interPositions[m_interIndex].X += direction.X;
			m_interIndex++;
			m_interMax++;
		}

		if(abs(direction.Y) > 0.01f)
		{
			m_interPositions[m_interIndex] = to;
			m_interIndex++;
			m_interMax++;
		}
	}
	else
	{
		if(abs(direction.Y) > 0.01f)
		{
			m_interPositions[m_interIndex] = from;
			m_interPositions[m_interIndex].Y += direction.Y;
			m_interIndex++;
			m_interMax++;
		}

		if(abs(direction.X) > 0.01f)
		{
			m_interPositions[m_interIndex] = to;
			m_interIndex++;
			m_interMax++;
		}
	}
}