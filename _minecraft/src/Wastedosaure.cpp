#include "Wastedosaure.h"


Wastedosaure::Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale):
IABase(_world), m_cone(50.0f,150)
{
	Initialize();
	type = WASTEDOSAURE;
	position = NYVert3Df(_positionInitiale.X * NYCube::CUBE_SIZE, _positionInitiale.Y*NYCube::CUBE_SIZE, _world->_MatriceHeights[(int)_positionInitiale.X][(int)_positionInitiale.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);
	life = 50;
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
			if (m_cone.IsInSight((*m_entities)[type][j]->position) && (*m_entities)[type][j]->GetID() != this->GetID())
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
	//Update Cone de vision
	m_cone.SetPosition(position);
	m_cone.SetOrientation(direction);

	//Update Creatures in sight
	GetCreaturesInSight();

	Update();//Update the state machine

	UpdateTimers();

	if (leader != NULL && leader->m_currentState == STATE_Dead)
	{
		WastedosaureManager::GetSingleton()->AssignToAGroup(this);
	}

	if (m_needPartner && (m_currentState == STATE_Move || m_currentState == STATE_Reproduction) && (partner == NULL || partner->GetState() == STATE_Dead))//Alors le wastedosaure se suicide :(
	{
		PushState(STATE_Suicide);
	}
}

void Wastedosaure::UpdateTimers()
{
	if (m_currentState == STATE_Move || m_currentState == STATE_Attack)
	{
		if (m_timerWandering >= m_durationWandering)
		{
			PushState(STATE_Reproduction);
			m_timerWandering = 0.0f;
		}
		m_timerWandering += NYRenderer::_DeltaTime;
	}
	else if (m_currentState == STATE_Reproduction)
	{
		if (m_timerReproduction >= m_durationReproduction)
		{
			PushState(STATE_Move);
			m_timerReproduction = 0.0f;
		}
		m_timerReproduction += NYRenderer::_DeltaTime;
	}
}

int Wastedosaure::FindClosestWaypoint(Path _path)
{
	int record = MAXINT;
	int index = 0;
	for (int i = 0; i < _path.GetSize(); ++i)
	{
		NYVert3Df diff = _path.GetWaypoint(i) - position;
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
		glColor3f(0.5f, 0.5f, 0);
	}
	else
	{
		glColor3f(1.0f, 0, 0);
	}
	
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	if (m_currentState != STATE_Dead)
	{
		if (m_currentState == STATE_Egg)
		{
			glutSolidSphere(5, 20, 20);
		}
		else
		{
			glutSolidCube(8);
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

	//Initialize
	State(STATE_Initialize)
	OnEnter
	PushState(STATE_Egg);
	

	//Egg
	State(STATE_Egg)
	OnEnter
	m_timerEgg = 0.0f;
	WastedosaureManager::GetSingleton()->AddWastedosaure(this);
	OnUpdate

	if (m_timerEgg >= m_timeEgg)
	{
		PushState(STATE_FindPath);
	}
	m_timerEgg += NYRenderer::_DeltaTime;
	OnExit
	WastedosaureManager::GetSingleton()->AssignToAGroup(this);
	//On assigne son sexe
	m_isMale = (bool)(rand() % 2);
	

	//Find path
	State(STATE_FindPath)
	OnEnter
	m_timerTryFindPath = 0.0f;
	m_path.Clear();
	NYVert2Df arrival = NYVert2Df(rand() % MAT_SIZE_CUBES, rand() % MAT_SIZE_CUBES);
	if (leader == NULL)
	{
		m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
	}
	else
	{
		m_path = leader->GetPath(); //On va suivre le chemin du leader pour éviter de faire 1 pf/créature
	}
	
	m_currentIndex = FindClosestWaypoint(m_path);
	if (HasAPath())
	{
		NYVert3Df distanceClosestWP = m_path.GetWaypoint(m_currentIndex) - position;
		if (distanceClosestWP.getSize() > 10.0f)//Si le closest point est trop loin, on lance quand meme un pf pour éviter que l'entité passe à travers les murs
		{
			m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, 1, m_path);
		}
	}
	
	
	OnUpdate
	if (HasAPath())
	{
		PushState(STATE_Move);
	}

	if (m_timerTryFindPath >= m_timeTryFindPath)
	{
		PushState(STATE_FindPath);
	}
	m_timerTryFindPath += NYRenderer::_DeltaTime;

	OnExit
	

	//Move
	State(STATE_Move)
	OnEnter
	//cout << "Moving\n";
	m_timerWandering = 0.0f;
	m_timerReproduction = 0.0f;
	OnUpdate
	if (m_currentIndex < m_path.GetSize()-groupPosition)
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
			position += direction * m_speed;
		}
	}
	else
	{
		PushState(STATE_FindPath);
	}

	OnExit


	//Reproduction
	State(STATE_Reproduction)
	OnEnter
	m_needPartner = true;
	WastedosaureManager::GetSingleton()->FindPartner(this);
	m_path.Clear();
	m_timerWandering = 0.0f;
	m_timerReproduction = 0.0f;
	
	
	if (arrivalPartner == NYVert2Df(0, 0))
	{
		WastedosaureManager::GetSingleton()->FindReproductionPlace(this, this->partner);
		
	}
	//cout << arrivalPartner.X << "," << arrivalPartner.Y << endl;
	m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrivalPartner, 1, m_path);
	
	m_currentIndex = 0;
	//cout << "Reproduction\n";
	OnUpdate
	/*if (!HasAPath())
		PushState(STATE_FindPath);*/
	//Follow the path
	if (HasAPath() && m_currentIndex < m_path.GetSize())
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
			position += direction * m_speed;
		}
	}
	else
	{
		NYVert3Df tmpArrival = NYVert3Df(arrivalPartner.X, arrivalPartner.Y, m_world->_MatriceHeights[(int)arrivalPartner.X][(int)arrivalPartner.Y]);
		NYVert3Df offsetPartners1 = position / NYCube::CUBE_SIZE - tmpArrival;
		NYVert3Df offsetPartners2 = partner->position / NYCube::CUBE_SIZE - tmpArrival;
		if (m_canReproduce && m_counterReproduction < m_maxReproductions && offsetPartners1.getSize() <= 1 && offsetPartners2.getSize() <= 1 /*&& m_isMale != partner->m_isMale*/)
		{
			m_counterReproduction++;
			partner->m_counterReproduction++;
			partner->m_canReproduce = false;
			m_canReproduce = false;

			/*Wastedosaure * w = new Wastedosaure(m_world, NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE));
			w->SetEntities(m_entities);
			(*m_entities)[WASTEDOSAURE].push_back(w);*/
		}
	}
	
	//If close enought, reproduce
	
	OnExit
	m_canReproduce = true;
	arrivalPartner = NYVert2Df(0, 0);

	//Suicide
	State(STATE_Suicide)
	OnEnter
	m_path.Clear();
	FindClosestCubeWater();
	m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), NYVert2Df(m_cubeWater.X, m_cubeWater.Y), 2, m_path, true);
	m_currentIndex = 0;
	OnUpdate
	if (HasAPath())
	{
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
				position += direction * m_speed;
			}
		}
		else
		{
			PushState(STATE_Dead);
		}
	}

	OnExit

	//Dead
	State(STATE_Dead)
	OnMsg(MSG_Attack)
	OnEnter
	cout << "Im dead\n";
	

	EndStateMachine
}