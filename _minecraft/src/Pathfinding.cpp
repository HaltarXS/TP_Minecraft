#include "Pathfinding.h"


Pathfinding::Pathfinding()
{
	//Init nodes
	//WIDTH HEIGHT DEPTH
	m_nodes = new Node**[MAT_SIZE_CUBES];
	for (int i = 0; i < MAT_SIZE_CUBES; ++i)
	{
		m_nodes[i] = new Node*[MAT_HEIGHT_CUBES];
		for (int j = 0; j < MAT_HEIGHT_CUBES; ++j)
		{
			m_nodes[i][j] = new Node[MAT_SIZE_CUBES];
		}
	}

	InitializeNodes();
}


Pathfinding::~Pathfinding()
{
	for (int i = 0; i<MAT_SIZE_CUBES; i++)
	{
		for (int j = 0; j<MAT_HEIGHT_CUBES; j++)
		{
			delete m_nodes[i][j];
		}
		delete m_nodes[i];
	}
	delete[]m_nodes;
}

void Pathfinding::InitializeNodes()
{
	for (int i = 0; i<MAT_SIZE_CUBES; i++)
	{
		for (int j = 0; j<MAT_HEIGHT_CUBES; j++)
		{
			for (int k = 0; k<MAT_SIZE_CUBES; k++)
			{
				m_nodes[i][j][k].Weight = 10;
				m_nodes[i][j][k].Position = NYVert3Df(i, j, k);
				m_nodes[i][j][k].CubeType = m_world->getCube(i, j, k)->_Type;
			}
		}
	}
}

int Pathfinding::DistanceManhattan(const NYVert3Df & a, const NYVert3Df & b)
{
	return (abs(b.X - a.X) + abs(b.Y - a.Y) + abs(b.Z - a.Z)) * 10;
}

bool Pathfinding::AnalyseAdjacentNodes(NYCubeType _cubeType)
{
	bool foundBetterNode = false;

	Node actualNode = m_nodes[(int)(m_actualNode.X)][(int)(m_actualNode.Y)][(int)(m_actualNode.Z)];
	Node betterNode = actualNode;
	//On va regarder toutes les nodes autour de la node Actuelle.
	m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z].List = CLOSE_LIST;//On met la node actuelle(que l'on va utiliser pour essayer de trouver une node utilisable autour) dans la liste fermée pour ne plus ensuite la réutiliser.

	int x, y = 0;
	float factorWeight = 0;
	factorWeight = 1.0f;

	for (int x = -1; x <= 1;++x)
	{
		for (int y = -1; y <= 1;++y)
		{
			for (int z = -1; z <= 1;++z)
			{
				Node nodeToAnalyse = m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)];
				if ((x + m_actualNode.X >= 0 && x + m_actualNode.X < MAT_SIZE_CUBES) && (z + m_actualNode.Z >= 0 && z + m_actualNode.Z < MAT_SIZE_CUBES) && (y + m_actualNode.Y >= 0 && y + m_actualNode.Y < MAT_HEIGHT_CUBES) && nodeToAnalyse.CubeType == _cubeType && (m_nodes[(int)m_actualNode.X + x][(int)m_actualNode.Y + y][(int)m_actualNode.Z + z].List != CLOSE_LIST) && (x != 0 && y != 0 && z != 0))
				{
					if (nodeToAnalyse.List == NO_LIST)  // si le noeud n'a jamais été visité
					{
						//On lui donne son parent
						nodeToAnalyse.Parent = &actualNode;
						//On calcul la distance Manhattan en vue de Calculer son cout F
						nodeToAnalyse.H = DistanceManhattan(nodeToAnalyse.Position, m_arrivalPosition);
						//Calcul du poid du déplacement
						nodeToAnalyse.G = actualNode.G + actualNode.Weight * factorWeight;
						//Calcul du nouveau Cout
						nodeToAnalyse.F = nodeToAnalyse.H + nodeToAnalyse.G;
						//On met la node dans l'open List
						nodeToAnalyse.List = OPEN_LIST;
						m_openList.insert(std::pair<int, Node*>(nodeToAnalyse.F, &nodeToAnalyse));
					}
					else if (nodeToAnalyse.List == OPEN_LIST && (nodeToAnalyse.G > actualNode.G + actualNode.Weight * factorWeight))//Sinon si il est déjà dans l'open List
					{
						nodeToAnalyse.Parent = &actualNode;
						nodeToAnalyse.G = actualNode.G + actualNode.Weight * factorWeight;
						nodeToAnalyse.F = nodeToAnalyse.H + nodeToAnalyse.G;
					}

					//si le noeud scanné est plus proche de la fin que le meilleur noeud actuel, on a trouvé une meilleur node !
					if (nodeToAnalyse.F <= betterNode.F)
					{
						foundBetterNode = true;
						betterNode = nodeToAnalyse;
					}
				}
			}
		}
	}

	if (foundBetterNode)
	{
		betterNode.List = CLOSE_LIST;
		actualNode = betterNode;
	}

	return foundBetterNode;

	

	return foundBetterNode;
}

bool Pathfinding::FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, NYCubeType _cubeType, Path & _outPath)
{
	bool PathNotFound = false;
	m_startPosition = _startPosition;
	m_arrivalPosition = _arrivalPosition;

	//Initialisation de la première Node actuelle (StartPos).
	m_actualNode = _startPosition;

	if (_arrivalPosition == _startPosition || !(_arrivalPosition.X >= 0 && _arrivalPosition.X < MAT_SIZE_CUBES) || !(_arrivalPosition.Y >= 0 && _arrivalPosition.Y < MAT_HEIGHT_CUBES) || !(_arrivalPosition.Z >= 0 && _arrivalPosition.Z < MAT_SIZE_CUBES))
	{
		PathNotFound = true;
	}

	m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z].H = DistanceManhattan(m_actualNode, _arrivalPosition);//On calcul H
	m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z].F = m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z].H;// + m_mapsNode[layerName][m_actualNode.x + m_actualNode.y*m_width].G;//On calcul F = H + G
	m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z].List = CLOSE_LIST;//On met la premiere node dans la liste fermée

	while (m_actualNode != _arrivalPosition && !PathNotFound)
	{
		//On explore toutes les possibilités en partant d'une node.
		do
		{
			PathNotFound = !AnalyseAdjacentNodes(_cubeType);
		} while (!PathNotFound);

		//On a bloqué dans la boucle d'avant, on va chercher dans la liste ouverte la prochaine node utilisable. Si on trouve aucune node utilisable, on a trouvé aucun chemin.
		if (m_openList.size() != 0)//Si on a encore des éléments dans la liste ouverte, alors on a encore une chance de trouver un chemin
		{
			PathNotFound = false;
			m_actualNode = m_openList.begin()->second->Position;
			m_openList.erase(m_openList.begin());
		}


	}

	if (!PathNotFound)
	{
		std::cout << "=====Path found=====\n";
		//On "déplie" les nodes pour connaitre le chemin
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