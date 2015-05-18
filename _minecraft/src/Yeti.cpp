#include "Yeti.h"

Yeti::Yeti(NYWorld * _world, NYVert2Df _positionInitiale):
IABase(_world), m_cone(m_viewAngle, m_viewDistance)
{
	Initialize();
	type = YETI;

	NYVert2Df arrival = NYVert2Df(rand_a_b(_positionInitiale.X - 40, _positionInitiale.X  + 40), rand_a_b(_positionInitiale.Y  - 40, _positionInitiale.Y  + 40));
	int zStart = m_world->_MatriceHeights[(int)arrival.X][(int)arrival.Y]-1 ;

	while (arrival.X <= 0 && arrival.Y <= 0 && m_world->getCube(arrival.X, arrival.Y, zStart)->_Type != CUBE_NEIGE)
	{
		arrival = NYVert2Df(rand_a_b(_positionInitiale.X  - 40, _positionInitiale.X + 40), rand_a_b(_positionInitiale.Y - 40, _positionInitiale.Y  + 40));
		 zStart = m_world->_MatriceHeights[(int)arrival.X][(int)arrival.Y] - 1;
	}
	
	position = NYVert3Df(arrival.X * NYCube::CUBE_SIZE, arrival.Y * NYCube::CUBE_SIZE, _world->_MatriceHeights[(int)arrival.X][(int)arrival.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);

	life = 5000;
	m_lastUpdate.start();
}

Yeti::~Yeti()
{

}

void Yeti::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

void Yeti::GetCreaturesInSight()
{
	m_creaturesInSight.clear();
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		for (int j = 0; j < (*m_entities)[type].size(); ++j)
		{
			if (m_cone.IsInSight((*m_entities)[type][j]->position) && 
				(*m_entities)[type][j]->GetID() != this->GetID() && 
				type != YETI && 
				type != GRIFFONKITU)
			{
				m_creaturesInSight.push_back((*m_entities)[type][j]);
			}
		}
	}
	
}

Path Yeti::GetPath()
{
	return m_path;
}

bool Yeti::HasAPath()
{
	return m_path.GetSize() > 0;
}

NYVert3Df Yeti::FindClosestCubeSnow()
{
	NYVert3Df offset;
	float lenght = 10000000.0f;
	float tmpLenght = 0.0f;
	NYCube * tmpCube = NULL;
	for (int x = 0; x<MAT_SIZE_CUBES; x++)
	{
		for (int y = 0; y<MAT_SIZE_CUBES; y++)
		{
			for (int z = 0; z<MAT_HEIGHT_CUBES; z++)
			{
				tmpCube = m_world->getCube(x, y, z);
				if (tmpCube->_Type == CUBE_NEIGE && m_world->getCube(x, y, z+1)->_Type == CUBE_AIR)
				{
					offset = position - NYVert3Df(x, y, z)*NYCube::CUBE_SIZE;
					tmpLenght = offset.getMagnitude();
					if (tmpLenght < lenght)
					{
						lenght = tmpLenght;
						return NYVert3Df(x, y, z);
					}
				}
			}
		}
	}
}

void Yeti::UpdateIA()
{
	//Update Cone de vision
	m_cone.SetPosition(position);
	m_cone.SetOrientation(direction);

	//Update Creatures in sight
	GetCreaturesInSight();

	UpdateTimers();

	if (m_creaturesInSight.size() > 0 &&
		target == NULL &&
		m_creaturesInSight[0]->type == RADIATOSAURE)
	{
		target = m_creaturesInSight[0];
		PushState(STATE_Dance);
	}

	if (m_creaturesInSight.size() > 0 && 
		target == NULL &&
		m_creaturesInSight[0]->type != RADIATOSAURE)
	{
		target = m_creaturesInSight[0];
		PushState(STATE_Attack);
	}

	Update();//Update the state machine

	m_lastUpdate.start();
	
}

void Yeti::UpdateTimers()
{
	if (m_currentState == STATE_Move || m_currentState == STATE_Attack || m_currentState == STATE_FindPath)
	{
		m_timerWandering += m_lastUpdate.getElapsedSeconds();
	}
	
	else if (m_currentState == STATE_Attack)
	{
		if (m_timerAttack >= m_durationAttack)
		{
			PushState(STATE_Reproduction);
			m_timerAttack = 0.0f;
			m_timerWandering = 0.0f;
		}
		m_timerAttack += m_lastUpdate.getElapsedSeconds();
	}

}

int Yeti::FindClosestWaypoint(Path _path)
{
	int record = MAXINT;
	int index = 0;
	for (int i = 0; i < _path.GetSize(); ++i)
	{
		NYVert3Df diff = _path.GetWaypoint(i) / NYCube::CUBE_SIZE - position / NYCube::CUBE_SIZE;
		float lenght = diff.getSize();
		if (lenght < record)
		{
			record = lenght;
			index = i;
		}
	}

	return index;
}

void Yeti::Draw()
{
	glPushMatrix();

 if (m_currentState == STATE_Attack)
	{
		glColor3f(1.0f, 0.0f, 1.0f);
	}
	else
	{
		glColor3f(0.0f, 0.5f, 1.0f);
	}
	
	glTranslatef(position.X, position.Y, position.Z );

	if (m_currentState != STATE_Dead)
	{
		glutSolidCube(NYCube::CUBE_SIZE);
		glTranslatef(0, 0, NYCube::CUBE_SIZE);
		glutSolidCube(NYCube::CUBE_SIZE);
		glTranslatef(0, 0, NYCube::CUBE_SIZE);
		glutSolidCube(NYCube::CUBE_SIZE);
		glTranslatef(0, 0, NYCube::CUBE_SIZE);
		glutSolidCube(NYCube::CUBE_SIZE);
	}
	
	glPopMatrix();

	if (m_debugDraw)
	{
		m_cone.DebugDraw();
		m_path.DrawPath();
	}
}


bool Yeti::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine
			
	//Reception des messages
	OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		//life -= *data;//I remove the value of the message data from my life.
		delete data;//Delete the data

		//if (life <= 0)//If i don't have any life
	//	{
		//	PushState(STATE_Dead);//Use PushState to go in an other state
		//}
	}//Message Attack

	OnMsg(MSG_PrepareAttack)
	{
		PushState(STATE_Attack);
		m_timerAttack = 0.0f;
	}//Message Attack

	//Initialize
	State(STATE_Initialize)
	OnEnter
	PushState(STATE_FindPath);
	
	//Find path
	State(STATE_FindPath)
	OnEnter

	m_timerTryFindPath = 0.0f;
	m_path.Clear();
	
	NYVert2Df arrival = NYVert2Df(rand_a_b(position.X / NYCube::CUBE_SIZE-6, position.X  / NYCube::CUBE_SIZE+6), rand_a_b((position.Y) / NYCube::CUBE_SIZE-6, position.Y  / NYCube::CUBE_SIZE+6));

	if (arrival.X > 0 && arrival.Y > 0)
	{
		int zStart = m_world->_MatriceHeights[(int)arrival.X][(int)arrival.Y] - 1;

		if (m_world->getCube(arrival.X, arrival.Y, zStart)->_Type == CUBE_NEIGE)
		{
			m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
			m_currentIndex = FindClosestWaypoint(m_path);
		}
	}
	isArrived = false;
	
	OnUpdate
	
	if (HasAPath())
	{
		PushState(STATE_Move);
	}

	if (m_timerTryFindPath >= m_timeTryFindPath)
	{
		PushState(STATE_FindPath);
	}
	m_timerTryFindPath += m_lastUpdate.getElapsedSeconds();

	OnExit

	//Move
	State(STATE_Move)
	OnEnter
	m_timerWandering = 0.0f;
	OnUpdate
	if (m_currentIndex < m_path.GetSize())
	{
		//On récupère la direction
		direction = m_path.GetWaypoint(m_currentIndex) - position;

		float lenght = direction.getSize();
		direction.normalize();

		if (lenght < 3.0f)
		{
			++m_currentIndex;
		}
		else
		{
			position += direction * m_speed * m_lastUpdate.getElapsedSeconds();
		}
	}
	else
	{
		direction = direction.rotate(NYVert3Df(0, 0, 1), rand() % 180);

		PushState(STATE_FindPath);
		isArrived = true;
	}

	OnExit
	m_path.Clear();

	//Attack
	State(STATE_Attack)
	OnEnter
	
	m_timeElapsedBetween2Attacks = 0;
	OnUpdate
	
	NYVert2Df arrival = NYVert2Df(target->position.X / NYCube::CUBE_SIZE,target->position.Y / NYCube::CUBE_SIZE );
	int zStart = m_world->_MatriceHeights[(int)arrival.X][(int)arrival.Y] - 1;

	m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();
	
	if (m_distanceToTarget >= m_viewDistance)
	{
		target = NULL;
		PushState(STATE_FindPath);
	}
	else
	{
			direction = target->position - position;
			direction.normalize();

			if (m_world->getCube(arrival.X, arrival.Y, zStart)->_Type == CUBE_NEIGE)
			{
			

			if (m_distanceToTarget > NYCube::CUBE_SIZE / 8)
			{
				position += direction * m_speed * m_lastUpdate.getElapsedSeconds();
			}
			}
			if (m_timeElapsedBetween2Attacks >= m_timeBetween2Attacks && m_distanceToTarget <= m_viewDistance / 4.0f)
			{
				//SendMsg(MSG_Attack, target->GetID(), 0);
				m_timeElapsedBetween2Attacks = 0.0f;
			}

			m_timeElapsedBetween2Attacks += m_lastUpdate.getElapsedSeconds();
			
		}
		
	OnExit
	m_path.Clear();
	
	//Fuite
	State(STATE_Dance)
		OnEnter

		m_timeElapsedBetween2Attacks = 0;
	OnUpdate

		m_path.Clear();

//	NYVert2Df arrival = NYVert2Df(rand_a_b(position.X / NYCube::CUBE_SIZE - 6, position.X / NYCube::CUBE_SIZE + 6), rand_a_b((position.Y) / NYCube::CUBE_SIZE - 6, position.Y / NYCube::CUBE_SIZE + 6));
		//NYVert2Df arrival = NYVert2Df(target->position.X / NYCube::CUBE_SIZE, target->position.Y / NYCube::CUBE_SIZE);

	m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();

	if (m_distanceToTarget >= m_viewDistance)
	{
		target = NULL;
		PushState(STATE_FindPath);
	}
	else
	{
		direction = target->position - position;
		direction.normalize();

		NYVert2Df arrival = NYVert2Df(rand_a_b((position.X - direction.X) / NYCube::CUBE_SIZE - 3, (position.X - direction.X) / NYCube::CUBE_SIZE + 3), rand_a_b((position.Y - direction.Y) / NYCube::CUBE_SIZE - 6, (position.Y - direction.Y) / NYCube::CUBE_SIZE + 3));
		int zStart = m_world->_MatriceHeights[(int) arrival.X][(int) arrival.Y] - 1;

		if (arrival.X > 0 && arrival.Y > 0)
		{
			int zStart = m_world->_MatriceHeights[(int) arrival.X][(int) arrival.Y] - 1;

			if (m_world->getCube(arrival.X, arrival.Y, zStart)->_Type == CUBE_NEIGE)
			{
				m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
				m_currentIndex = FindClosestWaypoint(m_path);
			}
		}
		isArrived = false;

		//if (HasAPath())
	//	{
		//	PushState(STATE_Move);
		//}

		if (m_world->getCube(arrival.X, arrival.Y, zStart)->_Type == CUBE_NEIGE)
		{
				position -= direction * m_speed * m_lastUpdate.getElapsedSeconds();	
		}
	}

	OnExit
		m_path.Clear();
	//Dead
	//State(STATE_Dead)
	//OnMsg(MSG_Attack)
	//OnMsg(MSG_PrepareAttack)
	//OnEnter
	
	EndStateMachine
}

