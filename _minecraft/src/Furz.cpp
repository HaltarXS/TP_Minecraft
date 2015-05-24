#include "Furz.h"
#include "RessourcesManager.h"
#include <stdlib.h>

Furz::Furz(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	type = FURZ;

	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];

	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;

	_timeFartBomb.start();
	_spentTime.start();

	nextPosition = position;
	
}

Furz::~Furz()
{

}

void  Furz::setPosition(int posCubeX, int posCubeY, int posCubeZ){

	nextPosition.X = posCubeX*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	nextPosition.Y = posCubeY*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	nextPosition.Z = posCubeZ*NYCube::CUBE_SIZE;

}

void  Furz::updatePosition(float elapsedTime){

	positionCube.X = (int)(position.X / NYCube::CUBE_SIZE);
	positionCube.Y = (int)(position.Y / NYCube::CUBE_SIZE);
	positionCube.Z = (int)m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y];
	
	position.X += (nextPosition.X - position.X)  * elapsedTime *2;
	position.Y += (nextPosition.Y - position.Y)  * elapsedTime *2;
	position.Z += ((m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y] * NYCube::CUBE_SIZE) - position.Z) * 4 * elapsedTime;

}

void Furz::Draw(){

	
	glPushMatrix();

	glColor3f(0.49f, 0.40f, 0.14f);
	glTranslatef(position.X, position.Y, position.Z + (NYCube::CUBE_SIZE * _size)/2);
	glutSolidCube(NYCube::CUBE_SIZE * _size);

	glPopMatrix();
}

void Furz::UpdateIA()
{
	updatePosition(_deltaTime.getElapsedSeconds());

	//cout << positionCube.X << " || " << positionCube.Y << " || " << positionCube.Z << endl;
	//cout << position.X << " || " << position.Y << " || " << position.Z << endl;

	//Furz vivant
	if (m_currentState != STATE_Dead){
		
		//Quand le compteur de pet arrive au nombre de pet necessaire pour faire apparaitre un nouveau furz
		if (_fartCounter >= _numberFartSpawn){
			NYVert2Df* pos = new NYVert2Df(positionCube.X, positionCube.Y);
			Furz* f = new Furz(m_world, *pos);
			f->m_entities = m_entities;
			(*m_entities)[FURZ].push_back(f);
			_numberFartSpawn = 20 + rand() % 20;//Reset du nombre de pet necessaire pour faire spawn
		}

		//On Commence a grossir
		if (_timeFartBomb.getElapsedSeconds() > _fartFrequency - 2){
			_size += _deltaTime.getElapsedSeconds()*((_inflateSize - 1) / 2);
		}
		else if(_size > 1){
			_size =  max(1, _size - _deltaTime.getElapsedSeconds()*2 );
		}

		//On pete
		if (_timeFartBomb.getElapsedSeconds() > _fartFrequency){
			RessourcesManager *pRessourceMgr = RessourcesManager::GetSingleton();
			//fait pop un bloc d'herbe sur la poscube de l'entité
			*pRessourceMgr->Create(HERBE, NYVert3Df(positionCube.X * NYCube::CUBE_SIZE, positionCube.Y *  NYCube::CUBE_SIZE, positionCube.Z *  NYCube::CUBE_SIZE), 1000);
							
			_inflateSize = (14.0 + rand() % 7)/10;//Random de la taille de gonflement
			if (_inflateSize < 1.6){
				_distanceDeplacement = 1;
			}
			else if (_inflateSize >= 1.6 && _inflateSize < 1.8){
				_distanceDeplacement = 2;
			}
			else if (_inflateSize >= 1.8 && _inflateSize <= 2){
				_distanceDeplacement = 3;
			}
			int _direction = rand() % 4;
			if ((int)(positionCube.X - _distanceDeplacement >= 0 && (int)(positionCube.Y - _distanceDeplacement) >= 0)){
				if (_direction == 0){
					setPosition((int)(positionCube.X + _distanceDeplacement), (int)positionCube.Y, (int)m_world->_MatriceHeights[(int)positionCube.X + _distanceDeplacement][(int)positionCube.Y]);
				}
				else if (_direction == 1){
					setPosition((int)(positionCube.X - _distanceDeplacement), (int)positionCube.Y, (int)m_world->_MatriceHeights[(int)positionCube.X - _distanceDeplacement][(int)positionCube.Y]);
				}
				else if (_direction == 2){
					setPosition((int)positionCube.X, (int)(positionCube.Y + _distanceDeplacement), (int)m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y + _distanceDeplacement]);
				}
				else if (_direction == 3){
					setPosition((int)positionCube.X, (int)(positionCube.Y - _distanceDeplacement), (int)m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y - _distanceDeplacement]);
				}
			}
		
			_fartFrequency = 3 + rand() % 3;//Reset du temps avant le prochain pet
			_timeFartBomb.start();//On reset le temps depuis le dernier pet
			_fartCounter++;//On incremente le compteur de pet
		}
		//le furz meurt si il a dépassé sa durée de vie 
		if (_spentTime.getElapsedSeconds() > _lifeTime * 60 || life <= 0){
			PushState(STATE_Dead);
		}

		/*
		//On cherche les Gendamour a portee
		for (int i = 0; i < (*m_entities)[GENDAMOUR].size(); i++)
		{
			float distance = ((*m_entities)[GENDAMOUR][i]->positionCube - this->positionCube).getMagnitude();
			
			if (distance < NYCube::CUBE_SIZE * 5){
				m_targetPosition = &(*m_entities)[GENDAMOUR][i]->positionCube;
			}
			
		}*/
	}
	_deltaTime.start();
	//Update Finite State Machine
	Update();

}


bool Furz::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)

		OnEnter{
	}

	State(STATE_FindPath)

	OnEnter{
	}
	OnUpdate{
	}

	State(STATE_Move)

	OnEnter{
	}
	OnUpdate{

	}
	State(STATE_Dead)

	OnEnter{
		cout << "Furz is dead !" << endl;
	}
		OnUpdate{
	}

	EndStateMachine
}

