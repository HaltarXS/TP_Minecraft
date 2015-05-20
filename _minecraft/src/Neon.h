/**
* File : Neon.h
* Author : Lucas Le Gouic
* Description : Header file of the Neon AI class
*/

#ifndef NEON_H
#define NEON_H

#include "IABase.h"
#include "Pathfinding.h"
#include "Viewcone.h"
#include "engine/timer.h"
#include <math.h>

typedef std::vector<IABase*> CreatureVector;

class Neon : public IABase
{
private:

	// CONST
	const float MAXFORCE = 0.05f;	// Max steering force
	const float MAXSPEED = 0.1f;
	const float NORMALSPEED = 0.1f;
	const float DIGESTIONDURATION = 120.f;		// in seconds

	const float SEPARATION_WEIGHT = 1.3f;
	const float ALIGNMENT_WEIGHT = 1.f;
	const float COHESION_WEIGHT = 1.f;
	const float AVOIDANCE_WEIGHT = 3.f;
	const float AVOIDANCE_DISTANCE = 40.f;

	//Timer to determine delta time
	NYTimer m_lastUpdate;

	//Pathfinding
	Path m_path;
	Pathfinding *m_pathfinding;
	bool m_pathFound;

	// Stomach
	int m_stomachCapacity;
	int m_bugsEaten;
	float m_stomachDigestionTimer;

	// Perceptron
	std::vector<IABase*> m_creaturesInSight;				//Les créatures dans le champ visuel
	std::map<eTypeCreature, CreatureVector> * m_entities;	//Toutes les creatures du jeu

public:
	Neon(NYWorld *pWorld, NYVert2Df pos);
	~Neon();

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);

	//Override base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);

	// Flocking/Boids implementation
public:
	NYVert3Df m_velocity;
	NYVert3Df m_acceleration;

private:
	float s;
	
	NYVert3Df Steer(NYVert3Df target, bool slowdown);
	void Flock();
	void Seek(NYVert3Df target);
	void Arrive(NYVert3Df target);

	NYVert3Df Separate();
	NYVert3Df Align();
	NYVert3Df Cohesion();
	NYVert3Df Avoidance(IABase * predator);

	void Boundaries(NYVert3Df position);

	void Ground(NYVert3Df position);

	// Utils
private:
	void MultiplyVert(NYVert3Df * vect, float n);
	void DivideVert(NYVert3Df * vect, float n);
	void LimitVert(NYVert3Df * vect, float limit);
	float DistanceBetween(NYVert3Df vectA, NYVert3Df vectB) const;
	IABase * NearestAI(vector<IABase*> ai);
	float RandomFloat(float a, float b);
};

#endif