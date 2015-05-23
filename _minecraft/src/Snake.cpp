/**
* File : Snake.cpp
* Author : Vincent Swaenepoel
* Description : Source file of the Snake AI class
*/

#include "Snake.h"


Snake::Snake(NYWorld *pWorld, NYVert2Df pos, int size = 5) : IABase(pWorld)
{
	Initialize();

	type = SNAKE;

	m_size = size;
	world = pWorld;

	//On créer tous les cubes du snake
	for (int i = 0; i < size; i++){
		NYVert3Df posSpawn = NYVert3Df((pos.X+0.5f)*NYCube::CUBE_SIZE + i*NYCube::CUBE_SIZE, (pos.Y+0.5f) * NYCube::CUBE_SIZE, (pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] + 1)*NYCube::CUBE_SIZE);
		m_listPosition.push_back(posSpawn);
	}
}


Snake::~Snake()
{
}

void Snake::UpdateIA(){

	Update();
}

//Mange les monstres à une case de sa tête
bool Snake::Proximity(NYVert3Df food){
	return sqrt(pow(food.X - m_listPosition[0].X, 2) + pow(food.Y - m_listPosition[0].Y, 2) + pow(food.Z - m_listPosition[0].Z, 2)) < NYCube::CUBE_SIZE + 1;
}

void Snake::Draw(){

	glColor3f(255, 255, 255);
	glPushMatrix();
	
	for (int i = 0; i < m_listPosition.size(); i++){
		if (i == 0){
			glTranslatef(m_listPosition[i].X, m_listPosition[i].Y, m_listPosition[i].Z);
		}
		else{
			glTranslatef(m_listPosition[i].X - m_listPosition[i - 1].X, m_listPosition[i].Y - m_listPosition[i - 1].Y, m_listPosition[i].Z - m_listPosition[i - 1].Z);
		}
		glutSolidCube(NYCube::CUBE_SIZE);
	}


	glPopMatrix();
}

bool Snake::States(StateMachineEvent event, MSG_Object * msg, int state){

	//Etats : Move -> Sleep -> Eat -> Reproduction -> Move -> ...

	BeginStateMachine

	//MESSAGES
	OnMsg(MSG_Attack)//Si le snake se fait attaquer par un vautour, il perd un bloque jusqu'à arriver à zéro
	{
		int * data = (int*)msg->GetMsgData();
		
		m_listPosition.pop_back();

		std::cout << "--Entity SNAKE " << this->GetID() << "-- Attack from entity " << msg->GetSender() << ". Life remaining : " << m_listPosition.size() << std::endl;
		delete data;//Delete the data

		if (m_listPosition.size() <= 0 ) // à zéro bloque, le snake meurt
		{
			PushState(STATE_Dead); //il sert plus à rien
		}
	}//Message Attack


	//STATES
	//Initialise : ALWAYS MUST BE HERE
	State(STATE_Initialize)
	OnEnter
	std::cout << "--Entity SNAKE : " << this->GetID() << "-- Initialisation " << std::endl;
	PushState(STATE_Move); //Go to STATE_Move state




	//Move
	State(STATE_Move)
	OnUpdate

	if (m_currentState != STATE_Dead){

		//Tant que le snake n'est pas mort il choisi une position aléatoire
		int direction = rand() % 4;
		NYVert3Df newPosition;
		bool isValideMove = true;

		if (direction == 0)
			newPosition = NYVert3Df(m_listPosition[0].X + NYCube::CUBE_SIZE, m_listPosition[0].Y, (world->_MatriceHeights[(int)(m_listPosition[0].X + NYCube::CUBE_SIZE) / NYCube::CUBE_SIZE][(int)m_listPosition[0].Y / NYCube::CUBE_SIZE] + 0.5f)*NYCube::CUBE_SIZE);
		else if (direction == 1)
			newPosition = NYVert3Df(m_listPosition[0].X - NYCube::CUBE_SIZE, m_listPosition[0].Y, (world->_MatriceHeights[(int)(m_listPosition[0].X - NYCube::CUBE_SIZE) / NYCube::CUBE_SIZE][(int)m_listPosition[0].Y / NYCube::CUBE_SIZE] + 0.5f)*NYCube::CUBE_SIZE);
		else if (direction == 2)
			newPosition = NYVert3Df(m_listPosition[0].X, m_listPosition[0].Y + NYCube::CUBE_SIZE, (world->_MatriceHeights[(int)m_listPosition[0].X / NYCube::CUBE_SIZE][(int)(m_listPosition[0].Y + NYCube::CUBE_SIZE) / NYCube::CUBE_SIZE] + 0.5f)*NYCube::CUBE_SIZE);
		else
			newPosition = NYVert3Df(m_listPosition[0].X, m_listPosition[0].Y - NYCube::CUBE_SIZE, (world->_MatriceHeights[(int)m_listPosition[0].X / NYCube::CUBE_SIZE][(int)(m_listPosition[0].Y - NYCube::CUBE_SIZE) / NYCube::CUBE_SIZE] + 0.5f)*NYCube::CUBE_SIZE);

		for (int i = 0; i < m_listPosition.size(); i++){
			if (newPosition == m_listPosition[i] && isValideMove){
				isValideMove = false;
				//bloque le déplacement et choisi une nouvelle direction si le snake veut se mordre la queue
			}
		}

		if (isValideMove)
		{
			

			for (int i = m_listPosition.size() - 1; i >= 0; i--){
				if (i > 0)
					m_listPosition[i] = m_listPosition[i - 1];
				else
					m_listPosition[i] = newPosition;
			}
			PushState(STATE_Sleep);
		}
	}

	//Eat
	State(STATE_Eat)
	OnUpdate

	for (int i = 0; i < CREATURE_NUM; ++i)
	{
		eTypeCreature type = (eTypeCreature)i;
		if (type != eTypeCreature::SNAKE) {
			for (int j = 0; j < (*m_entities)[type].size(); ++j)
			{
				//On parcourt la liste des créatures.

				//Si la créature à proximité n'est pas un snake, on la mange
				if (Proximity((*m_entities)[type][j]->position)) {
					//TODO: vérifier si ça casse pas le code des autres
					//(*m_entities).erase(advance((*m_entities).begin(),j));

					//Et on grandi
					NYVert3Df push = m_listPosition[m_listPosition.size() - 1];
					m_listPosition.push_back(push);

					//faire caca
					RessourcesManager *pRessourceMgr = RessourcesManager::GetSingleton();
					m_listPosition.back().X;
					*pRessourceMgr->Create(CROTTE, NYVert3Df(m_listPosition.back().X, m_listPosition.back().Y, m_listPosition.back().Z), 1000);
				}
			}
		}
	}

	PushState(STATE_Reproduction);

	State(STATE_Sleep)
	OnUpdate
	if (m_timerSleep >= m_sleepDuration)
	{
		PushState(STATE_Eat);
	}
	m_timerSleep += NYRenderer::_DeltaTime;

	OnExit
	m_timerSleep = 0.0f;


	State(STATE_Reproduction)
	OnUpdate
	if (m_listPosition.size() >= 10){
		if ((*m_entities)[SNAKE].size() <= 20)
		{
			//Si un snake est trop grand, il se coupe en deux
			cout << "SNAKE : verification reproduction" << endl;
			Snake * pommeDArgent = new Snake(m_world, NYVert2Df(m_listPosition[5].X / NYCube::CUBE_SIZE, m_listPosition[5].Y / NYCube::CUBE_SIZE), 5);
			pommeDArgent->m_entities = m_entities;
			(*m_entities)[SNAKE].push_back(pommeDArgent);

			//et on détruit les 5 dernières éléments
			for (int i = 0; i < 5; i++)
				m_listPosition.pop_back();
		}
	}
	
	PushState(STATE_Move);


	EndStateMachine
}
