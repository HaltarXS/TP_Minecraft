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

	_spentTime.start();
	_spentTimeTogether.start();
}

Gendamour::~Gendamour()
{

}


void Gendamour::Draw(){

	glColor3f(255, 0, 0);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glPopMatrix();
}

void Gendamour::UpdateIA()
{
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
			PushState(STATE_Dead);
		}
	}

	//Update Finite State Machine
	Update();

}


bool Gendamour::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)

	OnEnter{
		//cout << " Initialisation of the state machine d'un gendamour" << endl;
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
		cout << "Gendamour is dead !" << endl;
	}
	OnUpdate{
	}

	EndStateMachine
}

void Gendamour::setTarget(IABase* target){
	_target = target;
}
