#include "Wastedosaure.h"


Wastedosaure::Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale):
IABase(_world)
{
	Initialize();
	type = WASTEDOSAURE;
	position = NYVert3Df(_positionInitiale.X * NYCube::CUBE_SIZE, _positionInitiale.Y*NYCube::CUBE_SIZE, _world->_MatriceHeights[(int)_positionInitiale.X][(int)_positionInitiale.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);
}

Wastedosaure::~Wastedosaure()
{

}

void Wastedosaure::UpdateIA()
{
	Update();//Update the state machine


}

void Wastedosaure::Draw()
{
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	if (m_currentState == STATE_Egg)
	{
		glutSolidSphere(5, 20, 20);
	}
	else
	{
		glutSolidCone(5, 5, 20, 20);
	}

	glPopMatrix();
}


bool Wastedosaure::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine

	State(STATE_Initialize)
	OnEnter
	PushState(STATE_Egg);


	//Move
	State(STATE_Egg)
	OnEnter
		
	OnUpdate
		
	OnExit


	//Dead
	State(STATE_Dead)
	
	OnEnter


	EndStateMachine
}