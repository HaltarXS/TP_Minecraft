#include "cameleon.h"

// _world : pointer to the world object in which the entity will be place
// _startingPosition : starting position of the entity in cube matrix coordinates
Cameleon::Cameleon(NYWorld * _world, NYVert2Df _startingPosition) : IABase(_world)
{

	Initialize();

	type = CAMELEON;

	positionCube.X = (int)_startingPosition.X;
	positionCube.Y = (int)_startingPosition.Y;
	positionCube.Z = (int)_world->_MatriceHeights[(int)_startingPosition.X][(int)_startingPosition.Y];

	//calculation of position in absolute coordinates
	position.X = positionCube.X*NYCube::CUBE_SIZE ;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE ;
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

		//body
		glTranslatef(position.X, position.Y, position.Z);
		if (GetState() == STATE_Dead) // if cameleon is dead
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f); // then his corpse his grey
		else
			glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glutSolidCube(NYCube::CUBE_SIZE / 4.0f);

		//eyes
		glColor4f(0.0f,0.0f, 0.0f, 1.0f);
		glPushMatrix();
			glTranslatef(1,1,1);
			glutSolidCube(NYCube::CUBE_SIZE / 16.0f);
		glPopMatrix();		
		glPushMatrix();
			glTranslatef(-1, 1, 1);
			glutSolidCube(NYCube::CUBE_SIZE / 16.0f);
		glPopMatrix();

	glPopMatrix();

	glDisable(GL_BLEND);

	if (m_debug && m_path.GetSize() > 0)m_path.DrawPath();
}

void Cameleon::UpdateIA()
{
	m_hungerClock = m_hungerClock + m_timer.getElapsedSeconds();
	m_reproductionClock = m_reproductionClock + m_timer.getElapsedSeconds();
	m_mouchFindingClock = m_mouchFindingClock + m_timer.getElapsedSeconds();
	m_pathFindingClock = m_pathFindingClock + m_timer.getElapsedSeconds();
	if (m_hungerClock > m_hungerClockStep){// every second Leon get hungrier
		m_hunger++;
		m_hungerClock = 0;
	}
	if (m_hunger >= m_starvationLimit) // if leon hasn't eaten since the last 100 seconds, he will die !
		PushState(STATE_Dead);


	if (m_reproductionClock >m_reproductionClockStep){ // after m_reproductionClockStep seconds, leon lay an egg
		m_reproductionClock = 0;

		Cameleon * minileon = new Cameleon(m_world, NYVert2Df(positionCube.X , positionCube.Y )); // spawning of a new cameleon
		(*m_entities)[CAMELEON].push_back(minileon);
	}

	//Update FSM
	Update();

	//Start timer
	m_timer.start();
}

int  Cameleon::MoucheFinding(int _range){
	NYVert3Df smallestDistance = NYVert3Df(_range, _range, _range);
	int closestMoucheIndex=-1;
	for (int i = 0; i < (*m_entities)[MOUCHE].size(); i++)
	{
		if (((*m_entities)[MOUCHE][i]->GetState() != STATE_Initialize && // if mouche is not dead
			( (*m_entities)[MOUCHE][i]->positionCube - positionCube).getSize() < smallestDistance.getSize() ) ){ // if mouche is ccloser than previous closest mouche
			smallestDistance = ((*m_entities)[MOUCHE][i]->positionCube - positionCube);
			closestMoucheIndex = i;
		}
	}
	if (closestMoucheIndex == -1)
		return -1;// no mouche in range :'( ? 

	NYVert3Df closestMouchePositionCube = (*m_entities)[MOUCHE][closestMoucheIndex]->positionCube;
	NYVert3Df closestMouchePosition = (*m_entities)[MOUCHE][closestMoucheIndex]->position;

	return closestMoucheIndex;
}



bool Cameleon::States(StateMachineEvent event, MSG_Object * msg, int state){
	BeginStateMachine

		OnMsg(MSG_Attack)
		{
			PushState(STATE_Dead);
		}

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
		OnEnter{

		}
		OnUpdate
		{
			if (m_pathFindingClock > m_pathFindingClockStep){ // if it's time to look for a path
				m_pathFindingClock = 0;

				int _x, _y;
				int _closestMoucheIndex = MoucheFinding(m_moucheFindingRange);
				if (_closestMoucheIndex == -1){ // if no mouche in a range
					int i = rand() % 2;
					int j = 1;
					if (i == 0) j = -1;
					_x = positionCube.X + (rand() % pathFindingRange)*j; // then cameleon goes wandering around
					_y = positionCube.Y + (rand() % pathFindingRange)*j;

					// checking if not out of bounds 
					if (_x < 1)_x = 1;
					if (_y < 1)_y = 1;
					if (_x > MAT_SIZE_CUBES)_x = MAT_SIZE_CUBES - 1;
					if (_y > MAT_SIZE_CUBES)_y = MAT_SIZE_CUBES - 1;

				}
				else{// else he goes to the closest Mouche

					m_destination = (*m_entities)[MOUCHE][_closestMoucheIndex]->positionCube;
					_x = m_destination.X;
					_y = m_destination.Y;
				}

				m_pathFinding->FindPath(NYVert2Df(positionCube.X, positionCube.Y), NYVert2Df(_x, _y), 1, m_path);

				PushState(STATE_Move);
			}

		}
		OnExit{}

		State(STATE_Move)
		OnEnter
		{

			if (m_path.GetSize() > 1) // if our path is not empty
			{
				direction = m_path.GetWaypoint(0) - position; // we determine the direction
				direction.normalize();
				m_waypointIndex = 0; // and start aiming for the first waypoint
			}
			else // else we need to find a new path
				PushState(STATE_FindPath);
		}

		OnUpdate
		{
			if (m_mouchFindingClock > m_moucheFindingClockStep){ // if it's time to look for a Mouche
				m_mouchFindingClock = 0;
				int _closestMoucheIndex = MoucheFinding(m_moucheEatingRange);
				if (_closestMoucheIndex != -1 && (*m_entities)[MOUCHE][_closestMoucheIndex]->GetState() != STATE_Initialize){ // living mouche in range ! Fire at will !
					(*m_entities)[MOUCHE][_closestMoucheIndex]->SendMsg(MSG_Attack, (*m_entities)[MOUCHE][_closestMoucheIndex]->GetID());
					m_hunger--; // the cameleon is less hungry
				}
			}
			else{ // no mouche in range, let's move

				position += direction * m_speed *NYCube::CUBE_SIZE* m_timer.getElapsedSeconds() ;
				positionCube = position / NYCube::CUBE_SIZE;

				NYVert3Df _dist = m_path.GetWaypoint(m_waypointIndex) - position;

				if (_dist.getSize() <= 5.f)// if we are near enough the destination waypoint 
					m_waypointIndex++; // we switch to next waypoint

				if (m_waypointIndex < m_path.GetSize()) // as long as we haven't reach the last waypoint
				{
					direction = m_path.GetWaypoint(m_waypointIndex) - position; // we aim for the next
					direction.normalize();
				}
				else //if we have reached our destination
				{
					m_path.Clear();	
					PushState(STATE_FindPath); // we try to find a new path
				}
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
		OnEnter{}
		OnUpdate{}
		OnExit{}

		EndStateMachine

}