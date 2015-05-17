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

	//Active time, le parasite peut infecter les autres cr�atures en cr�ant un fils parasite qui ira sur l'h�te cible
	float m_durationReproduction = 3.0f;
	float m_timeReproduction = 0.0f;

	//Zone de d�tection des cr�atures proches � infecter
	float m_areaEffect = 5.0f;

	//Si le parasite est n� dans une crotte (et non pas par transmission) 
	bool m_isFirstBorn;

	//Timer to determine delta time
	NYTimer m_lastUpdate;


public:

	Parasite(NYWorld *pWorld, NYVert2Df pos, bool firstBorn);
	~Parasite();

	std::vector<IABase*> m_creaturesNear;//Les cr�atures � proximit�

	IABase* target = NULL; //H�te du parasite
	void InfectCreaturesInArea(float sizeArea);
	void FollowTarget();


	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

#endif
