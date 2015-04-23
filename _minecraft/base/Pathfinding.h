#pragma once

#include <iostream>
#include "ny_utils.h"
#include "engine/utils/types_3d.h"
#include "engine/render/camera.h" 
#include "engine/render/text_engine.h" 
#include "engine/log/log.h"
#include "Path.h"
#include "cube.h"
#include <map>

enum eNodeList { NONE, OPEN_LIST, CLOSE_LIST };

struct Node
{
	int G, H, F, Weight;
	eNodeList List;
	Node* Parent;
	NYVert3Df Position;
	NYCubeType CubeType;
	Node() :
		G(0), 
		H(0), 
		F(0), 
		Parent(NULL), 
		List(NONE)
	{};
};

class Pathfinding
{
	friend class Path;
private:

	//Graph pour le pathfinding 
	Node*** m_nodes;

	//La liste ouverte triée par priorité des node
	std::multimap<int, Node*> m_openList;

	//Node actuelle
	NYVert3Df m_actualNode;

	//Manhattan Distance
	int DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b);

	//Analyse des nodes adjacentes
	bool AnalyseAdjacentNodes(NYCubeType _cubeType);

	Pathfinding();
	
public:
	
	static Pathfinding * GetInstance()
	{
		static Pathfinding instance;
		return &instance;
	}

	~Pathfinding();


	bool FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition,  NYCubeType _cubeType, Path & _outPath);
};

