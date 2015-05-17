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
		//glColor4f(0.0f,1.0f, 0.0f,0.5f);
		glColor4f(0.0f,0.0f, 1.0f,1.0f);
		glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
		glutSolidCube(10);
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
		OnEnter
		{

			bool m_pathFound = false;
		}
		OnUpdate
		{

			//Recherche d'une destination aléatoire dans la neige
			if (!m_pathFound)
			{
				NYVert2Df destination = NYVert2Df(rand() % MAT_SIZE_CUBES, rand() % MAT_SIZE_CUBES);
				NYVert2Df start = NYVert2Df(positionCube.X, positionCube.Y);

				m_pathFound = m_pathFinding->FindPath(start, destination, CUBE_HERBE | CUBE_NEIGE | CUBE_TERRE, m_path);

			}
			else
			{
				PushState(STATE_Move);
			}

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
			if (m_waypointIndex < m_path.GetSize() )
			{
				//On récupère la direction
				direction = m_path.GetWaypoint(m_waypointIndex) - position;
				float lenght = direction.getSize();
				direction.normalize();

				if (lenght < 1.0f)
				{
					m_waypointIndex++;
				}
				else
				{
					position += direction * m_speed * m_timer.getElapsedSeconds();
				}
			}
			else
			{
				PushState(STATE_FindPath);
			} 
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