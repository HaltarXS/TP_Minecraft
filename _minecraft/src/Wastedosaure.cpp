#include "Wastedosaure.h"


Wastedosaure::Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale):
IABase(_world), m_cone(m_viewAngle, m_viewDistance)
{
	Initialize();
	type = WASTEDOSAURE;
	position = NYVert3Df(_positionInitiale.X * NYCube::CUBE_SIZE, _positionInitiale.Y*NYCube::CUBE_SIZE, _world->_MatriceHeights[(int)_positionInitiale.X][(int)_positionInitiale.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);
	life = 50;
	//m_durationWandering += WastedosaureManager::GetSingleton()->rand_a_b(-2,2);
	//m_durationReproduction += WastedosaureManager::GetSingleton()->rand_a_b(-2, 2);
	m_lastUpdate.start();
}

Wastedosaure::~Wastedosaure()
{

}

void Wastedosaure::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

void Wastedosaure::GetCreaturesInSight()
{
	m_creaturesInSight.clear();
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		for (int j = 0; j < (*m_entities)[type].size(); ++j)
		{
			if (m_cone.IsInSight((*m_entities)[type][j]->position) && 
				(*m_entities)[type][j]->GetID() != this->GetID() && 
				type != WASTEDOSAURE && 
				type != GRIFFONKITU)
			{
				m_creaturesInSight.push_back((*m_entities)[type][j]);
			}
		}
	}
	
}

Path Wastedosaure::GetPath()
{
	return m_path;
}

bool Wastedosaure::HasAPath()
{
	return m_path.GetSize() > 0;
}

NYCubeType Wastedosaure::GetCubeUnderType(IABase * target)
{
	NYVert3Df positionCube = target->position / NYCube::CUBE_SIZE;
	positionCube.Z = m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y];
	return m_world->getCube(positionCube.X, positionCube.Y, positionCube.Z)->_Type;
}

NYVert3Df Wastedosaure::FindClosestCubeWater()
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
				if (tmpCube->_Type == CUBE_EAU && m_world->getCube(x, y, z+1)->_Type == CUBE_AIR)
				{
					offset = position - NYVert3Df(x, y, z)*NYCube::CUBE_SIZE;
					tmpLenght = offset.getMagnitude();
					if (tmpLenght < lenght)
					{
						lenght = tmpLenght;
						m_cubeWater = NYVert3Df(x, y, z);
					}
				}
			}
		}
	}

	return m_cubeWater;
}

void Wastedosaure::UpdateIA()
{
	if (m_currentState != STATE_Dead)
	{
		//Update Cone de vision
		m_cone.SetPosition(position);
		m_cone.SetOrientation(direction);

		//Update Creatures in sight
		GetCreaturesInSight();

		UpdateTimers();

		if (leader != NULL && leader->m_currentState == STATE_Dead)
		{
			WastedosaureManager::GetSingleton()->AssignToAGroup(this);
		}

		if (m_creaturesInSight.size() > 0 &&
			target == NULL &&
			m_currentState != STATE_Reproduction &&
			m_currentState != STATE_Suicide &&
			m_currentState != STATE_Dead &&
			m_currentSize >= m_maxSize / 2.0f &&
			GetCubeUnderType(m_creaturesInSight[0]) != NYCubeType::CUBE_EAU)
		{
			//cout << "Creature in sight\n";
			WastedosaureManager::GetSingleton()->PrepareAttack(m_creaturesInSight[0]);
		}

		m_currentSize = (1 - (m_timerGrow / m_durationGrow))*m_minSize + (m_timerGrow / m_durationGrow)*m_maxSize;
		m_currentSize = min(m_currentSize, m_maxSize);

		m_timerGrow += m_lastUpdate.getElapsedSeconds();
	}
	


	Update();//Update the state machine

	m_lastUpdate.start();
	
}

void Wastedosaure::UpdateTimers()
{
	if (m_currentState == STATE_Move || /*m_currentState == STATE_Attack ||*/ m_currentState == STATE_FindPath)
	{
		if (m_timerWandering >= m_durationWandering)
		{
			PushState(STATE_Reproduction);
			m_timerWandering = 0.0f;
			m_timerReproduction = 0.0f;
			m_timerAttack = 0.0f;
		}
		m_timerWandering += m_lastUpdate.getElapsedSeconds();
	}
	else if (m_currentState == STATE_Reproduction)
	{
		if (m_timerReproduction >= m_durationReproduction)
		{
			PushState(STATE_Move);
			m_timerReproduction = 0.0f;
			m_timerWandering = 0.0f;
			m_timerAttack = 0.0f;
		}
		m_timerReproduction += m_lastUpdate.getElapsedSeconds();
	}
	else if (m_currentState == STATE_Attack)
	{
		if (m_timerAttack >= m_durationAttack)
		{
			PushState(STATE_Reproduction);
			m_timerAttack = 0.0f;
			m_timerWandering = 0.0f;
			m_timerReproduction = 0.0f;
			//target = NULL;
		}
		m_timerAttack += m_lastUpdate.getElapsedSeconds();
	}

	
}

int Wastedosaure::FindClosestWaypoint(Path _path)
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

void Wastedosaure::Draw()
{
	if (m_currentState == STATE_Reproduction)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	else if (m_currentState == STATE_Attack)
	{
		glColor3f(0.7f, 0.2, 0.8);
	}
	else if (m_currentState == STATE_Dead)
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	else
	{
		glColor3f(1.0f, 0, 0);
	}
	
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z - (NYCube::CUBE_SIZE - m_currentSize)/2.0f);
	if (1/*m_currentState != STATE_Dead*/)
	{
		if (m_currentState == STATE_Egg)
		{
			glutSolidSphere(m_minSize, 20, 20);
		}
		else
		{
			glutSolidCube(m_currentSize);
		}
	}
	
	glPopMatrix();

	if (m_debugDraw)
	{
		m_cone.DebugDraw();
		m_path.DrawPath();
	}
}




bool Wastedosaure::States(StateMachineEvent event, MSG_Object * msg, int state)
{
	BeginStateMachine

	//Reception des messages
	OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		life -= *data;//I remove the value of the message data from my life.
		//std::cout << "--Entity " << this->GetID() << "-- Attack from entity " << msg->GetSender() << ". Life removed : " << *data << ". Life remaining : " << m_life << std::endl;
		delete data;//Delete the data

		if (life <= 0)//If i don't have any life
		{
			PushState(STATE_Dead);//Use PushState to go in an other state
		}
	}//Message Attack

	OnMsg(MSG_PrepareAttack)
	{
		m_timerAttack = 0.0f;
	}//Message Attack

	//Initialize
	State(STATE_Initialize)
	OnEnter
	WastedosaureManager::GetSingleton()->AddWastedosaure(this);
	PushState(STATE_Egg);
	partner = NULL;
	

	//Egg
	State(STATE_Egg)
	OnEnter
	m_timerEgg = 0.0f;
	
	OnUpdate

	if (m_timerEgg >= m_timeEgg)
	{
		PushState(STATE_FindPath);
	}
	m_timerEgg += m_lastUpdate.getElapsedSeconds();
	OnExit
	WastedosaureManager::GetSingleton()->AssignToAGroup(this);
	//On assigne son sexe
	m_isMale = (bool)(rand() % 2);
	

	//Find path
	State(STATE_FindPath)
	OnEnter
	m_timerTryFindPath = 0.0f;
	m_path.Clear();
	NYVert2Df arrival = NYVert2Df(WastedosaureManager::GetSingleton()->rand_a_b(1, MAT_SIZE_CUBES - 1), WastedosaureManager::GetSingleton()->rand_a_b(1, MAT_SIZE_CUBES - 1));
	if (leader == NULL)
	{
		m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
	}
	else
	{
		m_path = leader->GetPath(); //On va suivre le chemin du leader pour éviter de faire 1 pf/créature
	}
	//m_currentIndex = 0;
	m_currentIndex = FindClosestWaypoint(m_path);
	if (HasAPath())
	{
		NYVert3Df distanceClosestWP = m_path.GetWaypoint(m_currentIndex) / NYCube::CUBE_SIZE - position / NYCube::CUBE_SIZE;
		if (distanceClosestWP.getSize() > 20.0f)//Si le closest point est trop loin, on lance quand meme un pf pour éviter que l'entité passe à travers les murs
		{
			arrival = NYVert2Df(leader->position.X / NYCube::CUBE_SIZE, leader->position.Y / NYCube::CUBE_SIZE);
			m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
			m_currentIndex = 0;
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
	//cout << "Moving\n";
	m_timerWandering = 0.0f;
	m_timerReproduction = 0.0f;
	if (partner != NULL && partner->GetState() != STATE_Move && partner->GetState() != STATE_Dead)
	{
		partner->PushState(STATE_Move);
	}
	OnUpdate
	if (m_currentIndex < m_path.GetSize() - (m_path.GetSize() > groupPosition?groupPosition:0))
	{
		//On récupère la direction
		direction = m_path.GetWaypoint(m_currentIndex) - position;
		float lenght = direction.getSize();
		direction.normalize();

		if (lenght < 4.0f)
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
	m_distanceToTarget = NYVert3Df(position / NYCube::CUBE_SIZE - target->position / NYCube::CUBE_SIZE).getSize();
	if (m_distanceToTarget <= m_viewDistance)
	{
		direction = target->position - position;
		direction.normalize();
		position += direction * m_speed * m_lastUpdate.getElapsedSeconds();

		if (m_timeElapsedBetween2Attacks >= m_timeBetween2Attacks && m_distanceToTarget <= m_viewDistance/2.0f)
		{
			SendMsg(MSG_Attack, target->GetID(), new int(m_damages + rand() % 5));
			//cout << "Send message\n" << endl;
			m_timeElapsedBetween2Attacks = 0.0f;
		}

		m_timeElapsedBetween2Attacks += m_lastUpdate.getElapsedSeconds();
	}

	if (target->GetState() == STATE_Dead)
	{
		target = NULL;
		PushState(STATE_FindPath);
	}
			
	OnExit
	m_path.Clear();
	target == NULL;

	//Reproduction
	State(STATE_Reproduction)
	OnEnter
	if (partner == NULL)
		WastedosaureManager::GetSingleton()->FindPartner(this);

	m_needPartner = true;
	
	if (!hasPartner || partner == NULL || partner->GetState() == STATE_Dead)//Alors le wastedosaure se suicide :(
	{
		PushState(STATE_Suicide);
	}
	else
	{
		m_path.Clear();
		m_timerWandering = 0.0f;
		m_timerReproduction = 0.0f;
		m_currentIndex = 0;

		if (partner->GetState() != STATE_Reproduction)
		{
			partner->PushState(STATE_Reproduction);
		}

		if (arrivalPartner == NYVert2Df(0, 0))
		{
			WastedosaureManager::GetSingleton()->FindReproductionPlace(this, this->partner);
		}

		m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrivalPartner, 1, m_path);
		
	}
	OnUpdate

	//Follow the path
	if (HasAPath() && m_currentIndex < m_path.GetSize())
	{
		//On récupère la direction
		direction = m_path.GetWaypoint(m_currentIndex) - position;
		float lenght = direction.getSize();
		direction.normalize();

		if (lenght < 4.0f)
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
		m_path.Clear();
		NYVert3Df tmpArrival = NYVert3Df(arrivalPartner.X, arrivalPartner.Y, m_world->_MatriceHeights[(int)arrivalPartner.X][(int)arrivalPartner.Y]);
		NYVert3Df offsetPartners1 = position / NYCube::CUBE_SIZE - tmpArrival;
		NYVert3Df offsetPartners2 = partner->position / NYCube::CUBE_SIZE - tmpArrival;
		//If close enought, reproduce
		if (m_canReproduce && 
			m_counterReproduction < m_maxReproductions && 
			offsetPartners1.getSize() <= 1 && 
			offsetPartners2.getSize() <= 1 &&
			(*m_entities)[WASTEDOSAURE].size() < MAX_WASTEDOSAURE/*&& m_isMale != partner->m_isMale*/)
		{
			m_counterReproduction++;
			partner->m_counterReproduction++;
			partner->m_canReproduce = false;
			m_canReproduce = false;

			Wastedosaure * w = new Wastedosaure(m_world, NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE));
			w->SetEntities(m_entities);
			(*m_entities)[WASTEDOSAURE].push_back(w);
		}
	}
	
	
	
	OnExit
	m_canReproduce = true;
	arrivalPartner = NYVert2Df(0, 0);
	m_path.Clear();


	//Suicide
	State(STATE_Suicide)
	OnEnter
	m_path.Clear();
	FindClosestCubeWater();
	m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), NYVert2Df(m_cubeWater.X, m_cubeWater.Y), 2, m_path, true);
	m_currentIndex = 0;
	target = NULL;
	partner = NULL;
	OnUpdate
	if (HasAPath())
	{
		if (m_currentIndex < m_path.GetSize())
		{
			//On récupère la direction
			direction = m_path.GetWaypoint(m_currentIndex) - position;
			float lenght = direction.getSize();
			direction.normalize();

			if (lenght < 4.0f)
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
			PushState(STATE_Dead);
		}
	}
	else
	{
		PushState(STATE_Dead);
	}

	OnExit

	//Dead
	State(STATE_Dead)
	OnMsg(MSG_Attack)
	OnMsg(MSG_PrepareAttack)
	OnEnter
	//cout << "Im dead. ID " << GetID() <<"\n";
	

	EndStateMachine
}