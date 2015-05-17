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

#include "Viewcone.h"

class Cameleon :
	public IABase
{
public:
	
	int m_waypointIndex;
	Pathfinding * m_pathFinding = Pathfinding::GetSingleton();
	bool m_pathFound;
	Path m_path;
	float m_speed = 0.7f ;
	

	NYTimer m_timer;

	NYVert3Df m_destination;


	Cameleon(NYWorld * _world, NYVert2Df _positionInitiale);
	~Cameleon();


	void Draw();

	void UpdateIA();

	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

