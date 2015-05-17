/*  Furz BastienB */


#ifndef FURZ_H
#define FURZ_H

#include "IABase.h"
#include "Pathfinding.h"
#include "engine\timer.h"

class Furz : public IABase
{
public:
	Furz(NYWorld *pWorld, NYVert2Df pos);
	~Furz();

	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
	virtual void UpdateIA();

private:


	IABase* _target = NULL; // creature que va suivre le furz
	float _lifeTime = 180;//durée de vie en min
	float _fartCounter = 0;//Compteur de pet #jesuiscontentdetrela
	float _numberFartSpawn = 100;//Nombre de pet necessaire pour faire spawn un autre furz
	float _fartFrequency = 1;//Frequence des pets en secondes

	NYTimer	_spentTime;// temps passé vivant
	NYTimer	_timeFartBomb;// temps depuis le dernier pet

	void setTarget(IABase* target);

};

#endif