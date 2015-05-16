#include "Gendamour.h"

Gendamour::Gendamour(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	
	type = GENDAMOUR;

	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];

	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;
}

Gendamour::~Gendamour()
{

}


void Gendamour::Draw(){

	glColor3f(255, 255, 0);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 2.0f);

	glPopMatrix();
}

void Gendamour::UpdateIA()
{
	//Update FSM
	Update();
}


bool Gendamour::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine
	EndStateMachine
}
