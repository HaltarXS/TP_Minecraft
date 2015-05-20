/**
* File : Dahut.h
* Author : Paul Gerst
* Description : Source file of the Dahut AI class
*/

#include "Neon.h"

Neon::Neon(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = NEON;

	//Init position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE + NYCube::CUBE_SIZE / 2.0f + 50;

	//Init pathfinding singleton
	m_pathfinding = Pathfinding::GetSingleton();

	m_entities = new std::map<eTypeCreature, CreatureVector>();
	m_acceleration = NYVert3Df(RandomFloat(-1, 1), RandomFloat(-1, 1), RandomFloat(-1, 1));

	//Init timer
	//m_lastUpdate.start();
}


Neon::~Neon()
{

}

void Neon::SetEntities(std::map<eTypeCreature, CreatureVector> * entities)
{
	m_entities = entities;
}

#pragma region Utils

void Neon::MultiplyVert(NYVert3Df * vect, float n)
{ 
	vect->X *= SEPARATION_WEIGHT;
	vect->Y *= SEPARATION_WEIGHT;
	vect->Z *= SEPARATION_WEIGHT;
}

void Neon::DivideVert(NYVert3Df * vect, float n)
{
	vect->X /= SEPARATION_WEIGHT;
	vect->Y /= SEPARATION_WEIGHT;
	vect->Z /= SEPARATION_WEIGHT;
}

void Neon::LimitVert(NYVert3Df * vect, float limit)
{
	if (vect->X > limit)
		vect->X = limit;
	if (vect->Y > limit)
		vect->Y = limit;
	if (vect->Z > limit)
		vect->Z = limit;

	if (vect->X < -limit)
		vect->X = -limit;
	if (vect->Y < -limit)
		vect->Y = -limit;
	if (vect->Z < -limit)
		vect->Z = -limit;
}

float Neon::DistanceBetween(NYVert3Df vectA, NYVert3Df vectB) const
{
	return sqrt(
		pow(vectA.X - vectB.X, 2) +
		pow(vectA.Y - vectB.Y, 2) +
		pow(vectA.Z - vectB.Z, 2)
	);
}

IABase* Neon::NearestAI(vector<IABase*> ai)
{
	IABase* nearest = nullptr;
	float nearestDist = -1;
	for (int i = 0; i < ai.size(); i++)
	{
		float dist = DistanceBetween(ai.at(i)->position, this->position);
		//float dist = PVector.dist(actorList.get(i).Position, this.Position);
		if (dist < nearestDist || nearestDist < 0)
		{
			nearest = ai.at(i);
			nearestDist = dist;
		}
	}
	return nearest;
}

float Neon::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

#pragma endregion

#pragma region Flocking

NYVert3Df Neon::Steer(NYVert3Df target, bool slowdown)
{
	NYVert3Df steer;
	NYVert3Df desired = target - this->position;
	float mag = desired.getMagnitude();

	if (mag > 0)
	{
		desired.normalize();
		if (slowdown && mag < 100.f)
		{
			MultiplyVert(&desired, MAXSPEED*mag / 100.f);
		}
		else
		{
			MultiplyVert(&desired, MAXSPEED);
		}
		steer = desired - this->m_acceleration;
		LimitVert(&steer, MAXFORCE);
	}
	else
	{
		steer = NYVert3Df(0, 0, 0);
	}

	return steer;
}

void Neon::Flock()
{
	NYVert3Df separation = Separate();
	NYVert3Df alignment = Align();
	NYVert3Df cohesion = Cohesion();
	//NYVert3Df avoidance = Avoidance(NearestAI((*m_entities)[VAUTOUR]));

	MultiplyVert(&separation, SEPARATION_WEIGHT);
	MultiplyVert(&alignment, SEPARATION_WEIGHT);
	MultiplyVert(&cohesion, SEPARATION_WEIGHT);
	//MultiplyVert(&avoidance, SEPARATION_WEIGHT);

	m_acceleration += separation;
	m_acceleration += alignment;
	m_acceleration += cohesion;
	//m_acceleration += avoidance;

	NYVert3Df positionAfterFlock = position + m_acceleration;
	//LimitVert(&positionAfterFlock, NORMALSPEED);
	
	Ground(positionAfterFlock);
	Boundaries(positionAfterFlock);
	
}

void Neon::Seek(NYVert3Df target)
{
	m_acceleration += Steer(target, false);
}

void Neon::Arrive(NYVert3Df target)
{
	m_acceleration += Steer(target, true);
}

NYVert3Df Neon::Separate()
{
	float desiredSeparation = 25.f;
	NYVert3Df steer = NYVert3Df(0, 0, 0);
	int count = 0;

	for (int i = 0; i < (*m_entities)[NEON].size(); i++)
	{
		Neon * other = (Neon *)(*m_entities)[NEON][i];
		float distance = DistanceBetween(this->position, other->position);
		if (distance > 0 && distance < desiredSeparation)
		{
			NYVert3Df diff = this->position - other->position;
			diff.normalize();
			DivideVert(&diff, distance);
			steer += diff;
			count++;
		}
	}

	if (count > 0)
	{
		DivideVert(&steer, count);
	}

	if (steer.getMagnitude() > 0)
	{
		steer.normalize();
		MultiplyVert(&steer, NORMALSPEED);
		steer -= m_acceleration;
		LimitVert(&steer, MAXFORCE);
	}

	return steer;
}

NYVert3Df Neon::Align()
{
	float neighborDist = 50.f;
	NYVert3Df steer = NYVert3Df(0, 0, 0);
	int count = 0;

	for (int i = 0; i < (*m_entities)[NEON].size(); i++)
	{
		Neon * other = (Neon *)(*m_entities)[NEON][i];
		float distance = DistanceBetween(this->position, other->position);
		if (distance > 0 && distance < neighborDist)
		{
			steer += other->m_acceleration;
			count++;
		}
	}

	if (count > 0)
	{
		DivideVert(&steer, count);
	}

	if (steer.getMagnitude() > 0)
	{
		steer.normalize();
		MultiplyVert(&steer, NORMALSPEED);
		steer -= m_acceleration;
		LimitVert(&steer, MAXFORCE);
	}

	return steer;
}

NYVert3Df Neon::Cohesion()
{
	float neighborDist = 50.f;
	NYVert3Df steer = NYVert3Df(0, 0, 0);
	int count = 0;

	for (int i = 0; i < (*m_entities)[NEON].size(); i++)
	{
		Neon * other = (Neon *)(*m_entities)[NEON][i];
		float distance = DistanceBetween(this->position, other->position);
		if (distance > 0 && distance < neighborDist)
		{
			steer += other->position;
			count++;
		}
	}

	if (count > 0)
	{
		DivideVert(&steer, count);
		return Steer(steer, false);
	}

	return steer;
}

NYVert3Df Neon::Avoidance(IABase * predator)
{
	if (DistanceBetween(this->position, predator->position) > AVOIDANCE_DISTANCE)
	{
		return NYVert3Df(0, 0, 0);		// nothing to avoid in the perimeter
	}

	NYVert3Df steer = NYVert3Df(0, 0, 0);

	steer = this->position - predator->position;
	if (steer.getMagnitude() > 0)
	{
		steer.normalize();
		MultiplyVert(&steer, MAXSPEED);
		steer -= m_acceleration;
		LimitVert(&steer, MAXFORCE);
	}

	return steer;
}

void Neon::Boundaries(NYVert3Df position)
{
	if (position.X/NYCube::CUBE_SIZE < 1)
	{
		m_acceleration.X = 1;
	}
	else if (position.X / NYCube::CUBE_SIZE > MAT_SIZE_CUBES - 1)
	{
		m_acceleration.X = -1;
	}

	if (position.Y / NYCube::CUBE_SIZE < 1)
	{
		m_acceleration.Y = 1;
	}
	else if (position.Y / NYCube::CUBE_SIZE > MAT_SIZE_CUBES - 1)
	{
		m_acceleration.Y = -1;
	}

	if (position.Z / NYCube::CUBE_SIZE < 1)
	{
		m_acceleration.Z = 1;
	}
	else if (position.Z / NYCube::CUBE_SIZE > MAT_HEIGHT_CUBES - 1)
	{
		m_acceleration.Z = -1;
	}
}

void Neon::Ground(NYVert3Df position)
{
	NYCubeType type = m_world->getCube((int)round(position.X / NYCube::CUBE_SIZE), (int)round(position.Y / NYCube::CUBE_SIZE), (int)round(position.Z / NYCube::CUBE_SIZE))->_Type;
	if (type != CUBE_AIR)
	{
		m_acceleration.Z = 1;
	}
}

#pragma endregion

void Neon::UpdateIA()
{
	Flock();

	Update();

	m_lastUpdate.start();
}

void Neon::Draw()
{
	//Basic rendering
	glColor3f(0, 0, 255);
	glPushMatrix();
	glTranslatef(position.X, position.Y, position.Z);
	glutSolidCube(NYCube::CUBE_SIZE / 5.f);
	glPopMatrix();

	if (m_path.GetSize() > 0)
		m_path.DrawPath();
}

bool Neon::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

	State(STATE_Initialize)
	OnEnter
	{
		PushState(STATE_Move);
	}

	State(STATE_Move)
	OnEnter
	{
		
	}
	OnUpdate
	{
		m_velocity += m_acceleration;
		LimitVert(&m_velocity, MAXSPEED);

		position += m_velocity;

		m_acceleration = NYVert3Df(0, 0, 0);		
	}

	EndStateMachine
}