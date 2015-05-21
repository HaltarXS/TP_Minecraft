/*
* Source file : Lemming AI class
* Author : DESHORS Axel
* Date : 17/05/2015
*/
#include "Lemming.h"

#define DEBUG		if (m_printDebug)

int _called = 0;

Lemming::Lemming(NYWorld * _world, NYVert2Df _spawnPos)
	:IABase(_world), m_view(100.f, 65.f)
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
	IABase::life = m_maxLifePoints;
	m_currentReproductionPoints = 0;

	m_checkVisibility = 0.f;

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

	StateMachine::Update();	// Update state machine
}

void Lemming::Update(float _elapsedTime)
{
	m_tick += _elapsedTime;
	m_timeElapsed = _elapsedTime;

	if (m_currentState != STATE_Dead)
	{
		if (m_tick >= 1.f)
		{
			// Increase the hungerpoints
			if (m_currentHungerPoints < m_maxHungerPoints)
				m_currentHungerPoints++;

			if (m_currentHungerPoints == m_maxHungerPoints)
			{
				std::cout << "DEATH BY HUNGER!!" << std::endl;

				PushState(STATE_Dead);
			}
			// Increase the Reproduction points
			if (m_currentReproductionPoints < m_maxReproductionPoints)
				m_currentReproductionPoints++;

			m_tick = 0;
		}

		if (m_currentState == STATE_Move)
		{
			m_checkVisibility += m_timeElapsed;

			if (m_checkVisibility >= m_checkVisibilityTime)
				m_checkVisibility = 0;
		}

		if (m_inactiveTimer >= 0.f)
			m_inactiveTimer -= m_timeElapsed;

		if (m_currentState == STATE_Dance)
			m_dancingTime -= m_timeElapsed;
	}

}

void Lemming::Born()
{
	DEBUG
	std::cout << "New lemming is Born!!" << std::endl;

	m_currentHungerPoints = 0;
	IABase::life = m_maxLifePoints;
	m_currentReproductionPoints = 0;
}

void Lemming::SetEntities(std::map<eTypeCreature, std::vector<IABase *>> * _entities)
{
	m_entities = _entities;
}

void Lemming::GetVisibleCreatures()
{
	m_visibleCreatures.clear();
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;

		if (type != eTypeCreature::YETI && 
			type != eTypeCreature::GRIFFONKITU && 
			type != eTypeCreature::MOUCHE &&
			type != eTypeCreature::PARASITE &&
			type != eTypeCreature::VAUTOUR &&
			type != eTypeCreature::BIXI)
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
	if (m_drawDebug)
	{
		std::cout << "Eat some : ";
		switch (m_world->getCube((int)IABase::positionCube.X, (int)IABase::positionCube.Y, m_world->_MatriceHeights[(int)IABase::positionCube.X][(int)IABase::positionCube.Y] - 1)->_Type)
		{
		case NYCubeType::CUBE_EAU: std::cout << "water"; break;
		case NYCubeType::CUBE_AIR: std::cout << "wind"; break;
		case NYCubeType::CUBE_NEIGE: std::cout << "snow"; break;	// Suppose to be snow but for the debug we test
		case NYCubeType::CUBE_HERBE: std::cout << "grass"; break;
		case NYCubeType::CUBE_TERRE: std::cout << "earth"; break;
		}
		std::cout << std::endl;
	}

	m_currentHungerPoints -= 100;
	IABase::life += 10;
	m_currentHungerPoints = max(m_currentHungerPoints, 0);
	IABase::life = min(IABase::life, m_maxLifePoints);
}

bool Lemming::CanReproduce()
{
	return (m_currentReproductionPoints == m_maxReproductionPoints);
}

void Lemming::Reproduce()
{
	m_currentReproductionPoints = 0;

	IABase::life += 50;
	IABase::life = min(m_maxLifePoints, IABase::life);

	m_currentHungerPoints = 0;
}

bool Lemming::IsHungry()
{
	return (m_currentHungerPoints >= m_maxHungerPoints / 2.f);
}

void Lemming::Draw()
{
	glPushMatrix();

	// Draw the lemming
	if (m_currentState != STATE_Follow)
		glTranslatef(position.X, position.Y, position.Z);
	else
		glTranslatef(position.X, position.Y, position.Z + 5);

	if (IsDancing())
		glColor3f(1.f, 1.f, 0.9f);
	else if (m_currentState == STATE_Reproduction)
		glColor3f(0.f, 0.f, 0.f);
	else if (m_currentState == STATE_Follow)
		glColor3f(1.0f, 0.2f, 0.2f);
	else if (m_currentState == STATE_Dead)
		glColor3f(0.f, 0.f, 1.f);
	else
		glColor3f(0.f, 0.4f, 0.9f);


	// Corpse
	glutSolidCube(7);
	glPushMatrix();
	glTranslatef(0, 0, 5);
	glColor3f(0.1f, 0.9f, 0.1f);
	glutSolidCube(5);

	// Pop matrices
	glPopMatrix();
	glPopMatrix();

	if (m_drawDebug)
	{
		m_view.DebugDraw();
		m_path.DrawPath();

		if (m_currentState == STATE_Follow)
		{
			glLineWidth(10.f);

			glBegin(GL_LINES);

			glVertex3f(position.X, position.Y, position.Z);
			glVertex3f(m_followedTarget->position.X, m_followedTarget->position.Y, m_followedTarget->position.Z);

			glEnd();

			glLineWidth(1);
		}
		else if (m_currentState == STATE_Reproduction)
		{
			glLineWidth(10.f);

			glBegin(GL_LINES);

			glVertex3f(position.X, position.Y, position.Z);
			glVertex3f(m_partner->position.X, m_partner->position.Y, m_partner->position.Z);

			glEnd();

			glLineWidth(1);
		}
	}
}

bool Lemming::FindPath(int _x, int _y)
{
	_called++;

	float _elapsedTime = 0.f;

	m_debugTimer.start();

	_y = min(max(1, _y), MAT_SIZE_CUBES - 1);
	_x = min(max(1, _x), MAT_SIZE_CUBES - 1);
	
	m_path.Clear();	// Clear the path

	// If the cube is not water
	if (m_world->getCube(_x, _y, m_world->_MatriceHeights[_x][_y] - 1)->_Type == NYCubeType::CUBE_EAU)	
		return false;	// Not find the path (algorithm too long when the cube is water) Optimization

	m_currentPathIndex = 0;

	IABase::positionCube.X = (int)min(max(1, IABase::positionCube.X), MAT_SIZE_CUBES - 1);
	IABase::positionCube.Y = (int)min(max(1, IABase::positionCube.Y), MAT_SIZE_CUBES - 1);

	// Pathfinding research
	m_pathfind->FindPath(NYVert2Df(IABase::positionCube.X, IABase::positionCube.Y), NYVert2Df(_x, _y), 1, m_path);

	_elapsedTime = m_debugTimer.getElapsedSeconds();

	if (_elapsedTime > 0.01f && m_printDebug)
	{
		std::cout << "PATHFINDING num" << _called << ": (" << positionCube.X << ", " << positionCube.Y << ") TO : (" << _x << ", " << _y << ")" << " TAKE (SEC) : " << _elapsedTime << std::endl;
		std::cout << "TYPE CUBE IS : ";

		switch (m_world->getCube(_x, _y, m_world->_MatriceHeights[_x][_y] - 1)->_Type)
		{
		case NYCubeType::CUBE_EAU: std::cout << "WATER"; break;
		case NYCubeType::CUBE_AIR: std::cout << "WIND"; break;
		case NYCubeType::CUBE_NEIGE: std::cout << "SNOW"; break;
		case NYCubeType::CUBE_HERBE: std::cout << "GRASS"; break;
		case NYCubeType::CUBE_TERRE: std::cout << "EARTH"; break;
		}

		std::cout << std::endl;
	}

	return true;
}

void Lemming::GetTargetToFollow()
{
	// Select random creature sawed by the lemming
	int _index = rand() % m_visibleCreatures.size();

	m_followedTarget = m_visibleCreatures.at(_index);
}

bool Lemming::TargetIsVisible()
{
	// If the Lemming have a target
	if (m_followedTarget == NULL)
		return false;

	// Is this target visible by the lemming?
	return (m_view.IsInSight(m_followedTarget->position));
}

bool Lemming::IsDancing()
{
	return (m_currentState == STATE_Dance);
}

bool Lemming::UpdatePosition()
{
	// Update the position
	if (m_currentPathIndex < m_path.GetSize())
	{
		// Update the position with the direction path
		IABase::position += IABase::direction * m_walkSpeed * m_timeElapsed;
		IABase::positionCube = IABase::position / NYCube::CUBE_SIZE;

		IABase::direction = m_path.GetWaypoint(m_currentPathIndex) - IABase::position;
		IABase::direction.normalize();

		NYVert3Df _dist = m_path.GetWaypoint(m_currentPathIndex) - IABase::position;

		if (_dist.getSize() <= 5.f)
			m_currentPathIndex++;

	}

	return (m_currentPathIndex < m_path.GetSize());
}

bool Lemming::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine

	OnMsg(MSG_Eat)
	{

	}

	OnMsg(MSG_Reproduce)
	{
		if (IsDancing())
		{
			DEBUG
			std::cout << "Reproducing!!!" << std::endl;
						// Update the variables
			Reproduce();

			m_dancingTime = 0.f;
			PushState(STATE_Move);
		}
	}

	OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		this->life -= *data;//I remove the value of the message data from my life.
		std::cout << "--Entity " << this->GetID() << "-- Attack from entity " << msg->GetSender() << ". Life removed : " << *data << ". Life remaining : " << this->life << std::endl;
		delete data;//Delete the data

		if (this->life <= 0)//If i don't have any life
		{
			PushState(STATE_Dead);//Use PushState to go in an other state
		}
	}//Message Attack


	// States : 
	// Initialization state :
	State(STATE_Initialize)
	OnEnter
	PushState(STATE_Move);	// Principal state

	// Random move state
	State(STATE_Move)
	OnEnter
	if (m_path.GetSize() > 1)
	{
		IABase::direction = m_path.GetWaypoint(0) - IABase::position;
		IABase::direction.normalize();
		m_currentPathIndex = 0;
	}
	else
		PushState(STATE_FindPath);
	OnUpdate
		// If the lemming is hungry he take some snow at his feet
	if (IsHungry())
	{
		NYCube * _cube = m_world->getCube(IABase::positionCube.X, IABase::positionCube.Y, IABase::positionCube.Z - 1);

		if (_cube != NULL && _cube->_Type == CUBE_NEIGE)
			Eat();
	}
	
	// If the lemming have to check the visibility (Optimization)
	if (m_checkVisibility == 0.f)
	{
		// Update the visible creatures
		GetVisibleCreatures();
		// Lemming see other creatures
		if (m_visibleCreatures.size() > 0 && m_inactiveTimer <= 0.f)
		{
			DEBUG
			std::cout << "Lemming see : " << m_visibleCreatures.size() << " creatures" << std::endl;

			// If the lemming can reproduce
			if (CanReproduce())
			{
				for each (IABase * _monster in m_visibleCreatures)
				{
					bool _dance = false;
					// If see another lemming Lemming :
					if (_monster->type == LEMMING)
					{
						if (((Lemming *)_monster)->IsDancing())
						{
							DEBUG
							std::cout << "Find a partner dancing" << std::endl;
							
							m_partner = ((Lemming *)_monster);
							//m_destination = NYVert2Df(_monster->positionCube.X, _monster->positionCube.Y);	// Get the destination in case
							PushState(STATE_Reproduction);	// Repdroduce with the other lemming
							return false;	// Exit the function
						}
						else
						{
							_dance = true;
							DEBUG
							std::cout << "Find a partner who don't dance" << std::endl;
						}
						if (_dance)
						{
							PushState(STATE_Dance);	// Dance for the reporduction!!!
							return false;
						}
					}
				}
			}
			// Else the lemming follow a random creature
			int _rand = rand() % m_visibleCreatures.size();
			m_followedTarget = m_visibleCreatures.at(_rand);

			DEBUG
			std::cout << "Follow : : " << _rand << " index" << std::endl;

			PushState(STATE_Follow);
		}
	}

	// Update the position
	if (!UpdatePosition())
	{
		m_path.Clear();	// Delete the current path
		PushState(STATE_FindPath);
	}

	OnExit

	// Find path for a random point on the map
	State(STATE_FindPath)
	OnUpdate
	FindPath(IABase::positionCube.X + (10 - rand() % 20), IABase::positionCube.Y + (10 - rand() % 20));
	PushState(STATE_Move);

	// Reproduction for the lemming
	State(STATE_Reproduction)
	OnEnter
	DEBUG
	std::cout << "Lemming is reproducing with other" << std::endl;

	int _x = m_partner->positionCube.X;
	int _y = m_partner->positionCube.Y;

	// Get the path between the lemming and the target
	FindPath(_x, _y);

	if (m_path.GetSize() > 1)
	{
		IABase::direction = m_path.GetWaypoint(0) - IABase::position;
		IABase::direction.normalize();
		m_currentPathIndex = 0;
	}
	else
	{
		PushState(STATE_Move);
	}
	OnUpdate
	// We are at the end of the path
	if (!UpdatePosition())
	{
		m_path.Clear();
		NYVert3Df _distance = IABase::position - m_partner->position;

		if (_distance.getSize() < 15.f && m_partner != NULL && m_partner->IsDancing())	// The partner still exist and already dancing
		{
			m_partner->SendMsg(MSG_Reproduce, m_partner->GetID());
			Reproduce();

			// Add a little lemming
			Lemming * l = new Lemming(m_world, NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE));
			l->SetEntities(m_entities);
			l->Born();
			l->m_printDebug = m_printDebug;
			l->m_drawDebug = m_drawDebug;
			(*m_entities)[LEMMING].push_back(l);
		}
		PushState(STATE_Move);
	}

	// Dance for the reproduction!
	State(STATE_Dance)
	OnEnter
	DEBUG
	std::cout << "Lemming is dancing..." << std::endl;
	m_dancingTime = m_danceTime;
	OnUpdate
	if (m_dancingTime <= 0.f)
	{
		m_dancingTime = 0.f;
		PushState(STATE_Move);
	}
	OnExit
	m_path.Clear();
	m_currentPathIndex = 0;

	DEBUG
	std::cout << "Exit dance" << std::endl;

	// Follow the creature
	State(STATE_Follow)
	OnEnter
	DEBUG
	std::cout << "Lemming follow target" << std::endl;
	
	m_path.Clear();
	m_destination = m_followedTarget->position;
	//FindPath(_x, _y);

	IABase::direction = m_destination - IABase::position;
	IABase::direction.normalize();
	//m_currentPathIndex = 0;

	OnUpdate
	// If the creature still on sight :
	if (m_followedTarget != NULL && m_view.IsInSight(m_followedTarget->position))
	{
		// The followed target doesn't move
		if (m_destination == m_followedTarget->position)
		{
			NYVert3Df _dist = m_destination - IABase::position;

			// If the lemming is on the target (who don't move)
			if (_dist.getSize() <= 10.f)
			{
				PushState(STATE_Move);

				return false;
			}
		}

		// The lemming is following another lemming
		if (m_followedTarget->type == LEMMING)
		{
			if (CanReproduce() && ((Lemming *)m_followedTarget)->IsDancing())
			{
				m_path.Clear();
				PushState(STATE_Move);	// Go to move to detect the dancing lemming
			}
		}

		m_destination = m_followedTarget->position;
	}
	else
	{
		PushState(STATE_Move);
		return false;
	}

	// End of the path
	// Update the position with the direction path
	IABase::position += IABase::direction * m_walkSpeed * m_timeElapsed;
	IABase::positionCube = IABase::position / NYCube::CUBE_SIZE;


	if (IABase::positionCube.X >= 0 && IABase::positionCube.X < MAT_SIZE_CUBES && 
		IABase::positionCube.Y >= 0 && IABase::positionCube.Y < MAT_SIZE_CUBES)
	{
		IABase::position.Z = m_world->_MatriceHeights[(int)IABase::positionCube.X][(int)IABase::positionCube.Y] * NYCube::CUBE_SIZE;
		IABase::positionCube.Z = IABase::position.Z / NYCube::CUBE_SIZE;
	}

	IABase::direction = m_destination - IABase::position;
	IABase::direction.normalize();


	State(STATE_Dead)
	OnUpdate
	// Nothing is dead!

	EndStateMachine
}

Lemming::~Lemming()
{
}
