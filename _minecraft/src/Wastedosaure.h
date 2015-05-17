//Wastedosaure- Guillaume Pastor
#pragma once

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"
#include <iostream>
#include "IABase.h"

#include "world.h"

#include "Pathfinding.h"

#include "WastedosaureManager.h"

#include "Viewcone.h"

//using namespace std;

typedef std::vector<IABase*> CreatureVector;
#define MAX_WASTEDOSAURE 20

class Wastedosaure :public IABase
{
private:

	//-----Variables-----//

	//Wastedosaure Variables
	float m_speed = 45.0f + 0.05f*GetID(); //Vitesse
	float m_minSize = 2.0f;					 //Taille minimale
	float m_maxSize = 9.0f;					 //Taille maximale
	float m_currentSize = m_minSize;			 //Taille courante
	float m_durationGrow = 50.0f;				 //Temps de croissance pour arriver à l'age adulte
	float m_timerGrow = 0.0f;					 //Timer de croissance
	bool  m_isMale = false;				 //Sexe

	//View Cone
	const float m_viewDistance = 40.0f; //Distance de vue
	const float m_viewAngle = 100.0f;//Angle de vue
	Viewcone m_cone;

	//Egg
	const float m_timeEgg = 11.0f; //Durée du state STATE_Egg
	float		m_timerEgg = 0.0f;  //Timer pour ce state

	//Wandering
	const float m_durationWandering = 40.0f; //Durée du state STATE_Move
	float		m_timerWandering = 0.0f;  //Timer State_Move

	//Reproduction
	bool		m_canReproduce = true;	 //Savoir si le wastedosaure peut se reproduire
	int			m_counterReproduction = 0;	 //Compteur de reproductions
	const int	m_maxReproductions = 2;	 //Nombre de reproductions max
	bool		m_needPartner = false; //Savoir si le w doit trouver un partenaire pour la vie
	const float m_durationReproduction = 10.0f; //Durée de la reproduction
	float		m_timerReproduction = 0.0f;  //Timer pour la reproduction

	//Attaque
	const float	 m_durationAttack = 10.0f; //Durée maximale de l'attaque
	float		 m_timerAttack = 0.0f;  //Timer attaque
	const float	 m_timeBetween2Attacks = 3.0f;  //Temps entre deux attaques
	float		 m_timeElapsedBetween2Attacks = 0.0f;  //Timer entre deux attaques
	const int	 m_damages = 3;	   //Quantité de dégats à envoyer à l'ennemi
	float		 m_distanceToTarget = 0.0f;  //Distance jusqu'à la cible
	bool		 m_pathAttackFound = true;  //Savoir si un chemin d'attaque a été trouvé. Obsolete

	//Follow Path
	Path		  m_path;											//Chemin du Wastedosaure
	Pathfinding * m_pf = Pathfinding::GetSingleton();	//Variable pour le pathfinding
	int			  m_currentIndex = 0;							//Index courant du chemin pour suivre le chemin
	float		  m_timeTryFindPath = 3.0f;						//Temps entre deux tentatives de trouver un chemin (pour éviter d'un faire un à chaque frame et tout faire planter).
	float		  m_timerTryFindPath = 0.0f;						//Timer findpath

	//Autre
	bool									  m_debugDraw = false;	//Afficher ou pas les info de debug
	NYTimer									  m_lastUpdate;		    //Timer pour l'update
	std::map<eTypeCreature, CreatureVector> * m_entities;			//Toutes les creatures du jeu
	std::vector<IABase*>					  m_creaturesInSight;	//Les créatures dans le champ visuel
	NYVert3Df								  m_cubeWater;			//Utilisé dans FindClosestCubeWater. Trouver le cube le plus proche pour le suicide

	//-----Methodes-----//
	int			FindClosestWaypoint(Path _path);		//Renvoie l'index du waypoint le plus proche.
	bool		IsEveryoneArrived();					//Renvoie vrai si tout les Wastedosaures sont arrivés au bout du chemin.
	bool		HasAPath();								//Renvoie vrai si l'entité a un chemin qu'elle peut suivre.
	void		GetCreaturesInSight();					//Récupère l'ensemble des créatures attaquables dans le champ de vision.
	void		UpdateTimers();							//Met à jour les timers d'attaque, de wander et de reproduction.
	NYVert3Df	FindClosestCubeWater();					//Trouve le cube d'eau le plus proche de l'entité.
	NYCubeType	GetCubeUnderType(IABase * target);		//Renvoie le type de cube sur lequel se trouve une entité.
	NYCubeType	GetCubeUnderType(NYVert2Df position);	//Renvoie le type de cube à une position X,Y donnée.

public:
	//------Methodes------//

	//Constructeur/Destructeur
	Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale);//Constructeur
	~Wastedosaure(); //Destructeur

	void SetEntities(std::map<eTypeCreature, CreatureVector> * entities);//Permet de spécifier l'ensemble des entités du monde 
	Path GetPath();//Renvoie le chemin de l'entité

	virtual void UpdateIA(); //Voir IABase
	virtual void Draw();	 //Voir IABase
	//The method from the StateMachine class to override
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);

	//-----Variables-----//

	Wastedosaure * leader = NULL;  //Leader du groupe. Si NULL, alors l'entité est le leader
	Wastedosaure * partner = NULL; //Partenaire du wastedosaure
	bool hasPartner = false;	   //Booléan pour savoir si l'entité a un partenaire ou pas. Utile pour le suicide.
	bool isArrived = true;		   //Booléen pour savoir si le wastedosaure est bien arrivé à la fin d'un chemin ou pas.
	int groupPosition = 0;		   //Position dans le groupe. Obsolete
	IABase * target = NULL;		   //Cible du Wastedosaure
	NYVert2Df arrivalPartner;	   //Position d'arrivée du partenaire.
};