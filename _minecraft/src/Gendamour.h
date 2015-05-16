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
		void setTarget(IABase* target);

};

#endif