#include "Yeti.h"

Yeti::Yeti(NYWorld * _world, NYVert2Df _positionInitiale) :
IABase(_world), m_cone(m_viewAngle, m_viewDistance)
{
	Initialize();
	type = YETI;

	NYVert2Df arrival = NYVert2Df(rand_a_b(_positionInitiale.X - 40, _positionInitiale.X + 40), rand_a_b(_positionInitiale.Y - 40, _positionInitiale.Y + 40));
	int zStart = m_world->_MatriceHeights[(int) arrival.X][(int) arrival.Y] - 1;

	while (arrival.X < 1 || arrival.Y <= 1 || m_world->getCube(arrival.X, arrival.Y, zStart)->_Type != CUBE_NEIGE)
	{
		arrival = NYVert2Df(rand_a_b(_positionInitiale.X - 40, _positionInitiale.X + 40), rand_a_b(_positionInitiale.Y - 40, _positionInitiale.Y + 40));
		zStart = m_world->_MatriceHeights[(int) arrival.X][(int) arrival.Y] - 1;
	}

	position = NYVert3Df(arrival.X * NYCube::CUBE_SIZE, arrival.Y * NYCube::CUBE_SIZE, _world->_MatriceHeights[(int) arrival.X][(int) arrival.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);

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
		eTypeCreature type = (eTypeCreature) i;
		if (type != YETI && type != GRIFFONKITU
			&&
			type != MOUCHE &&
			type != PARASITE
			&& type != VAUTOUR )
		{
			for (int j = 0; j < (*m_entities)[type].size(); ++j)
			{
				NYVert3Df distance = (*m_entities)[type][j]->position - position;

				if (m_cone.IsInSight((*m_entities)[type][j]->position) &&
					(*m_entities)[type][j]->GetID() != this->GetID())
				{
					m_creaturesInSight.push_back((*m_entities)[type][j]);
				}
				else if (distance.getSize() < m_smellDistance * NYCube::CUBE_SIZE)
				{
					m_creaturesInSight.push_back((*m_entities)[type][j]);
				}
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

vector<NYVert2Df> Yeti::FindClosestCubesSnow(NYVert2Df pos)
{
	float tmpLenght = 0.0f;
	NYCube * tmpCube = NULL;
	vector<NYVert2Df> returned;
	int posOffset = 20;
	NYVert2Df newPos = NYVert2Df((int) pos.X, (int) pos.Y);
	int posZ = m_world->_MatriceHeights[(int) pos.X][(int) pos.Y] - 1;
	returned.clear();
	for (int y = -posOffset; y <= posOffset; y += posOffset)
	{
		if (y != 0)
		{
			for (int x = -posOffset; x <= posOffset; x += 2)
			{
				for (int i = 0; i <= 1; i++)
				{
					float newX = pos.X + x;
					float newY = pos.Y + y;
					if (i == 1){
						newX = pos.X + y;
						newY = pos.Y + x;
					}

					if ((int) newX > 1 && (int) newY > 1)
					{
						newPos = NYVert2Df((int) newX, (int) newY);
						posZ = m_world->_MatriceHeights[(int) newX][(int) newY] - 1;

						tmpCube = m_world->getCube(newPos.X, newPos.Y, posZ);
						if (tmpCube->_Type == CUBE_NEIGE && m_world->getCube(newPos.X, newPos.Y, posZ + 1)->_Type == CUBE_AIR)
						{
							returned.push_back(newPos);
						}
					}
				}
			}
		}
	}
	return returned;
}


void Yeti::UpdateIA()
{
	//Update Cone de vision
	m_cone.SetPosition(position);
	m_cone.SetOrientation(direction);

	//Update Creatures in sight
	GetCreaturesInSight();

	UpdateTimers();

	if (m_initialDuration > m_initialWait)
	{
		if (m_creaturesInSight.size() > 0 &&
			target == NULL &&
			m_creaturesInSight[0]->type == RADIATOSAURE && !infected)
		{
			target = m_creaturesInSight[0];
			PushState(STATE_Dance);
		}

		else if (m_creaturesInSight.size() > 0 &&
			target == NULL &&
			m_creaturesInSight[0]->type != RADIATOSAURE)
		{
			target = m_creaturesInSight[0];

			PushState(STATE_Attack);
		}
		
	}
	else m_initialDuration += m_lastUpdate.getElapsedSeconds();

	

		Update();//Update the state machine

		m_lastUpdate.start();
	
}

void Yeti::UpdateTimers()
{
	if (m_currentState == STATE_Attack)
	{
		if (m_timerAttack >= m_durationAttack)
		{
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
		glColor3f(0.0f, 1.0f, 0.0f);
	}
	else if (m_currentState == STATE_Dance)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else{
		glColor3f(0.0f, 0.0f, 1.0f);
	}

	glTranslatef(position.X, position.Y, position.Z);

	if (m_currentState != STATE_Dead)
	{
		glutSolidCube(NYCube::CUBE_SIZE * 2);
		glTranslatef(0, 0, NYCube::CUBE_SIZE * 2);
		glutSolidCube(NYCube::CUBE_SIZE * 2);
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
		int * data = (int*) msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		delete data;//Delete the data

	}//Message Attack

	//OnMsg(MSG_PrepareAttack)
	//{
	//	PushState(STATE_Attack);
	//	m_timerAttack = 0.0f;
	//}//Message Attack

	//Initialize
	State(STATE_Initialize)
		OnEnter
		PushState(STATE_FindPath);

	//Find path
	State(STATE_FindPath)
		OnEnter
	    target = NULL;
		m_path.Clear();
	vector<NYVert2Df>  resultVert = FindClosestCubesSnow(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE));

	int  random = rand() % resultVert.size();

	m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), resultVert[random], 4, m_path);
	m_currentIndex = FindClosestWaypoint(m_path);
	isArrived = false;

	OnUpdate
		if (HasAPath())
		{
			fuite = false;
			PushState(STATE_Move);
		}
		else PushState(STATE_FindPath);

			//Move
			State(STATE_Move)
			OnEnter
			m_timerWandering = 0.0f;
		m_timerTryFindPath = 0.0f;

		OnUpdate
			if (m_currentIndex < m_path.GetSize())
			{
				//On récupère la direction
				direction = m_path.GetWaypoint(m_currentIndex) - position;

				float lenght = direction.getSize();
				direction.normalize();

				if (lenght < 1.0f)
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
				if ((m_timerTryFindPath >= m_timeTryFindPath))
				{
					direction = direction.rotate(NYVert3Df(0, 0, 1), rand() % 180);
					m_timerTryFindPath = 0.0f;

				}
				else m_timerTryFindPath += m_lastUpdate.getElapsedSeconds();

				if ((m_timerWandering >= m_durationWandering))
				{
					//faire caca
					RessourcesManager *pRessourceMgr = RessourcesManager::GetSingleton();
					
					*pRessourceMgr->Create(CROTTE, NYVert3Df(position.X, position.Y, position.Z-4), 1000);
					*pRessourceMgr->Create(CROTTE, NYVert3Df(position.X+10, position.Y, position.Z-4), 1000);
					isArrived = true;

					PushState(STATE_FindPath);
				}
				else m_timerWandering += m_lastUpdate.getElapsedSeconds();
			}

		OnExit
			m_path.Clear();

		//Attack
		State(STATE_Attack)
			OnEnter
			m_timeElapsedBetween2Attacks = 0;
		OnUpdate
	
		if (target == NULL){
			PushState(STATE_FindPath);
		}
		else
		{
			m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();

			if ((int) m_distanceToTarget >= (int) m_viewDistance/2)
			{
				target = NULL;
				PushState(STATE_FindPath);
			}
			else if ((int)m_distanceToTarget > NYCube::CUBE_SIZE / 4)
			{
				int xTarget = target->position.X / NYCube::CUBE_SIZE;
				int  yTarget = target->position.Y / NYCube::CUBE_SIZE;

				int zStart = m_world->_MatriceHeights[xTarget][yTarget] - 1;

				direction = target->position - position;
				direction.normalize();

				if (m_world->getCube(xTarget, yTarget, zStart)->_Type == CUBE_NEIGE)
				{
					position += direction * m_speed * m_lastUpdate.getElapsedSeconds();

				}
				//if (m_timeElapsedBetween2Attacks >= m_timeBetween2Attacks && m_distanceToTarget <= m_viewDistance / 4.0f)
			//	{
					//SendMsg(MSG_Attack, target->GetID(), 0);
				//	m_timeElapsedBetween2Attacks = 0.0f;
				//}

				//m_timeElapsedBetween2Attacks += m_lastUpdate.getElapsedSeconds();
			}
		}
		OnExit
			m_path.Clear();
			target = NULL;

		//Fuite
		State(STATE_Dance)
			OnEnter

		//m_timerTryFindPath = 0.0f;
		m_path.Clear();

		direction = target->position - position;
		vector<NYVert2Df>  resultVert = FindClosestCubesSnow(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE));

		m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();

		direction.normalize();

		for (std::vector<NYVert2Df>::iterator i = resultVert.begin(); i != resultVert.end(); ++i)
		{
			NYVert2Df foundPosition = (NYVert2Df) (*i);
			int posZ = m_world->_MatriceHeights[(int) foundPosition.X][(int) foundPosition.Y] - 1;

			NYVert3Df foundPos = NYVert3Df(foundPosition.X, foundPosition.Y, posZ);

			if ((m_distanceToTarget < (foundPos / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize()));
			{
				NYVert2Df arrival = NYVert2Df(foundPosition.X, foundPosition.Y);

				m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
				m_currentIndex = FindClosestWaypoint(m_path);
			}

			isArrived = false;
		}
		OnUpdate
			m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();

		if (target == NULL || m_distanceToTarget >= m_viewDistance/2)
		{
			isArrived = true;
			target = NULL;
			PushState(STATE_FindPath);
		}
		else
		{
			if (HasAPath() && (m_currentIndex < m_path.GetSize()))
			{
				//On récupère la direction
				direction = m_path.GetWaypoint(m_currentIndex) - position;

				float lenght = direction.getSize();
				direction.normalize();

				NYVert3Df tempDir = position + direction * m_speed * m_lastUpdate.getElapsedSeconds();

				if (lenght < 3.0f || tempDir.X < 1.0f || tempDir.Y < 1.0f)
				{
					++m_currentIndex;
				}
				else
				{
					position = tempDir;
				}
			}
			else
			{
				PushState(STATE_Dance);
			}
			//m_timerTryFindPath += m_lastUpdate.getElapsedSeconds();
		}

		//OnExit
			//m_path.Clear();
			//target = NULL;

		EndStateMachine
}
