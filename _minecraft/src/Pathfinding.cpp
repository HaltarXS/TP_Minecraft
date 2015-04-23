#include "Pathfinding.h"


Pathfinding::Pathfinding()
{
	//Init nodes
	//WIDTH HEIGHT DEPTH
	m_nodes = new Node**[MAT_SIZE_CUBES];
	for (int i = 0; i < MAT_SIZE_CUBES; ++i)
	{
		m_nodes[i] = new Node*[MAT_SIZE_CUBES];
		for (int j = 0; j < MAT_SIZE_CUBES; ++j)
		{
			m_nodes[i][j] = new Node[MAT_HEIGHT_CUBES];
		}
	}
}


Pathfinding::~Pathfinding()
{
	for (int i = 0; i<MAT_SIZE_CUBES; i++)
	{
		for (int j = 0; j<MAT_SIZE_CUBES; j++)
		{
			delete m_nodes[i][j];
		}
		delete m_nodes[i];
	}
	delete[]m_nodes;
}

Pathfinding * Pathfinding::GetSingleton()
{
	static Pathfinding instance;
	return &instance;
}

void Pathfinding::InitializeNodes()
{
	for (int i = 0; i<MAT_SIZE_CUBES; i++)
	{
		for (int j = 0; j<MAT_SIZE_CUBES; j++)
		{
			for (int k = 0; k<MAT_HEIGHT_CUBES; k++)
			{
				m_nodes[i][j][k].Weight = 10;
				m_nodes[i][j][k].Position = NYVert3Df(i, j, k);
				m_nodes[i][j][k].CubeType = m_world->getCube(i, j, k)->_Type;

			}
		}
	}
}

void Pathfinding::ClearNodes()
{
	for (int i = 0; i<MAT_SIZE_CUBES; i++)
	{
		for (int j = 0; j<MAT_SIZE_CUBES; j++)
		{
			for (int k = 0; k<MAT_HEIGHT_CUBES; k++)
			{
				m_nodes[i][j][k].Weight = 10;
				m_nodes[i][j][k].List = NO_LIST;
			}
		}
	}
}

void Pathfinding::SetWorld(NYWorld * _world)
{
	m_world = _world;
	InitializeNodes();
	std::cout << "Graph for Pathfinding Initialized" << std::endl;
}

int Pathfinding::DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b)
{
	return (abs(b.X - a.X) + abs(b.Y - a.Y) + abs(b.Z - a.Z)) * 10;
}



bool Pathfinding::AnalyseAdjacentNodes(NYCubeType _cubeType)
{
	bool foundBetterNode = false;

	m_betterNode = m_realActualNode;

	//On va regarder toutes les nodes autour de la node Actuelle.
	m_realActualNode->List = CLOSE_LIST;//On met la node actuelle(que l'on va utiliser pour essayer de trouver une node utilisable autour) dans la liste ferm�e pour ne plus ensuite la r�utiliser.

	//int x, y = 0;
	float factorWeight = 0;
	factorWeight = 1.0f;

	for (int x = -1; x <= 1;++x)
	{
		for (int y = -1; y <= 1;++y)
		{
			for (int z = -1; z <= 1;++z)
			{
				bool inX = (x + m_actualNode.X >= 0 && x + m_actualNode.X < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en X
				bool inY = (y + m_actualNode.Y >= 0 && y + m_actualNode.Y < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en Y
				bool inZ = (z + m_actualNode.Z >= 0 && z + m_actualNode.Z < MAT_HEIGHT_CUBES); ////Si on est pas en dehors de la map en Z
				bool cubeType = (m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)].CubeType == _cubeType); //Si on teste le bon type de cube
				bool listOK = (m_nodes[(int)m_actualNode.X + x][(int)m_actualNode.Y + y][(int)m_actualNode.Z + z].List != CLOSE_LIST); //Si le cube test� n'est pas dans la liste ferm�e
				bool notCurrentCube = (x != 0 || y != 0 || z != 0); //Si on ne teste pas le cube courant

				if (inX && inY && inZ && cubeType && listOK && notCurrentCube)
				{
					m_nodeToAnalyse = &m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)];
					if (m_nodeToAnalyse->List == NO_LIST)  // si le noeud n'a jamais �t� visit�
					{
						//On lui donne son parent
						m_nodeToAnalyse->Parent = m_realActualNode;
						//On calcul la distance Manhattan en vue de Calculer son cout F
						m_nodeToAnalyse->H = DistanceManhattan(m_nodeToAnalyse->Position, m_arrivalPosition);
						//Calcul du poid du d�placement
						m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * factorWeight;
						//Calcul du nouveau Cout
						m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
						//On met la node dans l'open List
						m_nodeToAnalyse->List = OPEN_LIST;
						m_openList.insert(std::pair<int, Node*>(m_nodeToAnalyse->F, m_nodeToAnalyse));//On l'ins�re dans la liste ouverte
					}
					else if (m_nodeToAnalyse->List == OPEN_LIST && (m_nodeToAnalyse->G > m_realActualNode->G + m_realActualNode->Weight * factorWeight))//Sinon si il est d�j� dans l'open List
					{
						m_nodeToAnalyse->Parent = m_realActualNode;
						m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * factorWeight;
						m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
					}

					//si le noeud scann� est plus proche de la fin que le meilleur noeud actuel, on a trouv� une meilleur node !
					if (m_nodeToAnalyse->F <= m_betterNode->F)
					{
						foundBetterNode = true;
						m_betterNode = m_nodeToAnalyse;
					}
				}
			}
		}
	}

	if (foundBetterNode)
	{
		m_betterNode->List = CLOSE_LIST;
		m_realActualNode = m_betterNode;
		m_actualNode = m_realActualNode->Position;

	}

	return foundBetterNode;

}

bool Pathfinding::FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, NYCubeType _cubeType, Path & _outPath)
{
	ClearNodes();
	m_openList.clear();

	bool PathNotFound = false;
	m_startPosition = _startPosition;
	m_arrivalPosition = _arrivalPosition;
	
	

	//Initialisation de la premi�re Node actuelle (StartPos).
	m_actualNode = _startPosition;
	m_realActualNode = &m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z];

	if (_arrivalPosition == _startPosition || !(_arrivalPosition.X >= 0 && _arrivalPosition.X < MAT_SIZE_CUBES) || !(_arrivalPosition.Y >= 0 && _arrivalPosition.Y < MAT_HEIGHT_CUBES) || !(_arrivalPosition.Z >= 0 && _arrivalPosition.Z < MAT_SIZE_CUBES))
	{
		PathNotFound = true;
	}

	m_realActualNode->H = DistanceManhattan(m_actualNode, _arrivalPosition);//On calcul H
	m_realActualNode->F = m_realActualNode->H;// + m_mapsNode[layerName][m_actualNode.x + m_actualNode.y*m_width].G;//On calcul F = H + G
	m_realActualNode->List = CLOSE_LIST;//On met la premiere node dans la liste ferm�e

	while (m_actualNode != _arrivalPosition && !PathNotFound)
	{
		//On explore toutes les possibilit�s en partant d'une node.
		do
		{
			PathNotFound = !AnalyseAdjacentNodes(_cubeType);

		} 
		while (!PathNotFound);

		//On a bloqu� dans la boucle d'avant, on va chercher dans la liste ouverte la prochaine node utilisable. Si on trouve aucune node utilisable, on a trouv� aucun chemin.
		if (m_openList.size() != 0)//Si on a encore des �l�ments dans la liste ouverte, alors on a encore une chance de trouver un chemin
		{
			PathNotFound = false;
			m_actualNode = m_openList.begin()->second->Position;
			m_realActualNode = m_openList.begin()->second;
			m_openList.erase(m_openList.begin());
		}
	}

	if (!PathNotFound)
	{
		std::cout << "=====Path found=====\n";
		//On "d�plie" les nodes pour connaitre le chemin
		m_tempPath.clear();
		Node * TempNode = &m_nodes[(int)_arrivalPosition.X][(int)_arrivalPosition.Y][(int)_arrivalPosition.Z];
		while (TempNode->Parent)
		{
			TempNode->Position = NYVert3Df(TempNode->Position.X * NYCube::CUBE_SIZE, TempNode->Position.Y * NYCube::CUBE_SIZE, TempNode->Position.Z * NYCube::CUBE_SIZE);
			m_tempPath.push_back(TempNode->Position);
			TempNode = TempNode->Parent;
		}

		std::reverse(m_tempPath.begin(), m_tempPath.end());
		_outPath.setPath(m_tempPath);
	}
	else
	{
		std::cout << "=====Path not found=====\n";
	}

	

	return !PathNotFound;
}