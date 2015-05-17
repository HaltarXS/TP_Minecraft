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
	bool BresenhamRayCast(int x1, int y1, int z1, const int x2, const int y2, const int z2);

private:
	bool m_huntMod;
	bool m_redColor = false;
	bool m_hasLooked = false;
	float m_speed;
	NYVert3Df m_direction;
	float m_circleState;
	int m_circleCompleted;
	IABase* m_target = NULL; // creature que va suivre le griffon
	void setTarget(IABase* target);
	NYVert3Df m_huntAreaPoint;
	NYVert3Df m_positionFromHuntAreaPoint;
	NYVert3Df m_positionTarget;
	float m_radius;
    NYVert3Df m_s_up;
	Pathfinding * m_pf = Pathfinding::GetSingleton();
	Path m_path;
	int m_currentIndex = 0;
	bool m_attack = false;

};

#endif