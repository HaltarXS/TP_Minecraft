/*
* Source file : BiXi class
* Author : Julien Daniel
* Date : 16/05/2015
*/
#include "BiXi.h"

BiXi::BiXi (NYWorld* world, NYVert2Df spawnCoor) : IABase (world), _view (100.0f, 150)
{
	NYVert3Df spawnLocation;
	
	//Calculate 3D starting position from 2D coordinates
	spawnLocation = NYVert3Df(spawnCoor.X * NYCube::CUBE_SIZE, spawnCoor.Y * NYCube::CUBE_SIZE, (world->_MatriceHeights[(int)spawnCoor.X][(int)spawnCoor.Y] + 1) * NYCube::CUBE_SIZE);
	//State machine
	Initialize();
	type = BIXI;
	_eatGrass = true;
	_hasTarget = false;
	_hunger = 0;
	_life = MAX_HEALTH;
	//Timer initialization
	_tick = 0.f;
	_period = 0.f;
	_timeElapsed = 0.f;
	_timer.start ();
	//Position initialization
	position = spawnLocation;
	positionCube = position / NYCube::CUBE_SIZE;
	//Pathfind initialization
	_pathfind = Pathfinding::GetSingleton();
}

void BiXi::UpdateIA ()
{
	Update (_timer.getElapsedSeconds (true));
	_view.SetPosition (position);
	_view.SetOrientation (direction);
	StateMachine::Update ();
}

void BiXi::Update (float elapsedTime)
{
	_tick += elapsedTime;
	_period += elapsedTime;
	_timeElapsed = elapsedTime;
	if (_period >= PERIOD_SIZE)
	{
		_period = 0.f;
		_eatGrass = !_eatGrass;
	}
	if (_tick >= TICK_SIZE)
	{
		_tick = 0.f;
		//Increase hunger
		if (_hunger < MAX_HUNGER)
		{
			_hunger ++;
		}
		if ((IsHungry () && !_hasTarget) || _path.GetSize() <= 0)
		{
			PushState (STATE_FindPath);
		}
		if (IsStarving ())
		{
			_life -= HEALTH_LOSS;
		}
		else if (!IsHungry ())
		{
			_life += HEALTH_GAIN;
			_life = min (_life, MAX_HEALTH);
		}
		if (_life <= 0)
		{
			PushState (STATE_Dead);
		}
	}
}

void BiXi::Eat ()
{
	NYCube* toEat;

	toEat = m_world->getCube (_target.X, _target.Y, _target.Z);
	if (toEat->_Type == CUBE_TERRE)
	{
		toEat->_Type = CUBE_HERBE;
	}
	else
	{
		toEat->_Type = CUBE_TERRE;
	}
	m_world->updateCube (_target.X, _target.Y, _target.Z);
	if (DEBUG)
	{
		std::cout << "Entity " << this->GetID () << " (BiXi) changed the block at coordinates " << _target.X << "/" << _target.Y << "/" << _target.Z << std::endl;
	}
	_hunger -= HUNGER_RECOVERY;
	_life += HEALTH_RECOVERY;
	_hunger = max (_hunger, 0);
	_life = min (_life, MAX_HEALTH);
	_hasTarget = false;
}

bool BiXi::IsHungry ()
{
	return _hunger >= HUNGER_TRESHOLD;
}

bool BiXi::IsStarving ()
{
	return _hunger >= STARVATION_TRESHOLD;
}

void BiXi::Draw ()
{
	glPushMatrix ();
	glTranslatef (position.X, position.Y, position.Z);
	if (_eatGrass)
	{
		glColor3f (0.5f, 0.f, 1.f);
	}
	else
	{
		glColor3f (0.5f, 1.f, 0.f);
	}
	glutSolidCube (5);
	glPopMatrix ();
	if (DEBUG)
	{
		_view.DebugDraw ();
		_path.DrawPath ();
	}
}

NYVert2Df BiXi::FindClosestDirtCube ()
{
	int			x;
	int			y;
	int			z;
	float		length;
	float		cLength;
	NYVert2Df	location;
	NYVert3Df	offset;

	location = NYVert2Df ();
	cLength = 10000000.f;
	length = 0.f;
	for (x = 0; x < MAT_SIZE_CUBES; x++)
	{
		for (y = 0; y < MAT_SIZE_CUBES; y++)
		{
			for (z = 0; z < MAT_HEIGHT_CUBES; z++)
			{
				if (IsValidCube (x, y, z))
				{
					offset = position - NYVert3Df(x, y, z) * NYCube::CUBE_SIZE;
					length = offset.getMagnitude ();
					if (length < cLength)
					{
						cLength = length;
						location = NYVert2Df (x, y);
						_hasTarget = true;
						_target = NYVert3Df (x, y, z);
					}
				}
			}
		}
	}
	return location;
}

bool BiXi::IsValidCube (float x, float y, float z)
{
	return ((_eatGrass && m_world->getCube(x, y, z)->_Type == CUBE_HERBE) || (!_eatGrass && m_world->getCube(x, y, z)->_Type == CUBE_TERRE)) && m_world->getCube(x, y, z + 1)->_Type == CUBE_AIR;
}

bool BiXi::States (StateMachineEvent event, MSG_Object* msg, int state)
{
	int			x;
	int			y;
	int*		data;
	NYVert2Df	destination;
	NYVert3Df	distance;

	BeginStateMachine
		//Messages
		OnMsg (MSG_Attack)
		{
			//Gets the content of the message (has to be an int)
			data = (int*)msg->GetMsgData ();
			_life -= *data;
			if (DEBUG)
			{
				std::cout << "Entity " << this->GetID () << " (BiXi) was attacked by entity " << msg->GetSender () << ". Lost health: " << *data << ". Remaining health: " << _life << std::endl;
			}
			delete data;
			if (life <= 0)
			{
				PushState (STATE_Dead);
			}
		}
		//States
		State (STATE_Initialize)
			OnEnter
				if (DEBUG)
				{
					std::cout << "Entity " << this->GetID () << " (BiXi) entered Initialize State" << std::endl;
				}
		State (STATE_FindPath)
			OnEnter
				_path.Clear ();
				if (IsHungry())
				{
					destination = FindClosestDirtCube ();
				}
				if (!_hasTarget)
				{
					destination = NYVert2Df (rand () % MAT_SIZE_CUBES, rand () % MAT_SIZE_CUBES);
				}
				_pathfind->FindPath (NYVert2Df (position.X / NYCube::CUBE_SIZE, position.Y / NYCube::CUBE_SIZE), destination, STEP_HEIGHT, _path, true);
				_pathIndex = 0;
			OnUpdate
				if (_path.GetSize () > 0)
				{
					PushState (STATE_Move);
				}
		State(STATE_Move)
			OnEnter
				if (_path.GetSize() > 0)
				{
					direction = _path.GetWaypoint (0) - position;
					direction.normalize ();
					_pathIndex = 0;
				}
				else
				{
					PushState (STATE_FindPath);
				}
			OnUpdate
				position += direction * SPEED * _timeElapsed;
				positionCube = position / NYCube::CUBE_SIZE;
				distance = _path.GetWaypoint (_pathIndex) - position;
				if (distance.getSize() <= DISTANCE_TRESHOLD)
				{
					_pathIndex ++;
					if (_hasTarget && IsValidCube(_target.X, _target.Y, _target.Z))
					{
						Eat();
					}
				}
				if (_pathIndex < _path.GetSize ())
				{
					direction = _path.GetWaypoint (_pathIndex) - position;
					direction.normalize();
				}
				else
				{
					_path.Clear();
					PushState (STATE_FindPath);
				}
		State (STATE_Dead)
			OnEnter
				if (DEBUG)
				{
					std::cout << "Entity " << this->GetID () << " (BiXi) is Dead" << std::endl;
				}
	EndStateMachine
}