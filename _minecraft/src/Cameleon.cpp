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
	glTranslatef(position.X, position.Y, position.Z);
	glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
	//glColor4f(0.0f,0.0f, 1.0f,1.0f);
	glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	//glutSolidCube(10);
	glPushMatrix();

	glPopMatrix();
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

int  Cameleon::findClosestMoucheInRange(int _range){
	NYVert3Df smallestDistance = NYVert3Df(_range, _range, _range);
	int closestMoucheIndex=-1;
	for (int i = 0; i < (*m_entities)[MOUCHE].size(); i++)
	{
		if (((*m_entities)[MOUCHE][i]->positionCube - positionCube).getSize() < smallestDistance.getSize()){
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
		OnEnter{

		}
		OnUpdate
		{

			int _x ,_y;
			int _closestMoucheIndex = findClosestMoucheInRange(5);
			if (_closestMoucheIndex == -1){ // si il n'y a pas de mouche dans un rayon de 5 cubes

				 _x = positionCube.X+ rand( )% 5; // le cameleon part se balader
				 _y = positionCube.Y+ rand() % 5;

			}
			else{// sinon il se dirige vers la mouche en question

				m_destination = (*m_entities)[MOUCHE][_closestMoucheIndex]->positionCube;
				 _x = m_destination.X;
				 _y = m_destination.Y;
			}
			//std::cout << "Cameleon on the move" << std::endl;



			m_pathFinding->FindPath(NYVert2Df(positionCube.X, positionCube.Y), NYVert2Df(_x, _y), 1, m_path);

			PushState(STATE_Move);

		}
		OnExit{}

		State(STATE_Move)
		OnEnter
		{

			if (m_path.GetSize() > 1)
			{
				direction = m_path.GetWaypoint(0) - position;
				direction.normalize();
				m_waypointIndex = 0;
			}
			else 
				PushState(STATE_FindPath);
		}

		OnUpdate
		{
			int _closestMoucheIndex = findClosestMoucheInRange(1);
			if (_closestMoucheIndex != -1){ // mouche in range ! Fire at will !
				(*m_entities)[MOUCHE][_closestMoucheIndex]->SendMsg(MSG_Attack, (*m_entities)[MOUCHE][_closestMoucheIndex]->GetID());
			}
			else{ // no mouche in range, let's move

				position += direction * m_speed * m_timer.getElapsedSeconds() * 30;
				positionCube = position / NYCube::CUBE_SIZE;

				NYVert3Df _dist = m_path.GetWaypoint(m_waypointIndex) - position;

				if (_dist.getSize() <= 5.f)
					m_waypointIndex++;

				if (m_waypointIndex < m_path.GetSize())
				{
					direction = m_path.GetWaypoint(m_waypointIndex) - position;
					direction.normalize();
				}
				else
				{
					m_path.Clear();	
					PushState(STATE_FindPath);
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
		OnMsg(MSG_Attack){}
	OnEnter{}
		OnUpdate{}
		OnExit{}

		EndStateMachine

}