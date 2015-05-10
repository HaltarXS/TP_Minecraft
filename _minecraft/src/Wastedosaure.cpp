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

bool Wastedosaure::HasAPath()
{
	return m_path.GetSize() > 0;
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
	m_path.DrawPath();
}


bool Wastedosaure::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine

	//Initialize
	State(STATE_Initialize)
	OnEnter
	PushState(STATE_Egg);


	//Egg
	State(STATE_Egg)
	OnEnter
	m_timerEgg = 0.0f;
	OnUpdate

	if (m_timerEgg >= m_timeEgg)
	{
		PushState(STATE_FindPath);
	}
	m_timerEgg += NYRenderer::_DeltaTime;
	OnExit

		
	State(STATE_FindPath)
	OnEnter
	m_path.Clear();
	m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), NYVert2Df(rand() % MAT_SIZE_CUBES, rand() % MAT_SIZE_CUBES), 1, m_path);
	cout << position.X << "," << position.Y << "," << position.Z << endl;
	m_path.PrintPath();
	m_currentIndex = 0;
	OnUpdate
	if (HasAPath())
	{
		PushState(STATE_Move);
	}
	OnExit

	//Move
	State(STATE_Move)
	OnEnter
	
	OnUpdate
	if (m_currentIndex < m_path.GetSize())
	{
		//On récupère la direction
		NYVert3Df currentDirection = m_path.GetWaypoint(m_currentIndex) - position;
		float lenght = currentDirection.getSize();
		currentDirection.normalize();

		if (lenght < 1.0f)
		{
			++m_currentIndex;
		}
		else
		{
			position += currentDirection * m_speed;
		}
	}
	else
	{
		PushState(STATE_FindPath);
	}

	OnExit
	


	//Dead
	State(STATE_Dead)
	
	OnEnter


	EndStateMachine
}