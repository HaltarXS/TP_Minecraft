#include "Gendamour.h"


Gendamour::Gendamour(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	type = GENDAMOUR;

	life = 250;

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
	_researhTime.start();

}

Gendamour::~Gendamour()
{

}


void Gendamour::Draw(){

	glColor3f(255, 0, 0);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	if (m_currentState != STATE_Dead)
		glColor3f(1, 0, 1);
	else
		glColor3f(0, 0, 0);

	glTranslatef(0, 0, 2);
	glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	glPopMatrix();


	//dessine une droit entre le gendamour et sa cible
	/*if (_target != NULL){
		glLineWidth(2.5);
		glColor3f(0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(position.X, position.Y, position.Z);
		glVertex3f((*_target).position.X, (*_target).position.Y, (*_target).position.Z);
		glLineWidth(1);
		glEnd();
	}*/

}

void Gendamour::UpdateIA(){




	//si le gendamour n'est pas mort, il fait des trucs
	if (m_currentState != STATE_Dead){

		//le gendamour fait aussi des bébé
		if (_target != NULL && (*_target).type == GENDAMOUR && _spentTimeTogether.getElapsedSeconds() > _reproductionTime * 60 ){
			//cout << "this the beginning of a new life" << endl;
			NYVert2Df* pos = new NYVert2Df(positionCube.X, positionCube.Y );
			Gendamour* g = new Gendamour(m_world, *pos);
			g->m_entities = m_entities;
			(*m_entities)[GENDAMOUR].push_back(g);
			_spentTimeTogether.start();
		}
		//le gendamour meurt si il a dépassé sa durée de vie 
		if (_spentTime.getElapsedSeconds() > _lifeTime * 60 ){
			//cout << "this is the end" << endl;
			PushState(STATE_Dead);
		}
		//Le gendamour meurt si la créature qu'il suit meurt
		if (_target != NULL && (*_target).GetState() == STATE_Dead){
			//cout << "gendamour's friend is dead, gendamour is now dead" << endl;
			PushState(STATE_Suicide);
		}
		//ci la cilbe va trop loin on lui bye bye
		if (_target != NULL){
			if (_researhTime.getElapsedSeconds() > 2){
				if((_target->position - position).getMagnitude() > 2000 ){
					_target = NULL;
				}
				else 				
					_researhTime.start();
			}
		
		}
	}

	//Update Finite State Machine
	Update();

	//Start timer
	m_lastUpdate.start();

	//recherche de créature a suivre
	if (m_currentState != STATE_Dead && _target == NULL && _researhTime.getElapsedSeconds() >1){
		_researhTime.start();
		for (int i = 0; i < CREATURE_NUM; ++i){
			eTypeCreature type = (eTypeCreature)i;
			for (int j = 0; j < (*m_entities)[type].size(); ++j){
				if ( (*m_entities)[type][j]->GetState() != STATE_Dead && (*m_entities)[type][j] != this && ((*m_entities)[type][j]->position - position).getMagnitude() < 1000){
					_target = (*m_entities)[type][j];
					//cout << " found someone to follow" << endl;
					_researhTime.start();
					PushState(STATE_FindPath);
					return;
				}
			}
		}
	}
}


bool Gendamour::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine


		OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		this->life -= *data;//I remove the value of the message data from my life.
		std::cout << "--Gendamour " << this->GetID() << "-- Attack from entity " << msg->GetSender() << ". Life removed : " << *data << ". Life remaining : " << this->life << std::endl;
		delete data;//Delete the data

		if (this->life <= 0)//If i don't have any life
		{
			PushState(STATE_Dead);//Use PushState to go in an other state
		}
	}//Message Attack

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
			cout << "Gendamour " << GetID() <<" is dead !" << endl;
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
					if ((_target->position - position).getMagnitude() > 500){

						newPosition.X = _target->positionCube.X;
						newPosition.Y = _target->positionCube.Y;
						newPosition.Z = (int)m_world->_MatriceHeights[(int)newPosition.X][(int)newPosition.Y];

						if (newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
							newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
							newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
						{
							NYVert2Df* pos = new NYVert2Df(positionCube.X, positionCube.Y);
							NYVert2Df* newPos = new NYVert2Df(newPosition.X, newPosition.Y);
							pathFound = m_pPathfinder->FindPath(*pos, *newPos, 2, m_path, false);
							PushState(STATE_Move);

						}
					}
				}
				else{

					newPosition.X = (int)(positionCube.X + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
					newPosition.Y = (int)(positionCube.Y + rand() / ((float)RAND_MAX) * 50.0f - 25.0f);
					newPosition.Z = (int)m_world->_MatriceHeights[(int)newPosition.X][(int)newPosition.Y];

					if (newPosition.X > 1 && newPosition.X < MAT_SIZE_CUBES - 1 &&
						newPosition.Y > 1 && newPosition.Y < MAT_SIZE_CUBES - 1 &&
						newPosition.Z > 1 && newPosition.Z < MAT_SIZE_CUBES - 1)
					{
						NYVert2Df* pos = new NYVert2Df(positionCube.X, positionCube.Y);
						NYVert2Df* newPos = new NYVert2Df(newPosition.X, newPosition.Y);
						pathFound = m_pPathfinder->FindPath(*pos, *newPos, 2, m_path, false);

						PushState(STATE_Move);

					}
				}
			
		}
	State(STATE_Suicide)
		OnEnter{
			//cout << "I don't want to live on this planet anymore !" << endl;
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
