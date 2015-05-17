/**
* Author : Guillaume Dor
*/

#ifndef MOUCHE_H
#define MOUCHE_H

#include "engine\timer.h"
#include "IABase.h"
#include "Crotte.h"
#include "RessourcesManager.h"

typedef std::vector<IABase*> CreatureVector;

class Mouche : public IABase
{
	private:
		NYTimer _lastUpdate; //Timer to determine delta time
		
		Crotte* _poop;
		float _rebornTime;
		float _rebornTimer;
		bool _isAlive;
		bool _wasKilled;
		NYVert3Df _color;
		float _size;
		float _movementSpeedX;
		float _movementSpeedY;

		void UpdatePosition();

	public:
		static const int MAX_MOUCHE = 10;

		Mouche(NYWorld *pWorld, NYVert2Df pos);
		~Mouche();

		//Override base class methods
		virtual void UpdateIA();
		virtual void Draw();
		virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);

		NYVert3Df GetColor();
		float GetSize();
};

#endif