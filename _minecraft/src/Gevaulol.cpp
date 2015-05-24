#include "Gevaulol.h"

std::map<eTypeCreature, std::vector<IABase*>>* Gevaulol::creatureMap;
const float Gevaulol::breedingWaitTime(64), Gevaulol::breedingRadius(NYCube::CUBE_SIZE);

Gevaulol::Gevaulol(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld){
	(*creatureMap)[GEVAULOL].push_back(this);
	type = GEVAULOL;
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y] + 4;
	position.X = positionCube.X*NYCube::CUBE_SIZE;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;
	speed.X = randFloat();
	speed.Y = randFloat();
	speed.Z = randFloat();
	speed.normalize();
	direction = speed;
	repulsionFactor = 2;
	orientationFactor = 1;
	attractionFactor = 4;
	attackFactor = 4;
	repulsionRadius = NYCube::CUBE_SIZE;
	orientationRadius = 4 * NYCube::CUBE_SIZE;
	attractionRadius = 8 * NYCube::CUBE_SIZE;
	perceptionRadius = 24 * NYCube::CUBE_SIZE;
	attackRadius = 2 * NYCube::CUBE_SIZE;
	maxSpeed = 3 * NYCube::CUBE_SIZE;
	m_timer.start();
	m_lastbreeding.start();
}

void Gevaulol::breed(Gevaulol& a, Gevaulol& b){
	a.m_lastbreeding.start();
	b.m_lastbreeding.start();
	NYVert3Df center((a.positionCube + b.positionCube) / 2);
	Gevaulol* gevaulol(new Gevaulol(a.m_world, NYVert2Df(center.X, center.Y)));
	gevaulol->attackRadius = mutate(a.attackRadius, b.attackRadius);
	gevaulol->attractionFactor = mutate(a.attractionFactor, b.attractionFactor);
	gevaulol->attractionRadius = mutate(a.attractionRadius, b.attractionRadius);
	gevaulol->maxSpeed = mutate(a.maxSpeed, b.maxSpeed);
	gevaulol->orientationFactor = mutate(a.orientationFactor, b.orientationFactor);
	gevaulol->orientationRadius = mutate(a.orientationRadius, b.orientationRadius);
	gevaulol->perceptionRadius = mutate(a.perceptionRadius, b.perceptionRadius);
	gevaulol->repulsionFactor = mutate(a.repulsionFactor, b.repulsionFactor);
	gevaulol->repulsionRadius = mutate(a.repulsionRadius, b.repulsionRadius);
}
float Gevaulol::mutate(float a, float b){
	if (randFloat() < 0.f){ // Half of the time
		float w(abs(randFloat()));
		return a*w + b*(1.f - w); // Random interpolation
	}
	else{
		float f(1 + randFloat()*.25f); // 25% max modification
		return f*((a + b) / 2); // Take average as starting value
	}
}

void Gevaulol::UpdateIA(){
	if (GetState() == STATE_Dead) return; // Stop right there if dead
	float delta(m_timer.getElapsedSeconds());
	m_timer.start();
	position += speed*delta;
	positionCube = position / NYCube::CUBE_SIZE;
	for (int i(0); i<(*creatureMap)[GEVAULOL].size(); i++){ // For all gevaulols
		Gevaulol* gevaulol((Gevaulol*)(*creatureMap)[GEVAULOL][i]);
		if (gevaulol != this && gevaulol->GetState() != STATE_Dead){ // Except this gevaulol and dead gevaulols
			float distance((position - gevaulol->position).getSize());
			NYVert3Df toOther(gevaulol->position - position);
			toOther.normalize();
			if (direction.scalProd(toOther) > -.5f){ // Can't see behind (duh)
				if (distance < repulsionRadius) // Repulsion
					speed -= toOther*delta*repulsionFactor;
				else if (distance < orientationRadius) // Orientation
					speed += gevaulol->direction*delta*orientationFactor;
				else if (distance < attractionRadius) // Attraction
					speed += toOther*delta*attractionFactor;
			}
			if (canBreed() && gevaulol->canBreed())
				breed(*this, *gevaulol);
		}
	}
	IABase* targetWastedosaure(NULL);
	for (int i(0); i < (*creatureMap)[WASTEDOSAURE].size(); i++){ // For all wastedosaures
		IABase* wastedosaure((*creatureMap)[WASTEDOSAURE][i]);
		if (wastedosaure->GetState() == STATE_Dead) continue; // Don't care about dead wastedosaures
		NYVert3Df toWastedosaure(wastedosaure->position - position);
		float distSq(toWastedosaure.getMagnitude());
		if (distSq < attackRadius*attackRadius){
			targetWastedosaure = wastedosaure;
			break;
		}
		else if (distSq < perceptionRadius*perceptionRadius){
			toWastedosaure.normalize();
			speed += toWastedosaure*delta * attackFactor;
		}
	}
	if (targetWastedosaure)
		SendMsg(MSG_Attack, targetWastedosaure->GetID(), new int(1));
	if (positionCube.X < 0)
		speed.X = abs(speed.X);
	else if (positionCube.X >= MAT_SIZE_CUBES)
		speed.X = -abs(speed.X);
	else if (positionCube.Y < 0)
		speed.Y = abs(speed.Y);
	else if (positionCube.Y >= MAT_SIZE_CUBES)
		speed.Y = -abs(speed.Y);
	else if (positionCube.Z < m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y] + 2) // Too low
		speed.Z = abs(speed.Z);
	else if (positionCube.Z > m_world->_MatriceHeights[(int)positionCube.X][(int)positionCube.Y] + 8) // Too high
		speed.Z = -abs(speed.Z);
	if (speed.getSize() > maxSpeed){ // Cap speed
		speed.normalize();
		speed *= maxSpeed;
	}
	direction = speed; // Compute direction
	direction.normalize();

	Update(); // Update state machine
}
void Gevaulol::Draw(){
	if (GetState() == STATE_Dead) return; // Stop right there if dead
	glColor3f(255, 0, 128);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 8.0f);
	glPopMatrix();
}
bool Gevaulol::States(StateMachineEvent event, MSG_Object *msg, int state){
	BeginStateMachine

		//Receive attack
		OnMsg(MSG_Attack)
	{
		int *pData = (int*)msg->GetMsgData();
		life -= *pData;
		delete pData;

		if (life <= 0)
		{
			PushState(STATE_Dead);
		}
	}
	State(STATE_Initialize)

		State(STATE_Dead)

		EndStateMachine
}