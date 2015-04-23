#pragma once

#include <iostream>
#include "ny_utils.h"
#include "engine/utils/types_3d.h"
#include "engine/render/camera.h" 
#include "engine/render/text_engine.h" 
#include "engine/log/log.h"
#include "Path.h"
//#include "cube.h"
#include <map>
#include "world.h"

	enum eNodeList { NO_LIST, OPEN_LIST, CLOSE_LIST };

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
			List(NO_LIST)
		{};
	};

	class Pathfinding
	{
		friend class Path;
	private:

		NYWorld * m_world;

		std::vector<NYVert3Df> m_tempPath;

		//Graph pour le pathfinding 
		Node*** m_nodes;
		//vector<vector<vector<Node>>> m_nodes;

		NYVert3Df m_startPosition;
		NYVert3Df m_arrivalPosition;

		//La liste ouverte triée par priorité des node
		std::multimap<int, Node*> m_openList;

		//Node actuelle
		NYVert3Df m_actualNode;
		Node *m_realActualNode;
		Node *m_nodeToAnalyse;
		Node *m_betterNode;

		//Manhattan Distance
		int DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b);

		//Analyse des nodes adjacentes
		bool AnalyseAdjacentNodes(NYCubeType _cubeType);

		void InitializeNodes();

		void ClearNodes();



		Pathfinding();

	public:
		static Pathfinding * GetSingleton();
		~Pathfinding();

		void SetWorld(NYWorld * _world);
		bool FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, NYCubeType _cubeType, Path & _outPath);
	};
