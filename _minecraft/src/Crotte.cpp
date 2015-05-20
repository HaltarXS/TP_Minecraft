#include "Crotte.h"


Crotte::Crotte(NYVert3Df position, int maxQuantity) : Ressource(position, maxQuantity)
{
	this->Type = TypeRessource::CROTTE;

	_hasFly = false;
}


Crotte::~Crotte()
{
}


void Crotte::Update(float deltaTime){
	this->Use(1);
}


void Crotte::Render()
{
	glPushMatrix();
	glTranslatef(this->Position.X + NYCube::CUBE_SIZE / 2.0f, this->Position.Y + +NYCube::CUBE_SIZE / 2.0f, this->Position.Z + CROTTE_SIZE / 2.0f);
	glColor3f(115. / 255.f, 30 / 255.f, 15 / 255.f);
	glutSolidCube(CROTTE_SIZE * 1);
	glPopMatrix();
}

bool Crotte::GetHasFly()
{
	return _hasFly;
}

void Crotte::SetHasFly(bool val)
{
	_hasFly = val;
}

bool Crotte::GetHasParasite()
{
	return m_hasParasite;
}

void Crotte::SetHasParasite(bool val)
{
	m_hasParasite = val;
}