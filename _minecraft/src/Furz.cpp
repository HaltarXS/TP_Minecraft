#include "Furz.h"

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
}

Furz::~Furz()
{

}


void Furz::Draw(){

	
	glPushMatrix();

	glColor3f(0.49f, 0.40f, 0.14f);
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE * 1.5f);

	glPopMatrix();
}

void Furz::UpdateIA()
{
	//Furz vivant
	if (m_currentState != STATE_Dead){
		if (_fartCounter >= _numberFartSpawn){
			//On spawn un autre Furz
			//On reset le compteur de pet
		}
		if (_timeFartBomb.getElapsedSeconds() > _fartFrequency){
			//Bah on pete, Spawn de l'herbe
			cout << _fartCounter << endl;
			_timeFartBomb.start();
			_fartCounter++;
		}
		//le furz meurt si il a dépassé sa durée de vie 
		if (_spentTime.getElapsedSeconds() > _lifeTime * 60 || life <= 0){
			cout << "this is the end" << endl;
			PushState(STATE_Dead);
		}
	}

	//Update Finite State Machine
	Update();

}


bool Furz::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)

		OnEnter{
		//cout << " Initialisation of the state machine d'un furz" << endl;
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

