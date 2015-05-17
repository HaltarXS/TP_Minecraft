/*
* Source file : BiXi header
* Author : Julien Daniel
* Date : 16/05/2015
*/


#ifndef _BIXI_H_
	#define _BIXI_H_
	#include "engine\timer.h"
	#include "IABase.h"
	#include "Path.h"
	#include "Pathfinding.h"
	#include "Viewcone.h"

	class BiXi : public IABase
	{
	public :
		BiXi (NYWorld * _world, NYVert2Df _spawnPos);

		void Draw();
		void UpdateIA ();
		bool States (StateMachineEvent event, MSG_Object * msg, int state);

		//All entities of the simulation
		std::map <eTypeCreature, std::vector <IABase *>>* _entities;

	private:
		const bool	DEBUG = false;
		const int	MAX_HUNGER = 50;
		//Hunger value at which the creature will start searching for food
		const int	HUNGER_TRESHOLD = 20;
		//Hunger value at which the creature will start to lose life
		const int	STARVATION_TRESHOLD = 30;
		//How much eating one cube reduces hunger
		const int	HUNGER_RECOVERY = 5;
		const int	MAX_HEALTH = 150;
		//How much eating one cube heals
		const int	HEALTH_RECOVERY = 1;
		//How much health is lost every tick when the creature is starving
		const int	HEALTH_LOSS = 2;
		//How much healh is gained every tick when the creature is full
		const int	HEALTH_GAIN = 1;
		//Maximum height the creature can climb
		const int	STEP_HEIGHT = 2;
		const float SPEED = 10.f;
		//The minimum distance between the creature and its target to consider the target as reached
		const float DISTANCE_TRESHOLD = 0.5f;
		//How long between two updates
		const float TICK_SIZE = 1.f;
		//How long between the two cycles
		const float PERIOD_SIZE = 30.f;
		bool			_eatGrass;
		bool			_hasTarget;
		int				_hunger;
		int				_life;
		int				_pathIndex;
		float			_period;
		float			_tick;
		float			_timeElapsed;
		NYTimer			_timer;
		Viewcone		_view;
		Path			_path;
		NYVert3Df		_movement;
		NYVert3Df		_target;
		Pathfinding*	_pathfind;

		void Update (float _elapsedTime);
		void Eat ();
		bool IsHungry ();
		bool IsStarving ();
		bool IsValidCube (float x, float y, float z);
		NYVert2Df FindClosestDirtCube ();
	};
#endif