//MADE BY GUILLAUME PASTOR
#include "TestStateMachine.h"

TestStateMachine::TestStateMachine() :
IABase(NULL)
{
	//State Machine Initialisation. Important ! A faire ici, pas dans IABase
	Initialize();

	//On initialise le type
	type = TEST_STATEMACHINE;
}

//Destructor
TestStateMachine::~TestStateMachine()
{

}

void TestStateMachine::Draw()
{

}

//It hase to be called every frame
void TestStateMachine::UpdateIA()
{
	this->Update();//Update the state machine ("State" method)

	//Do the other updates

}

//For test purpose. 
//In the end you will be able to communicate with all the entities in your sight. 
//You going to need a vector of entities in your sight, then, get the id of one of the entities to communicate with it.
void TestStateMachine::SetEntityToCommunicateWith(objectID OtherEntity)
{
	m_entityToCommunicateWith = OtherEntity;
}


/* STATES
* 
* State have the following form
*
* State(STATE_NAME)
*
* OnEnter //Called once when entering the state
*
* OnUpdate //Called every frame while you are in the state
*
* OnExit //Called once when exiting the state
*
* --------------------------------------------------
*
* But if you don't need an event when you are entering, exiting or updating a state, you can remove the "OnXXX" you want.
*
* For example look at the state STATE_Initialize down below which don't need the OnUpdate and the OnExit event.
*
*
*
* To Change a state, use the method "PushState(STATENAME)".
*
*
* You can ADD A STATE in the STATE_FSM enum in statemch.h
*
*
* ----------------------------------------------------------------------------------------------
*
* MESSAGES
*
* You can send messages to other entities thanks to the Method SendMsg() . Look at the state STATE_Move for an example.
*
* You get the messages by using the method OnMsg(MSG_NAME), then, make a scope where you will do the specific treatment for your message. See OnMsg(MSG_Attack) down below.
*
* You can override message reception in a certain state. For example, if i'm dead, i won't receive any message anymore; see the reception of the MSG_Attack in State(STATE_Dead) down below.  
*
* From a message, you can get its sender's ID by using the method GetSender() : see how it works in OnMsg(MSG_Attack) down below
*
* From a message, you can get its receiver's ID by using the method GetReceiver()
*
* From a message, you can get its data by using the method GetMsgData(). /!\ It's a void *; you have to know what data is in the message to cast it : see how it works in OnMsg(MSG_Attack) down below
*
* You can ADD A MESSAGE in the MSG_Name enum in msg.h
*
*
*/
bool TestStateMachine::States(StateMachineEvent event, MSG_Object * msg, int state)
{
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
	PushState(STATE_Sleep);
	OnExit



	//Sleep
	State(STATE_Sleep)
	OnEnter
	std::cout << "--Entity " << this->GetID() << "-- Entering Sleep " << std::endl;

	OnUpdate
	if (m_timerSleep >= m_sleepDuration)
	{
		PushState(STATE_Move);
	}
	m_timerSleep += NYRenderer::_DeltaTime;
	//Sleep. Do nothing

	OnExit
	m_timerSleep = 0.0f; //Reinitialisation of the timer when exiting



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