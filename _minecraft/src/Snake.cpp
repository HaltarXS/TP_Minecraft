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

	BeginStateMachine

	//MESSAGES
	//Get the messages first in the global scope
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


	//STATES
	//Initialise : ALWAYS MUST BE HERE
	State(STATE_Initialize)
	OnEnter
	std::cout << "--Entity " << this->GetID() << "-- Initialisation " << std::endl;
	PushState(STATE_Move); //Go to STATE_Move state




	//Move
	State(STATE_Move)
	OnUpdate

	if (m_currentState != STATE_Dead){

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
				//bloque le déplacement et choisi une nouvelle direction
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
	//Pour l'instant on fait comme ça, mais faudrait trouver mieux
	if (rand() % 10 == 0){
	NYVert3Df push = m_listPosition[m_listPosition.size() - 1];
	m_listPosition.push_back(push);
	}

	PushState(STATE_Reproduction);

	State(STATE_Sleep)
	OnUpdate
	if (m_timerSleep >= m_sleepDuration)
	{
		//Si un autre monstre est à portée de sa bouche
		PushState(STATE_Eat);
	}
	m_timerSleep += NYRenderer::_DeltaTime;
	//Sleep. Do nothing

	OnExit
	m_timerSleep = 0.0f; //Reinitialisation of the timer when exiting


	State(STATE_Reproduction)
	OnUpdate
	if (m_listPosition.size() >= 10){
		cout << "SNAKE : verification reproduction" << endl;
		Snake * pommeDArgent = new Snake(m_world, NYVert2Df(m_listPosition[5].X / NYCube::CUBE_SIZE, m_listPosition[5].Y / NYCube::CUBE_SIZE), 5);
		pommeDArgent->m_entities = m_entities;
		(*m_entities)[SNAKE].push_back(pommeDArgent);

		//et on détruit les 5 dernières éléments
		for (int i = 0; i < 5; i++)
		m_listPosition.pop_back();
	}
	
	PushState(STATE_Move);


	EndStateMachine
}
