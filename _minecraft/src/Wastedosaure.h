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

class Wastedosaure :public IABase
{
private:

	float m_speed = 0.7f;

	float m_timeEgg = 3.0f;
	float m_timerEgg = 0.0f;

	//Path fo PF
	Pathfinding * m_pf = Pathfinding::GetSingleton();
	Path m_path;
	int m_currentIndex = 0;

	float m_timeTryFindPath = 3.0f;
	float m_timerTryFindPath = 0.0f;
	

	bool HasAPath();

	int FindClosestWaypoint(Path _path);

	Viewcone m_cone;

	std::vector<IABase*> * m_entities;//Toutes les creatures du jeu

	std::vector<IABase*> m_creaturesInSight;//Les créatures dans le champ visuel

	void GetCreaturesInSight();

	bool m_debugDraw = true;

	bool m_isMale = false;

	NYCube * m_cubeWater = NULL;//Sert juste dans FindClosestCubeWater. A delete
	NYCube * FindClosestCubeWater();
	

public:
	Wastedosaure(NYWorld * _world, NYVert2Df _positionInitiale);
	~Wastedosaure();

	Wastedosaure * leader = NULL;//Leader d'un groupe
	Wastedosaure * partner = NULL; //Partenaire du wastedosaure
	int groupPosition = 0;

	IABase * target= NULL; //Cible du Wastedosaure

	void SetEntities(std::vector<IABase*> * entities);

	Path GetPath();

	virtual void UpdateIA();

	virtual void Draw();

	//The method from the StateMachine class to override
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);

};