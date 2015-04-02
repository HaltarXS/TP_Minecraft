#include "Herbe.h"


Herbe::Herbe(NYVert3Df position, int maxQuantity) : Ressource(position, maxQuantity)
{
	this->Type = TypeRessource::HERBE;
}


Herbe::~Herbe()
{
}


void Herbe::Update(float deltaTime){
	this->Use(1);
}


void Herbe::Render(){
	glPushMatrix();
	glTranslatef(this->Position.X+5, this->Position.Y+5, this->Position.Z+2.5);
	glColor3f(0.0, 1.0, 0.0);
	glutSolidCube(5);
	glPopMatrix();
}