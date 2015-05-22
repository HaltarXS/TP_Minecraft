#include "Gevaulol.h"

std::map<eTypeCreature, std::vector<IABase*>>* Gevaulol::creatureMap;

Gevaulol::Gevaulol(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld){
	(*creatureMap)[GEVAULOL].push_back(this);
	type = GEVAULOL;
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] + 4;
	position.X = positionCube.X*NYCube::CUBE_SIZE;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;
	speed.X = (rand() % 200) - 100;
	speed.Y = (rand() % 200) - 100;
	speed.Z = (rand() % 200) - 100;
	speed.normalize();
	direction = speed;
	repulsionFactor = 2;
	orientationFactor = 1;
	attractionFactor = 4;
	repulsionRadius = 8;
	orientationRadius = 32;
	attractionRadius = 64;
	maxSpeed = 16;
	m_timer.start();
}

void Gevaulol::UpdateIA(){
	if (m_currentState == STATE_Dead) return; // Stop right there if dead
	float delta(m_timer.getElapsedSeconds());
	m_timer.start();
	position += speed*delta;
	positionCube = position / NYCube::CUBE_SIZE;
	for (auto&& gevaulol : (*creatureMap)[GEVAULOL]) // For all gevaulols
		if (gevaulol != this){ // Except this gevaulol
		float distance((position - gevaulol->position).getSize());
		NYVert3Df toOther(gevaulol->position - position);
		toOther.normalize();
		if (direction.scalProd(gevaulol->direction) > 0){
			if (distance < repulsionRadius) // Repulsion
				speed -= toOther*delta*repulsionFactor;
			else if (distance < orientationRadius) // Orientation
				speed += gevaulol->direction*delta*orientationFactor;
			else if (distance < attractionRadius) // Attraction
				speed += toOther*delta*attractionFactor;
		}
		}
	if (m_world->getCube(positionCube.X, positionCube.Y, positionCube.Z)->isSolid()){ // Collision with cube
		speed = NYVert3Df((rand() % 200) - 100, (rand() % 200) - 100, 50);
		speed.normalize();
		speed *= 8;
	}
	else if (positionCube.X < 0)
		speed.X = -speed.X;
	else if (positionCube.Y < 0)
		speed.Y = -speed.Y;
	else if (positionCube.Z > m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y] + 8) // Too high
		speed.Z = -speed.Z;
	if (speed.getSize() > maxSpeed){ // Cap speed
		speed.normalize();
		speed *= maxSpeed;
	}
	direction = speed; // Compute direction
	direction.normalize();
}
void Gevaulol::Draw(){
	if (m_currentState == STATE_Dead) return; // Stop right there if dead
	glColor3f(255, 0, 128);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 4.0f);
	glPopMatrix();
}
bool Gevaulol::States(StateMachineEvent event, MSG_Object *msg, int state){
	return false;
}