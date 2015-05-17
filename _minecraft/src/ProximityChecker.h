#pragma once

#include "IABase.h"

typedef std::vector<IABase*> CreatureVector;

class ProximityChecker
{

private:

	//Singleton instance
	static ProximityChecker* s_pcInstance;

	std::map<eTypeCreature, CreatureVector> * m_entities;//Toutes les creatures du jeu

	float getSquarredDistance(NYVert3Df p1, NYVert3Df p2);

public:
	static ProximityChecker * GetSingleton();

	vector<IABase*> checkParasiteProximityCreatures(Parasite* callerEntity, float sizeArea);

};

