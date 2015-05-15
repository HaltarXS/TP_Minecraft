/*
* Source file : Lemming AI class
* Author : DESHORS Axel
* Date : 15/05/2015
*/

#include "Lemming.h"


Lemming::Lemming(NYWorld * _world, NYVert2Df _spawnPos)
	:IABase(_world)
{
	// Get the spawn position :
	NYVert3Df _worldSpawnPos = NYVert3Df(	_spawnPos.X * NYCube::CUBE_SIZE,
											_spawnPos.Y*NYCube::CUBE_SIZE,
											(_world->_MatriceHeights[(int)_spawnPos.X][(int)_spawnPos.Y] + 1) * NYCube::CUBE_SIZE);

	// State machine initialization
	IABase::Initialize();
	IABase::type = LEMMING;

	// Gauge initialization
	m_currentHungerPoints = 0;
	m_currentLifePoints = m_maxLifePoints;
	m_currentReproductionPoints = 0;

	// Position init
	IABase::position = _worldSpawnPos;

}


bool Lemming::IsHungry()
{
	return (m_currentHungerPoints >= 100);
}

Lemming::~Lemming()
{
}
