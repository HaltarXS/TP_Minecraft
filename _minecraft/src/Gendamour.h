/*  Gendamour / Leroy Nicolas */


#ifndef GENDAMOUR_H
#define GENDAMOUR_H

#include "IABase.h"
#include "Pathfinding.h"
#include "engine\timer.h"

class Gendamour : public IABase
{
	public:
		Gendamour(NYWorld *pWorld, NYVert2Df pos);
		~Gendamour();

		//Pathfinding singleton
		int m_pathIndex;
		Path m_path;
		Pathfinding *m_pPathfinder;
		NYVert3Df m_targetPosition;
		std::map<eTypeCreature, std::vector<IABase *>> * m_entities;// All entities of the game

		//Timer to determine delta time
		NYTimer m_lastUpdate;

		//truc a override
		virtual void Draw();
		virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
		virtual void UpdateIA();

	private:
		IABase* _target = NULL; // creature que va suivre le gendamour
		float _lifeTime = 15;//durée de vie en min
		float _reproductionTime = 1;//temps en minute qu'il faut passé avec un autre gendamour pour se reproduire
		NYTimer	_spentTime ;// temps passé vivant
		NYTimer	_spentTimeTogether;//temps passé avec un autre gendamour
		NYTimer _researhTime; //temps avant la prochaine recherche
		void setTarget(IABase* target);

};

#endif