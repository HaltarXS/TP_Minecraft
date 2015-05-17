#include "Parasite.h"


Parasite::Parasite(NYWorld *pWorld, NYVert2Df pos, bool firstBorn) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = PARASITE;

	//Détermine si le virus vient d'une crotte
	m_isFirstBorn = firstBorn;

	//Init position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 5.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 5.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;

	//Init timer
	m_lastUpdate.start();
}


Parasite::~Parasite()
{

}

void Parasite::UpdateIA()
{
	//Update FSM
	Update();

	//Start timer
	m_lastUpdate.start();
}

void Parasite::Draw()
{
	glColor3f(0, 255, 0);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 10.0f);
	glPopMatrix();
}

bool Parasite::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

		State(STATE_Initialize)
		OnEnter
	{
		//Si le parasite a germé sur une crotte, il est infectieux directement
		if (m_isFirstBorn) {
			PushState(STATE_Reproduction);
		}
		else {//Sinon, il a une phase d'incubation
			PushState(STATE_Sleep);
		}
	}

		State(STATE_Sleep)
		OnEnter
	{
		m_timeSleep = 0.0f;
	}
		OnUpdate
	{
		if (m_timeSleep >= m_durationSleep)
		{
			PushState(STATE_Reproduction);
		}
		m_timeSleep += m_lastUpdate.getElapsedSeconds();
	}

		State(STATE_Reproduction)
		OnEnter
	{
		m_timeReproduction = 0.0f;
	}

		OnUpdate
	{
		//ProximityChecker::GetSingleton()->checkParasiteProximityCreatures(this, m_areaEffect);
		InfectCreaturesInArea(m_areaEffect);
		if (m_timeReproduction >= m_durationReproduction)
		{
			PushState(STATE_Dead);
		}
		m_timeReproduction += m_lastUpdate.getElapsedSeconds();
	}
		State(STATE_Dead)
		OnEnter
	{
		cout << "One parasite is dead, but the war is not over. His children will infect you all\n";
	}
		EndStateMachine
}

void Parasite::InfectCreaturesInArea(float sizeArea) {
	//m_creaturesNear = ProximityChecker::GetSingleton()->checkParasiteProximityCreatures(this, sizeArea);
	for (int i = 0; i < m_creaturesNear.size(); ++i)
	{
		m_creaturesNear[i]->infected = true;

	}
}
