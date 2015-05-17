/**
* Bastien NANCEAU
* - - Parasite
*/

#ifndef PARASITE_H
#define PARASITE_H

#include "engine\timer.h"
#include "IABase.h"
#include "ProximityChecker.h"

class Parasite : public IABase
{
private:

	//Sleeping time, avant l'activation du parasite
	float m_durationSleep = 2.0f;
	float m_timeSleep = 0.0f;

	//Active time, le parasite peut infecter les autres créatures en créant un fils parasite qui ira sur l'hôte cible
	float m_durationReproduction = 3.0f;
	float m_timeReproduction = 0.0f;

	//Zone de détection des créatures proches à infecter
	float m_areaEffect = 5.0f;

	//Si le parasite est né dans une crotte (et non pas par transmission) 
	bool m_isFirstBorn;

	//Timer to determine delta time
	NYTimer m_lastUpdate;


public:

	Parasite(NYWorld *pWorld, NYVert2Df pos, bool firstBorn);
	~Parasite();

	std::vector<IABase*> m_creaturesNear;//Les créatures à proximité

	IABase* target = NULL; //Hôte du parasite
	void InfectCreaturesInArea(float sizeArea);
	void FollowTarget();


	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

#endif
