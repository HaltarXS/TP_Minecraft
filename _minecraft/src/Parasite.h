/**
* Bastien NANCEAU
* - - Parasite
*/

#ifndef PARASITE_H
#define PARASITE_H

#include "engine\timer.h"
#include "IABase.h"

typedef std::vector<IABase*> CreatureVector;

class Parasite : public IABase
{
private:

	//Sleeping time, avant l'activation du parasite
	float m_durationSleep = 40.0f;
	float m_timeSleep = 0.0f;

	//Active time, le parasite peut infecter les autres créatures en créant un fils parasite qui ira sur l'hôte cible
	float m_durationReproduction = 60.0f;
	float m_timeReproduction = 0.0f;

	//Timer de détection des animaux à proximité
	float m_durationCheckProximity = 1.0f;
	float m_timeCheckProximity = 0.0f;

	//Timer de détection des animaux à proximité
	float m_durationCheckCrotte = 1.0f; //Délay dans la vérification des crottes
	float m_timeCheckCrotte = 0.0f;

	//Zone de détection des créatures proches à infecter, en nombre de cases (case courrante du virus incluse)
	float m_areaEffect = 3.0f;

	//Si le parasite est né dans une crotte (et non pas par transmission) 
	bool m_isFirstBorn = false;

	//Timer to determine delta time
	NYTimer m_lastUpdate;

	IABase* m_target = NULL; //Hôte du parasite

public:

	Parasite(NYWorld *pWorld, NYVert3Df pos, bool firstBorn);
	~Parasite();

	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);

	void InfectCreaturesInArea(float sizeArea);
	void FollowTarget();
	float getSquarredDistance(NYVert3Df* p1, NYVert3Df* p2);


	bool m_isSpawner = false;
	void checkCrottesSpanw();

	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

#endif
