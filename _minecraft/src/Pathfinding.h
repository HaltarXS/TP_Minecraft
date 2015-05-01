//BY Guillaume Pastor
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

		//Tableau des nodes adjacente pour appliquer un facteur au cout pour les diagonales
		int TabAdj[3][3][3];

		//Manhattan Distance
		int DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b);


		//Analyse des nodes adjacentes
		bool AnalyseAdjacentNodes(int _cubeType);
		bool AnalyseAdjacentNodes2(int _step, bool _walkOnWater);

		void InitializeNodes();

		void ClearNodes();



		Pathfinding();

	public:
		static Pathfinding * GetSingleton();
		~Pathfinding();

		void SetWorld(NYWorld * _world);


		bool FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, int _cubeType, Path & _outPath);

		//Trouve un chemin depuis la position x,y du terrain jusqu'à la position x',y' d'arrivée en prenant en compte une hauteur de cube franchissable
		bool FindPath(NYVert2Df _startPosition, NYVert2Df _arrivalPosition, unsigned _step, Path & _outPath, bool _walkOnWater = false);
	};
