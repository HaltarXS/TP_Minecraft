/*
* Source file : Lemming AI class
* Author : DESHORS Axel
* Date : 16/05/2015
*/
#include "Lemming.h"

#define DEBUG		if (m_debug)

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

	// Pathfind init
	m_pathfind = Pathfinding::GetSingleton();

	// Position init
	IABase::position = _worldSpawnPos;
	IABase::positionCube = IABase::position / NYCube::CUBE_SIZE;
}

void Lemming::UpdateIA()
{
	Update(m_timer.getElapsedSeconds(true));

	// Update view
	m_view.SetPosition(position);
	m_view.SetOrientation(direction);

	

	GetVisibleCreatures();

	StateMachine::Update();	// Update state machine
}

void Lemming::Update(float _elapsedTime)
{
	m_tick += _elapsedTime;
	m_timeElapsed = _elapsedTime;

	if (m_tick >= 1.f)
	{
		// Increase the hungerpoints
		if (m_currentHungerPoints < m_maxHungerPoints)
			m_currentHungerPoints++;
		// Increase the Reproduction points
		if (m_currentReproductionPoints < m_maxReproductionPoints)
			m_currentReproductionPoints++;

		m_tick = 0;
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

void Lemming::GetTargetToFollow()
{
	// Select random creature sawed by the lemming
	int _index = rand() % m_visibleCreatures.size();

	m_target = m_visibleCreatures.at(_index);
}

bool Lemming::TargetIsVisible()
{
	// If the Lemming have a target
	if (m_target == NULL)
		return false;

	// Is this target visible by the lemming?
	return (m_view.IsInSight(m_target->position));
}

bool Lemming::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine

	OnMsg(MSG_Eat)
	{
		Eat();
		std::cout << "Lemming : eating : Hunger = " << m_currentHungerPoints << ", Lifepoint = " << m_currentLifePoints << std::endl;
	}


	// States : 
	// Initialization state :
	State(STATE_Initialize)
	OnEnter
	DEBUG
	std::cout << "Lemming init..." << std::endl;
	PushState(STATE_Move);	// Principal state

	// Random move state
	State(STATE_Move)
	OnEnter
	DEBUG
	std::cout << "Lemming : Moving..." << std::endl;
	if (m_path.GetSize() > 1)
	{
		IABase::direction = m_path.GetWaypoint(0) - IABase::position;
		IABase::direction.normalize();
		m_currentPathIndex = 0;
	}
	else
		PushState(STATE_FindPath);
	OnUpdate
	// Update the position with the direction path
	IABase::position += IABase::direction * m_walkSpeed * m_timeElapsed;
	IABase::positionCube = IABase::position / NYCube::CUBE_SIZE;

	NYVert3Df _dist = m_path.GetWaypoint(m_currentPathIndex) - IABase::position;

	if (_dist.getSize() <= 5.f)
		m_currentPathIndex++;

	// We are not at the end of the 
	if (m_currentPathIndex < m_path.GetSize())
	{
		IABase::direction = m_path.GetWaypoint(m_currentPathIndex) - IABase::position;
		IABase::direction.normalize();
	}
	else
	{
		m_path.Clear();	// Delete the current path
		PushState(STATE_FindPath);
	}

	OnExit
	DEBUG
	std::cout << "Stop moving" << std::endl;
	
	// Find a path
	State(STATE_FindPath)

	OnUpdate
	DEBUG
	std::cout << "Lemming Find Path" << std::endl;
	int _x = rand() % MAT_SIZE_CUBES;
	int _y = rand() % MAT_SIZE_CUBES;

	m_pathfind->FindPath(NYVert2Df(IABase::positionCube.X, IABase::positionCube.Y), NYVert2Df(_x, _y), 1, m_path);

	PushState(STATE_Move);
	
	// Stay STATE
	

	// DEAD STATE
	State(STATE_Dead)
	OnEnter
	DEBUG
	std::cout << "Lemming is dead" << std::endl;

	EndStateMachine
}

Lemming::~Lemming()
{
}
