#include "Pathfinding.h"


Pathfinding::Pathfinding()
{
	//Init nodes
	//Node*** m_nodes;
}


Pathfinding::~Pathfinding()
{

}

int Pathfinding::DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b)
{
	return (abs(b.X - a.X) + abs(b.Y - a.Y) + abs(b.Z - a.Z)) * 10;
}

bool Pathfinding::AnalyseAdjacentNodes(NYCubeType _cubeType)
{

	return 0;
}

bool Pathfinding::FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, NYCubeType _cubeType, Path & _outPath)
{
	bool pathFound = false;

	return pathFound;
}