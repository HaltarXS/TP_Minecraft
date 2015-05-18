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

	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *msg, int state);
	virtual void UpdateIA();
	void ChangeHuntArea();
	// Temps de vol en mode observation
	NYTimer	_spentTime;
	// temps total
	NYTimer	_Time;
	//liste des entitées
	std::map<eTypeCreature, CreatureVector> * m_entities;
	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);
	//Algorithme qui sert à trouver les cubes sur la trajectoir de la ligne. Je l'utilise l'orsque le griffon à détécté une cible potentiel.
	bool BresenhamRayCast(int x1, int y1, int z1, const int x2, const int y2, const int z2);

private:
	// coordonée monde X
	int m_XWorldMap;
	// coordonée monde Y
	int m_YWorldMap;
	// flag qui indique si le griffon fait des cercles avant d'attaquer
	bool m_observationMod;
	// flag qui indique si le griffon attaque
	bool m_attack = false;
	// flag pour la couleur
	bool m_obsColor = false;
	// flag pour savoir si le griffon à déjà fait une inspection au moin une fois
	bool m_hasLooked = false;
	float m_speed;
	NYVert3Df m_direction;

	IABase* m_target = NULL; // creature que va suivre le griffon
	void setTarget(IABase* target);
	// Centre autour duquel va tourner le griffon en mode observation
	NYVert3Df m_huntAreaPoint;
	// position à partir du référenciel autour duquel va tourner le griffon en mode observation
	NYVert3Df m_positionFromHuntAreaPoint;
	// position de la cible
	NYVert3Df m_positionTarget;
	// Rayon du cercle dans lequel tourne le griffon en mode observation
	float m_radius;
	//Vecteur Up
	NYVert3Df m_s_up;
	// pathFinding de guillaume (pas utilisé pour le griffon mais je garde la référence)
	Pathfinding * m_pf = Pathfinding::GetSingleton();
	Path m_path;
	int m_currentIndex = 0;

};

#endif