#include "Crotte.h"


Crotte::Crotte(NYVert3Df position, int maxQuantity) : Ressource(position, maxQuantity)
{
	this->Type = TypeRessource::CROTTE;
}


Crotte::~Crotte()
{
}


void Crotte::Update(float deltaTime){
	this->Use(1);
}


void Crotte::Render(){
	glPushMatrix();
	glTranslatef(this->Position.X + 3, this->Position.Y + 3, this->Position.Z + 1.5);
	glColor3f(115. / 255.f, 30 / 255.f, 15 / 255.f);
	glutSolidCube(3);
	glPopMatrix();
}