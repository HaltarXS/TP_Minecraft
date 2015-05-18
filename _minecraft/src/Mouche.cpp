/**
* Author : Guillaume Dor
*/

#include "Mouche.h"

Mouche::Mouche(NYWorld *pWorld, NYVert2Df pos) : IABase(pWorld)
{
	//Init FSM
	Initialize();

	//Init type
	type = MOUCHE;

	srand(time(NULL));

	_poop = NULL;
	_rebornTime = 10.0f;
	_rebornTimer = 0.0f;
	_isAlive = false;
	_wasKilled = false;
	_lastUpdate.start();
	_color = NYVert3Df(0, 0, 0);
	_size = Crotte::CROTTE_SIZE / 6.0f;
	_movementSpeedX = 0.1f;
	_movementSpeedY = 0.2f;

	//Init position
	positionCube.X = (int)pos.X;
	positionCube.Y = (int)pos.Y;
	positionCube.Z = (int)pWorld->_MatriceHeights[(int)pos.X][(int)pos.Y];
	position.X = positionCube.X*NYCube::CUBE_SIZE;
	position.Y = positionCube.Y*NYCube::CUBE_SIZE;
	position.Z = positionCube.Z*NYCube::CUBE_SIZE;
}

Mouche::~Mouche()
{

}

void Mouche::UpdateIA()
{
	//Update FSM
	Update();

	//Start timer
	_lastUpdate.start();
}

void Mouche::Draw()
{
	if (_isAlive)
	{
		//Basic rendering
		glColor3f(_color.X, _color.Y, _color.Z);
		glPushMatrix();
		glTranslatef(position.X + NYCube::CUBE_SIZE / 2.0f, position.Y + NYCube::CUBE_SIZE / 2.0f, position.Z + Crotte::CROTTE_SIZE + _size * 2.0f);
		glutSolidSphere(_size, 20, 20);
		glPopMatrix();
	}
}

bool Mouche::States(StateMachineEvent event, MSG_Object *msg, int state)
{
	BeginStateMachine

	OnMsg(MSG_Attack)
	{
		_wasKilled = true;
		PushState(STATE_Dead);
	}

	State(STATE_Initialize)
	OnEnter 
	{
		_rebornTimer = 0.0f;
	}
	OnUpdate
	{
		if (_wasKilled)
		{
			_rebornTimer += _lastUpdate.getElapsedSeconds();

			if (_rebornTimer >= _rebornTime)
				_wasKilled = false;
		}
		else
		{
			RessourceList* allCrotte = RessourcesManager::GetSingleton()->GetRessourcesByType(TypeRessource::CROTTE);

			for (std::list<Ressource*>::iterator i = allCrotte->begin(); i != allCrotte->end(); ++i)
			{
				Crotte* crotte = (Crotte*)(*i);

				if (!crotte->GetHasFly())
				{
					crotte->SetHasFly(true);
					_poop = crotte;
					position = crotte->Position;
					positionCube = crotte->CubePostion;
					_isAlive = true;
					PushState(STATE_Eat);
					break;
				}
			}
		}
	}

	State(STATE_Eat)
	OnUpdate
	{
		UpdatePosition();

		RessourceList* allCrotte = RessourcesManager::GetSingleton()->GetRessourcesByType(TypeRessource::CROTTE);
		
		bool isFlyPoopDead = true;

		for (std::list<Ressource*>::iterator i = allCrotte->begin(); i != allCrotte->end(); ++i)
		{
			Crotte* crotte = (Crotte*)(*i);

			if (crotte == _poop)
				isFlyPoopDead = false;
		}

		if(isFlyPoopDead)
			PushState(STATE_Dead);
	}	

	State(STATE_Dead)
	OnEnter
	{
		//cout << "Deaderino\n";
		_isAlive = false;
		PushState(STATE_Initialize);
	}

	EndStateMachine
}

void Mouche::UpdatePosition()
{
	if (position.X > (positionCube.X * NYCube::CUBE_SIZE) + Crotte::CROTTE_SIZE / 2.0f)
		_movementSpeedX *= -1;
	else if (position.X < (positionCube.X * NYCube::CUBE_SIZE) - Crotte::CROTTE_SIZE / 2.0f)
		_movementSpeedX *= -1;

	if (position.Y > (positionCube.Y * NYCube::CUBE_SIZE) + Crotte::CROTTE_SIZE / 2.0f)
		_movementSpeedY *= -1;
	else if (position.Y < (positionCube.Y * NYCube::CUBE_SIZE) - Crotte::CROTTE_SIZE / 2.0f)
		_movementSpeedY *= -1;

	position.X += _movementSpeedX;
	position.Y += _movementSpeedY;
}

NYVert3Df Mouche::GetColor()
{
	return _color;
}

float Mouche::GetSize()
{
	return _size;
}