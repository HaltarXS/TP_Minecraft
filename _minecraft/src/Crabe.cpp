#include "Crabe.h"
#include "Gendamour.h"

Crabe::Crabe(NYWorld* world, NYVert2Df spawnPos)
	: IABase(world)
{
	type = CRABE;

	NYVert3Df worldSpawnPos = NYVert3Df(spawnPos.X * NYCube::CUBE_SIZE,
		spawnPos.Y * NYCube::CUBE_SIZE,
		(world->_MatriceHeights[(int)spawnPos.X][(int)spawnPos.Y] + 1) * NYCube::CUBE_SIZE);
	// Position init
	this->position = worldSpawnPos;
	this->positionCube = this->position / NYCube::CUBE_SIZE;
	this->Size = NYCube::CUBE_SIZE/2.0f;
	this->Speed = 10.0f;
	this->AxeX = true;
	this->LeftToRight = true;
	this->MaxTimeReprod = 60.0f;
	this->Reproduction = this->MaxTimeReprod;
	this->m_timer.start();
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
	Update(this->m_timer.getElapsedSeconds(true));
	std::vector<IABase*> gendamours;

	for (int i = 0; i < (*m_entities)[GENDAMOUR].size(); i++)
	{
		if ((*m_entities)[GENDAMOUR][i]->positionCube == this->positionCube)
		{
			(*m_entities)[GENDAMOUR][i]->life = 0.0f;
			this->Manger();
			(*m_entities)[GENDAMOUR][i]->~IABase();
		}
		else
		{
			gendamours.push_back((*m_entities)[GENDAMOUR][i]);
		}
	}
	(*m_entities)[GENDAMOUR] = gendamours;
}

void Crabe::Update(float elapsedTime)
{
	this->Reproduction -= elapsedTime;
	if (Reproduction <= 0.0f)
	{
		Crabe * crabe = new Crabe(m_world, NYVert2Df(this->positionCube.X,this->positionCube.Y),!this->AxeX);
		crabe->m_entities = m_entities;
		(*m_entities)[CRABE].push_back(crabe);
		this->Reproduction = this->MaxTimeReprod;
	}
	NYVert3Df oldPos = this->position;
	this->position.X += Speed*AxeX*elapsedTime*(-1 + 2 *LeftToRight);
	this->position.Y += Speed*(!AxeX)*elapsedTime*(-1 + 2 *LeftToRight);
	this->positionCube = this->position / NYCube::CUBE_SIZE;
	
	this->position.Z = (m_world->_MatriceHeights[(int)this->positionCube.X][(int)this->positionCube.Y] + 1) * NYCube::CUBE_SIZE;
	this->positionCube.Z = this->position.Z / NYCube::CUBE_SIZE;
	if (this->positionCube.Z <= 2)
	{
		if (LeftToRight)
			LeftToRight = false;
		else
			LeftToRight = true;
		this->position = oldPos;
		this->positionCube = this->position / NYCube::CUBE_SIZE;
	}
}

bool Crabe::States(StateMachineEvent event, MSG_Object * msg, int state){
	return false;
}

void Crabe::Draw()
{
	glPushMatrix();
	glTranslatef(this->position.X + this->Size /2, this->position.Y + this->Size/2, this->position.Z - this->Size - 2);
	glColor3f(1.0, 0.0, 0.0);
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
	if (this->AxeX)
		AxeX = false;
	else
		AxeX = true;
}