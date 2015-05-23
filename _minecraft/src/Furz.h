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


	std::map<eTypeCreature, std::vector<IABase *>> * m_entities;	// All entities of the game

private:

	void setPosition(int , int , int );
	void updatePosition(float );

	NYVert3Df* m_targetPosition;

	IABase* _target = NULL; // creature que va suivre le furz
	float _lifeTime = 180;//durée de vie en min
	float _fartCounter = 0;//Compteur de pet 
	float _numberFartSpawn = 20 + rand() %20;//Nombre de pet necessaire pour faire spawn un autre furz
	float _fartFrequency = 3+rand()%3;//Frequence des pets en secondes
	float _size = 1; //Taille du furz
	float _inflateSize = (14 + rand() % 7)/10;
	int _distanceDeplacement = 0;

	NYVert3Df nextPosition;
	NYTimer _deltaTime;
	NYTimer	_spentTime;// temps passé vivant
	NYTimer	_timeFartBomb;// temps depuis le dernier pet

	void setTarget(IABase* target);

};

#endif