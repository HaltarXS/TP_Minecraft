#include "GlaceGouille.h"


GlaceGouille::GlaceGouille(NYWorld *pWorld, NYVert2Df pos):
IABase(pWorld), m_cone(90.0f, 30)
{
	//Init state machine
	Initialize();

	//Init Type
	type = GLACEGOUILLE;

	//Init Position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;

	m_pathfinding = Pathfinding::GetSingleton();

}


GlaceGouille::~GlaceGouille()
{
}

void GlaceGouille::UpdateIA()
{
	//Update Cone de vision
	m_cone.SetPosition(position);
	m_cone.SetOrientation(direction);
	//Update State machine
	Update();
	//cout << m_LastUpdateTimer.getElapsedMs() << endl;
	//UpdateTimer();
	m_LastUpdateTimer.start();
}


void GlaceGouille::Draw()
{
	//Basic rendering
	//glColor3f(244.0/255.0, 250.0/255.0, 252.0/255.0);
	glColor3f(1, 1, 1);
	glPushMatrix();

	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE);
	
	if (m_currentState != STATE_Egg)
	{
		glPushMatrix();
		glTranslatef(0, 0, NYCube::CUBE_SIZE);
		glutSolidCube(NYCube::CUBE_SIZE);
		glPopMatrix();
	}

	glPopMatrix();

	if (m_debugDraw)
	{
		m_cone.DebugDraw();
		m_path.DrawPath();
	}
}

void GlaceGouille::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

bool GlaceGouille::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

	OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		m_life -= *data;//I remove the value of the message data from my life.
		std::cout << "--Entity " << this->GetID() << "-- Attack from entity " << msg->GetSender() << ". Life removed : " << *data << ". Life remaining : " << m_life << std::endl;
		delete data;//Delete the data

		if (m_life <= 0)//If i don't have any life
		{
			PushState(STATE_Dead);//Use PushState to go in an other state
		}
	}//Message Attack

	State(STATE_Initialize)
	OnEnter
	{
		PushState(STATE_Egg);
	}
	
	State(STATE_Egg)
	OnEnter
	{
		//cout << "Enter EggState" << endl;
		m_eggTimer = 0;
	}
	OnUpdate
	{
		//Determine le temps passé en oeuf
		m_eggTimer += m_LastUpdateTimer.getElapsedSeconds();
		if (m_eggTimer >= m_eggTime)
		{
			PushState(STATE_FindPath);
		}
	}

	State(STATE_FindPath)
	OnEnter
	{
		m_pathFound = false;
		//cout << "Enter FindPath" << endl;
	}
	OnUpdate
	{
		//Recherche d'une destination aléatoire dans la neige
		if (!m_pathFound)
		{
			NYVert2Df arrival = NYVert2Df(rand() % MAT_SIZE_CUBES, rand() % MAT_SIZE_CUBES);

			//cout << "X " << newPosition.X << " Y " << newPosition.Y << " Z " << newPosition.Z << endl;
			//cout << "X " << arrival.X << " Y " << arrival.Y << endl;
			if (m_world->getCube(arrival.X, arrival.Y, m_world->_MatriceHeights[(int)arrival.X][(int)arrival.Y] - 1)->_Type == CUBE_NEIGE)
			{
				m_pathFound = m_pathfinding->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), arrival, CUBE_HERBE | CUBE_NEIGE | CUBE_TERRE, m_path);
				//cout << m_pathFound << endl;
			}
		}
		else
		{
			PushState(STATE_Move);
		}
	}

	State(STATE_Move)
	OnEnter
	{
		m_currentPathIndex = 0;
		cout << "Enter Move" << endl;
	}
	OnUpdate
	{
		if (m_currentPathIndex < m_path.GetSize())
		{
			//On récupère la direction
			direction = m_path.GetWaypoint(m_currentPathIndex) - position;
			float lenght = direction.getSize();
			direction.normalize();

			if (lenght < 1.0f)
			{
				++m_currentPathIndex;
			}
			else
			{
				position += direction * m_speed;
			}
		}
		else if (m_reproduce)
		{
			PushState(STATE_Reproduction);
		}
		else
		{
			PushState(STATE_Sleep);
		}
	}

	State(STATE_Reproduction)
	OnEnter
	{
		//Pond un oeuf sur place
		//cout << "Enter Reproduction" << endl;
		NYVert2Df eggPosition;
		eggPosition.X = position.X;
		eggPosition.Y = position.Y;
		GlaceGouille * g = new GlaceGouille(m_world, eggPosition);
		(*m_entities)[GLACEGOUILLE].push_back(g);

		PushState(STATE_FindPath);
	}
	OnExit
	{
		m_reproduce = false;
	}

	State(STATE_Sleep)
	OnUpdate
	{
		//Update des timers
		m_starvationTimer += m_LastUpdateTimer.getElapsedSeconds();
		m_reproductionTimer += m_LastUpdateTimer.getElapsedSeconds();
		m_rotationTimer += m_LastUpdateTimer.getElapsedSeconds();
		m_lifeTimer += m_LastUpdateTimer.getElapsedSeconds();

		//Rotation de la vue
		if (m_rotationTimer >= m_rotationTime)
		{ 
			direction = direction.rotate(NYVert3Df(0, 0, 1), 90);
			m_rotationTimer = 0;
		}

		//Detection des lemmings
		GetCreaturesInSight();
		if (m_creaturesInSight.size() > 0)
		{
			m_targetID = m_creaturesInSight[0]->GetID();
			PushState(STATE_Eat);
		}
		else if (m_starvationTimer >= m_starvationTime || m_lifeTimer > m_lifeTime) //Conditions de mort
		{
			PushState(STATE_Dead);
		}
		else if (m_reproductionTimer >= m_reproductionTime)	//Condition de reproduction
		{
			m_reproduce = true;
			PushState(STATE_FindPath);
		}
	}
	
	State(STATE_Eat)
	OnEnter
	{
		SendMsg(MSG_Eat, m_targetID);
	}
	OnExit
	{
		m_starvationTimer = 0;
	}

	State(STATE_Dead)
	//Override Messages you don't want to receive in this particular state
	OnMsg(MSG_Attack)//i'm already dead, so no one can attack me anymore
	{
		std::cout << "--Entity " << this->GetID() << "-- Get message attack, but i'm already Dead :(" << std::endl;
	}

	OnEnter
		std::cout << "--Entity " << this->GetID() << "-- I'm DEAD." << endl;
	EndStateMachine
}

void GlaceGouille::GetCreaturesInSight()
{
	m_creaturesInSight.clear();
	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		for (int j = 0; j < (*m_entities)[type].size(); ++j)
		{
			if (m_cone.IsInSight((*m_entities)[type][j]->position) && //Si j'ai une entity dans mon champ de vision
				type == LEMMING)
			{
				m_creaturesInSight.push_back((*m_entities)[type][j]);//...On le considère dans le champ de vision
			}
		}
	}

}