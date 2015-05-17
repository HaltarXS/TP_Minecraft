#include "Gendamour.h"

Gendamour::Gendamour(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	type = GENDAMOUR;

	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];

	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;

	_spentTime.start(); // initialisation de la durée de vie
	_spentTimeTogether.start();

	//Init pathfinding singleton
	m_pPathfinder = Pathfinding::GetSingleton();

	//Init timer
	m_lastUpdate.start();

}

Gendamour::~Gendamour()
{

}


void Gendamour::Draw(){

	glColor3f(255, 0, 0);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glColor3f(1, 0, 1);
	glTranslatef(0, 0, 2);
	glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	glPopMatrix();
}

void Gendamour::UpdateIA(){


	//recherche de créature a suivre
	/*if (_target != NULL){
		for (int i = 0; i < CREATURE_NUM; ++i){
			eTypeCreature type = (eTypeCreature)i;
			for (int j = 0; j < (*m_entities)[type].size(); ++j){

				if ( (*m_entities)[type][j] )
					_target = (*m_entities)[type][j];
			}
		}
	}*/

	//si le gendamour n'est pas mort, il fait des trucs
	if (m_currentState != STATE_Dead){
		//le gendamour fait aussi des bébé
		if (_target != NULL && (*_target).type == GENDAMOUR && _spentTimeTogether.getElapsedSeconds() > _reproductionTime * 60){
			cout << "this the beginning of a new life" << endl;
			//crée un autre gendamour
			_spentTimeTogether.start();
		}
		//le gendamour meurt si il a dépassé sa durée de vie 
		if (_spentTime.getElapsedSeconds() > _lifeTime * 60 || life <= 0){
			cout << "this is the end" << endl;
			PushState(STATE_Dead);
		}
		//Le gendamour meurt si la créature qu'il suit meurt
		if (_target != NULL && (*_target).GetState() == STATE_Dead){
			cout << "gendamour's friend is dead, gendamour is now dead" << endl;
			PushState(STATE_Suicide);
		}
	}


	//Update Finite State Machine
	Update();

	//Start timer
	m_lastUpdate.start();

}


bool Gendamour::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine
	State(STATE_Initialize)
		OnEnter{
			PushState(STATE_FindPath);
		}
		OnUpdate{				
		}
	State(STATE_Move)
		OnEnter{
			if (m_path.GetSize() > 1)
			{
				m_targetPosition = m_path.GetWaypoint(1);
				direction = m_targetPosition - position;
				direction.normalize();
				m_pathIndex = 1;
			}
			else
			{
				PushState(STATE_FindPath);
			}
		}
		OnUpdate{
			position += direction * m_lastUpdate.getElapsedSeconds() * NYCube::CUBE_SIZE;

			NYVert3Df distance = m_targetPosition - position;
			if (distance.scalProd(direction) < 0.0f)
			{
				position = m_targetPosition;

				if (++m_pathIndex < m_path.GetSize())
				{
					m_targetPosition = m_path.GetWaypoint(m_pathIndex);
					direction = m_targetPosition - position;
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
	State(STATE_Sleep)
		OnEnter{
		}
		OnUpdate{
		}
	State(STATE_Eat)
		OnEnter{
		}
		OnUpdate{
		}
	State(STATE_Dead)
		OnEnter{
			cout << "Gendamour is dead !" << endl;
		}
		OnUpdate{

		}
	State(STATE_Egg)
		OnEnter{
		}
		OnUpdate{
		}
	State(STATE_FindPath)
		OnEnter{
			
		}
		OnUpdate{
			bool pathFound = false;
			NYVert3Df newPosition;

			if (_target != NULL){

				newPosition.X = _target->position.X;
				newPosition.Y = _target->position.Y;
				newPosition.Z = (int)m_world->_MatriceHeights[(int)newPosition.X][(int)newPosition.Y];

				if (newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
					newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
					newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
				{
					pathFound = m_pPathfinder->FindPathDahut(positionCube, newPosition, m_path);
				}
			}
			else{
				while (!pathFound)
				{
					newPosition.X = (int)(positionCube.X + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
					newPosition.Y = (int)(positionCube.Y + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
					newPosition.Z = (int)m_world->_MatriceHeights[(int)newPosition.X][(int)newPosition.Y];

					if (newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
						newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
						newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
					{
						pathFound = m_pPathfinder->FindPathDahut(positionCube, newPosition, m_path);
					}
				}
			}

			PushState(STATE_Move);
		}
	State(STATE_Suicide)
		OnEnter{
			cout << "I don't want to live on this planet anymore !" << endl;
			PushState(STATE_Dead);
		}
		OnUpdate{
		}
	State(STATE_Reproduction)
		OnEnter{
		}
		OnUpdate{
		}
	State(STATE_Attack)
		OnEnter{
		}
		OnUpdate{
		}
	State(STATE_Wait)

	EndStateMachine
}

void Gendamour::setTarget(IABase* target){
	_target = target;
	if (_target->type == GENDAMOUR)
		_spentTimeTogether.start();
}
