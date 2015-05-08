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

	enum eNodeList 
	{ 
		NO_LIST, 
		OPEN_LIST, 
		CLOSE_LIST 
	};

	struct Node
	{
		int G, H, F, Weight;//F = Cout Global, H = heuristique, G = cout deplacement
		eNodeList List;//Liste courante de la node
		Node* Parent; //Parent de la node
		NYVert3Df Position; //Posiiton de la node
		NYCubeType CubeType; //Type de la node
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

		//Graph pour le pathfinding. Tableau à 3 dimensions
		Node*** m_nodes;

		NYVert3Df m_startPosition;
		NYVert3Df m_arrivalPosition;

		//La liste ouverte triée par priorité des node.
		std::multimap<int, Node*> m_openList;

		//Node actuelle
		NYVert3Df m_actualNode;//Position de la node actuellement analysée
		Node *m_realActualNode;//Node actuellement analysé
		Node *m_nodeToAnalyse; //Node adjacente à la realactualnode en cours d'analyse
		Node *m_betterNode; //Pointeur sur une meilleure node si on en a trouvé une. Elle deviendra la realActualNdoe

		//Tableau des nodes adjacente pour appliquer un facteur au cout pour les diagonales
		int TabAdj[3][3][3];

		//Manhattan Distance
		int DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b);


		//Analyse des nodes adjacentes
		bool AnalyseAdjacentNodes(int _cubeType);//Pour le premier type de PF
		bool AnalyseAdjacentNodes2(int _step, bool _walkOnWater);//Pour le deuxième type de PF

		//Pertmet d'initialiser les nodes. Appelé une seule fois.
		void InitializeNodes();

		//Permet de réinitialiser les nodes avant un PF
		void ClearNodes();

		Pathfinding();

	public:
		static Pathfinding * GetSingleton();
		~Pathfinding();

		//Permet de dire quel monde on va utiliser pour la génération du graph
		void SetWorld(NYWorld * _world);

		//Trouve un chemin dans des types de cubes donnés. _cubeType est un champs de bits, on peut par exemple mettre "CUBE_TERRE | CUBE_AIR" pour cherche un chemin dans les cubes d'air et de terre. 
		bool FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, int _cubeType, Path & _outPath);

		//Trouve un chemin depuis la position x,y du terrain jusqu'à la position x',y' d'arrivée en prenant en compte une hauteur de cube franchissable
		bool FindPath(NYVert2Df _startPosition, NYVert2Df _arrivalPosition, unsigned _step, Path & _outPath, bool _walkOnWater = false);
	};
