#include "Crabe.h"
#include "Gendamour.h"

Crabe::Crabe(NYWorld* world, NYVert2Df spawnPos)
	: IABase(world)
{
	Initialize();
	type = CRABE;

	NYVert3Df worldSpawnPos = NYVert3Df(spawnPos.X * NYCube::CUBE_SIZE,
		spawnPos.Y * NYCube::CUBE_SIZE,
		(world->_MatriceHeights[(int)spawnPos.X][(int)spawnPos.Y] + 1) * NYCube::CUBE_SIZE);
	// Position init
	this->position = worldSpawnPos;
	this->positionCube = this->position / NYCube::CUBE_SIZE;
	this->Size = NYCube::CUBE_SIZE/2.0f;
	this->Speed = 30.0f;
	this->AxeX = true;
	this->LeftToRight = true;
	this->MaxTimeReprod = 60.0f;
	this->Reproduction = this->MaxTimeReprod;
	this->life = 10;
	this->NextAttack = 0.0f;
	this->TimeBetweenAttack = 1.0f;
	this->m_timer.start();
	this->m_timerAttack.start();
}
Crabe::Crabe(NYWorld* world, NYVert2Df spawnPos,bool axeX)
	: Crabe(world,spawnPos)
{
	this->AxeX = axeX;
}


Crabe::~Crabe()
{

}

void Crabe::UpdateIA(){

	if (m_currentState != STATE_Dead && this->NextAttack <= 0.0f)
	{
		for (int i = 0; i < (*m_entities)[GENDAMOUR].size(); i++)
		{
			NYVert3Df posGend = (*m_entities)[GENDAMOUR][i]->positionCube - this->positionCube;
			if (posGend.X*posGend.X <4 && posGend.Y*posGend.Y <4)
			{
				IABase* ia = (*m_entities)[GENDAMOUR][i];
				if (ia->GetState() != STATE_Dead)
				{
					std::cout << "--Crabe " << this->GetID() << " lance Charge sur Gendamour " << ia->GetID() << " ! Coup critique !" << endl;
					this->SendMsg(MSG_Attack, ia->GetID(), new int(25));
					this->Manger();
					this->NextAttack = TimeBetweenAttack;
					break;
				}
			}
		}
	}
	NextAttack -= m_timerAttack.getElapsedSeconds(true);
	StateMachine::Update();
}

void Crabe::Update(float elapsedTime)
{
	
}

bool Crabe::States(StateMachineEvent event, MSG_Object * msg, int state){

	float elapsedTime = this->m_timer.getElapsedSeconds(true);
	BeginStateMachine

		//MESSAGES
		//Get the messages first in the global scope
		OnMsg(MSG_Attack)//If i'm attacked
	{
		int * data = (int*)msg->GetMsgData();//We get the value in the message.  /!\ If i receive this message, i know that the message data will be an int !
		this->life -= *data;//I remove the value of the message data from my life.
		std::cout << "--Crabe " << this->GetID() << " subit Charge de " << msg->GetSender() << ". C'est super efficace ! " << *data << "dmg, Vie restante : " << this->life << std::endl;
		delete data;//Delete the data

		if (this->life <= 0)//If i don't have any life
		{
			PushState(STATE_Dead);//Use PushState to go in an other state
		}
	}//Message Attack


	//STATES
	//Initialise : ALWAYS MUST BE HERE
	State(STATE_Initialize)
		OnEnter
		std::cout << "--Crabe " << this->GetID() << "-- Initialisation " << std::endl;
	PushState(STATE_Move); //Go to STATE_Move state

	State(STATE_Move)
		OnUpdate
		if (m_currentState != STATE_Dead)
		{
			NYVert3Df oldPos = this->position;
			this->position.X += Speed*AxeX*elapsedTime*(-1 + 2 * LeftToRight);
			this->position.Y += Speed*(!AxeX)*elapsedTime*(-1 + 2 * LeftToRight);
			this->positionCube = this->position / NYCube::CUBE_SIZE;

			this->position.Z = (m_world->_MatriceHeights[(int)this->positionCube.X][(int)this->positionCube.Y] + 1) * NYCube::CUBE_SIZE;
			this->positionCube.Z = this->position.Z / NYCube::CUBE_SIZE;
			if (this->positionCube.Z <= 2 || this->positionCube.X < 0 || this->positionCube.Y < 0 || this->positionCube.X > 100 || this->positionCube.Y > 100)
			{
				if (LeftToRight)
					LeftToRight = false;
				else
					LeftToRight = true;
				this->position = oldPos;
				this->positionCube = this->position / NYCube::CUBE_SIZE;
			}
			this->Reproduction -= elapsedTime;
			if (Reproduction <= 0.0f)
			{
				PushState(STATE_Reproduction);
			}
		}
	State(STATE_Reproduction)
		OnEnter
		if (m_currentState != STATE_Dead)
		{
			if (m_entities)
			{
				if (m_entities->size()!=0)
				{
					if ((*m_entities)[CRABE].size() <= 10)
					{
						Crabe * crabe = new Crabe(m_world, NYVert2Df(this->positionCube.X, this->positionCube.Y), !this->AxeX);
						crabe->m_entities = m_entities;
						(*m_entities)[CRABE].push_back(crabe);
						std:cout << "Encore un qui marchera tout droit toute sa vie. NEW CRABE !" << endl;
					}
				}
			}
			this->Reproduction = this->MaxTimeReprod;
			PushState(STATE_Move);
		}
	//Dead
	State(STATE_Dead)
		//Override Messages you don't want to receive in this particular state
		OnMsg(MSG_Attack)//i'm already dead, so no one can attack me anymore
	{
		std::cout << "--Crabe " << this->GetID() << "-- s'est fait attaqué mais était déjà mort. Respecte les cadavres, merci !" << std::endl;
	}

	OnEnter
		std::cout << "--Crabe " << this->GetID() << "-- Ca y est, je suis mort." << endl;
	m_timerDeath.start();
		

	EndStateMachine
		if (m_currentState == STATE_Dead && m_timerDeath.getElapsedSeconds() > 30.0f )
		{
			std::vector<IABase*> crabes;
			for (int i = 0; i < (*m_entities)[CRABE].size(); i++)
			{
				IABase* crabe = (*m_entities)[CRABE][i];
				if (crabe->GetState() != STATE_Dead)
				{
					crabes.push_back(crabe);
				}
			}
			(*m_entities)[CRABE] = crabes;
			delete this;
		}
}

void Crabe::Draw()
{
	glPushMatrix();
	glTranslatef(this->position.X + this->Size /2, this->position.Y + this->Size/2, this->position.Z - this->Size - 2);
	if (m_currentState != STATE_Dead)
		glColor3f(1.0, 0.0, 0.0);
	else
		glColor3f(0.0, 0.0, 0.0);
	glutSolidCube(this->Size);
	glTranslatef(this->Size/2-1, 2, this->Size - 2);
	glColor3f(200, 200, 200);
	glutSolidCube(1);
	glTranslatef(-this->Size+2, 0,0);
	glColor3f(200, 200, 200);
	glutSolidCube(1);
	glPopMatrix();
}

void Crabe::Manger()
{
	IABase::Manger();
	this->life += 10;
	if (this->AxeX)
		AxeX = false;
	else
		AxeX = true;
}