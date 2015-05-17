#include "cameleon.h"

// _world : pointer to the world object in which the entity will be place
// _startingPosition : starting position of the entity in cube matrix coordinates
Cameleon::Cameleon(NYWorld * _world, NYVert2Df _startingPosition) : IABase(_world)
{

	Initialize(); // Initialize the FSM

	type = CAMELEON;
	
	positionCube.X = (int)_startingPosition.X;
	positionCube.Y = (int)_startingPosition.Y;
	positionCube.Z = (int)_world->_MatriceHeights[(int)_startingPosition.X][(int)_startingPosition.Y];

	//calculation of position in absolute coordinates
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;


	m_timer.start();

}


Cameleon::~Cameleon()
{
}


void Cameleon::Draw()
{

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
		glTranslatef(position.X, position.Y, position.Z);
		glColor4f(0.0f,1.0f, 0.0f,0.5f);
		//glColor4f(0.0f,0.0f, 1.0f,1.0f);
		glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
		//glutSolidCube(10);
	glPopMatrix();

	glDisable(GL_BLEND);

	if (m_path.GetSize() > 0)
		m_path.DrawPath();
}

void Cameleon::UpdateIA()
{
	//Update FSM
	Update();

	//Start timer
	m_timer.start();
}



bool Cameleon::States(StateMachineEvent event, MSG_Object * msg, int state){
	BeginStateMachine

		OnMsg(MSG_Attack){}

		//Initialize
	State(STATE_Initialize)
		OnEnter
		{
			PushState(STATE_FindPath);
		}


	State(STATE_Egg)
		OnEnter{}
		OnUpdate{}
		OnExit{}
	
	State(STATE_FindPath)
		OnEnter{}
		OnUpdate
		{
			bool pathFound = false;
			while (!pathFound)
			{
				NYVert3Df newPosition;
				newPosition.X = (int)(positionCube.X + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
				newPosition.Y = (int)(positionCube.Y + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
				newPosition.Z = (int)m_world->_MatriceHeights[(int)newPosition.X][(int)newPosition.Y];

				if (newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
					newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
					newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
				{
					pathFound = m_pathFinding->FindPath(positionCube, newPosition , 1, m_path);
					//pathFound = m_pathFinding->FindPathDahut(positionCube, newPosition, m_path);

				}
			}
			PushState(STATE_Move);
		}
		OnExit{}

	State(STATE_Move)
		OnEnter
		{
			if (m_path.GetSize() > 1)
			{
				m_destination = m_path.GetWaypoint(1);
				direction = m_destination - position;
				direction.normalize();
				m_waypointIndex = 1;
			}
			else
			{
				PushState(STATE_FindPath);
			}
		}

		OnUpdate
		{
			position += direction * m_timer.getElapsedSeconds() * NYCube::CUBE_SIZE;

			NYVert3Df distance = m_destination - position;
			if (distance.scalProd(direction) < 0.0f)
			{
				position = m_destination;
				m_waypointIndex++;

				if (m_waypointIndex< m_path.GetSize())
				{
					m_destination = m_path.GetWaypoint(m_waypointIndex);
					direction = m_destination - position;
					direction.normalize();
				}

				else
				{
					PushState(STATE_FindPath);
				}
			}

			positionCube.X = (int)position.X / NYCube::CUBE_SIZE;
			positionCube.Y = (int)position.Y / NYCube::CUBE_SIZE;
			positionCube.Z = (int)position.Z / NYCube::CUBE_SIZE;
		}
		OnExit{}
	
	State(STATE_Reproduction)
		OnEnter{}
		OnUpdate{}
		OnExit{}

	State(STATE_Suicide)
		OnEnter{}
		OnUpdate{}
		OnExit{}

	State(STATE_Dead)
		OnMsg(MSG_Attack){}
		OnEnter{}
		OnUpdate{}
		OnExit{}

	EndStateMachine

}