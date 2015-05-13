#include "Wastedosaure.h"


Wastedosaure::Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale):
IABase(_world), m_cone(90.0f,100)
{
	Initialize();
	type = WASTEDOSAURE;
	position = NYVert3Df(_positionInitiale.X * NYCube::CUBE_SIZE, _positionInitiale.Y*NYCube::CUBE_SIZE, _world->_MatriceHeights[(int)_positionInitiale.X][(int)_positionInitiale.Y] * NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f);

}

Wastedosaure::~Wastedosaure()
{

}

void Wastedosaure::SetEntities(std::vector<IABase*> * entities)
{
	m_entities = entities;
}

void Wastedosaure::GetCreaturesInSight()
{
	m_creaturesInSight.clear();
	for (int i = 0; i < m_entities->size(); ++i)
	{
		if (m_cone.IsInSight((*m_entities)[i]->position) && (*m_entities)[i]->GetID() != this->GetID())
		{
			m_creaturesInSight.push_back((*m_entities)[i]);
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

void Wastedosaure::UpdateIA()
{
	//Update Cone de vision
	m_cone.SetPosition(position);
	m_cone.SetOrientation(direction);

	GetCreaturesInSight();

	/*for (int i = 0; i < m_creaturesInSight.size(); ++i)
	{
		cout << "Type " << m_creaturesInSight[i]->type << " ID : " << m_creaturesInSight[i]->GetID() << ".\n";
	}*/

	Update();//Update the state machine
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
	glColor3f(255, 0, 0);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	if (m_currentState == STATE_Egg)
	{
		glutSolidSphere(5, 20, 20);
	}
	else
	{
		glutSolidCube(8);
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
	WastedosaureManager::GetSingleton()->AssignToAGroup(this);
		
	//Find path
	State(STATE_FindPath)
	OnEnter
	m_timerTryFindPath = 0.0f;
	m_path.Clear();
	if (leader == NULL)
	{
		m_pf->FindPath(NYVert2Df(position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), NYVert2Df(rand() % MAT_SIZE_CUBES, rand() % MAT_SIZE_CUBES), 1, m_path);
	}
	else
	{
		m_path = leader->GetPath(); //On va suivre le chemin du leader pour éviter de faire 1 pf/créature
		
	}
	
	m_currentIndex = FindClosestWaypoint(m_path);
	
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
	


	//Dead
	State(STATE_Dead)
	
	OnEnter


	EndStateMachine
}