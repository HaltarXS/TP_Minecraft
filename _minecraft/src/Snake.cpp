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

	//On créer tous les cubes du snake
	for (int i = 0; i < size; i++){
		NYVert3Df posSpawn = NYVert3Df(pos.X*NYCube::CUBE_SIZE + i*NYCube::CUBE_SIZE, pos.Y * NYCube::CUBE_SIZE, (pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] +1)*NYCube::CUBE_SIZE);
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
	OnEnter
	std::cout << "--Entity " << this->GetID() << "-- Entering Moving " << std::endl;
	OnUpdate
	if (m_timerMove >= m_moveDuration)
	{
		PushState(STATE_Eat);
	}
	m_timerMove += NYRenderer::_DeltaTime;

	//When i am in the move state, i try to attack the other entity
	int randomNumber = rand() % 100;
	if (randomNumber > 98)
	{
		int * value = new int(rand() % 20);
		this->SendMsg(MSG_Attack, m_entityToCommunicateWith, value);
		//If a manage to hit the other entity, i send a message telling i'm attacking it :
		// -- The first parameter is the type of the message
		// -- The second parameter is the id of the entity i'm attacking
		// -- The third paramter is the message data : the amount of life i'm removing from it
	}
	OnExit
	m_timerMove = 0.0f; //Reinitialisation of the timer when exiting



	//Eat
	State(STATE_Eat)
	OnEnter
	std::cout << "--Entity " << this->GetID() << "-- Entering Eat " << std::endl;
	OnUpdate
		std::cout << "--Entity " << this->GetID() << "-- I ate something :) " << std::endl;
	PushState(STATE_Move);
	OnExit

	//Dead
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
