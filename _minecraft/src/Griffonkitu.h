/*  Griffonkitu / Martin Lagrange */


#ifndef GRIFFONKITU_H
#define GRIFFONKITU_H

#include "IABase.h"
#include "Pathfinding.h"
#include "engine\timer.h"
typedef std::vector<IABase*> CreatureVector;
class Griffonkitu : public IABase
{
public:
	Griffonkitu(NYWorld *pWorld, NYVert2Df pos);
	~Griffonkitu();

	//truc a override
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
	virtual void UpdateIA();
	void ChangeHuntArea();
	NYTimer	_spentTime;// temps passé sur un spot
	std::map<eTypeCreature, CreatureVector> * m_entities;
	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);
	void LookTarget();

private:
	bool m_huntMod;
	float m_speed;
	NYVert3Df m_direction;
	float m_circleState;
	int m_circleCompleted;
	IABase* _target = NULL; // creature que va suivre le griffon
	void setTarget(IABase* target);
	NYVert3Df m_huntAreaPoint;
	NYVert3Df m_positionFromHuntAreaPoint;
	float m_radius;
    NYVert3Df m_s_up;

};

#endif