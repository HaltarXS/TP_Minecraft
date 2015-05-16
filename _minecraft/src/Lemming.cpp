/*
* Source file : Lemming AI class
* Author : DESHORS Axel
* Date : 16/05/2015
*/

#include "Lemming.h"


Lemming::Lemming(NYWorld * _world, NYVert2Df _spawnPos)
	:IABase(_world), m_view(100.0f, 150)
{
	// Get the spawn position :
	NYVert3Df _worldSpawnPos = NYVert3Df(_spawnPos.X * NYCube::CUBE_SIZE,
		_spawnPos.Y * NYCube::CUBE_SIZE,
		(_world->_MatriceHeights[(int)_spawnPos.X][(int)_spawnPos.Y] + 1) * NYCube::CUBE_SIZE);

	// State machine initialization
	IABase::Initialize();
	IABase::type = LEMMING;

	// Gauge initialization
	m_currentHungerPoints = 0;
	m_currentLifePoints = m_maxLifePoints;
	m_currentReproductionPoints = 0;

	// Timer initialization
	m_timer.start();

	// Position init
	IABase::position = _worldSpawnPos;
}

void Lemming::UpdateIA()
{
	// Update view
	m_view.SetPosition(position);
	m_view.SetOrientation(direction);

	Update(m_timer.getElapsedMs(true));

	GetVisibleCreatures();

	StateMachine::Update();	// Update state machine
}

void Lemming::Update(float elapsed_time)
{
	m_timeElapsed += elapsed_time;

	if (m_timeElapsed >= 1.f)
	{
		// Increase the hungerpoints
		if (m_currentHungerPoints < m_maxHungerPoints)
			m_currentHungerPoints++;
		// Increase the Reproduction points
		if (m_currentReproductionPoints < m_maxReproductionPoints)
			m_currentReproductionPoints++;

		m_timeElapsed = 0;
	}

	if (m_currentState == STATE_Move)
	{

	}
}

void Lemming::GetVisibleCreatures()
{
	m_visibleCreatures.clear();
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;

		//if (type != eTypeCreature::YETI)
		{
			for (int j = 0; j < (*m_entities)[type].size(); ++j)
			{
				if (m_view.IsInSight((*m_entities)[type][j]->position) && (*m_entities)[type][j]->GetID() != this->GetID())
				{
					m_visibleCreatures.push_back((*m_entities)[type][j]);
				}
			}
		}
	}
}

void Lemming::Eat()
{
	m_currentHungerPoints -= 100;
	m_currentLifePoints += 10;
	m_currentHungerPoints = max(m_currentHungerPoints, 0);
	m_currentLifePoints = min(m_currentLifePoints, m_maxLifePoints);
}

bool Lemming::IsHungry()
{
	return (m_currentHungerPoints >= 100);
}

void Lemming::Draw()
{
	glPushMatrix();

	// Draw the lemming
	glTranslatef(position.X, position.Y, position.Z);

	glColor3f(0.f, 0.2f, 0.9f);
	glutSolidCube(7);

	glPopMatrix();

	m_view.DebugDraw();
	m_path.DrawPath();
}

bool Lemming::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	/*BeginStateMachine

		OnMsg(MSG_Eat)
	{
		Eat();

		std::cout << "Lemming : eating : Hunger = " << m_currentHungerPoints << ", Lifepoint = " << m_currentLifePoints << std::endl;
	}


	// States : 
	// Initialization state :
	State(STATE_Initialize)
		OnEnter
		std::cout << "Lemming init..." << std::endl;
	PushState(STATE_Move);

	// Random move state
	State(STATE_Move)
		OnEnter




		EndStateMachine*/

		return false;
}

Lemming::~Lemming()
{
}
