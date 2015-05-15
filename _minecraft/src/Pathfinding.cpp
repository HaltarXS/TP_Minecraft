//BY Guillaume Pastor
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


	//Init Tableau des nodes adjacentes
	int offset = 1;
	//Rangée du bas
	m_tabAdj[offset - 1][offset - 1][offset - 1] = 2.4f;
	m_tabAdj[offset - 0][offset - 1][offset - 1] = 1.4f;
	m_tabAdj[offset + 1][offset - 1][offset - 1] = 2.4f;

	m_tabAdj[offset - 1][offset - 0][offset - 1] = 1.4f;
	m_tabAdj[offset - 0][offset - 0][offset - 1] = 1.0f;
	m_tabAdj[offset + 1][offset - 0][offset - 1] = 1.4f;

	m_tabAdj[offset - 1][offset + 1][offset - 1] = 2.4f;
	m_tabAdj[offset - 0][offset + 1][offset - 1] = 1.4f;
	m_tabAdj[offset + 1][offset + 1][offset - 1] = 2.4f;

	//Rangée milieu
	m_tabAdj[offset - 1][offset - 1][offset - 0] = 1.4f;
	m_tabAdj[offset - 0][offset - 1][offset - 0] = 1.0f;
	m_tabAdj[offset + 1][offset - 1][offset - 0] = 1.4f;

	m_tabAdj[offset - 1][offset - 0][offset - 0] = 1.0f;
	m_tabAdj[offset - 0][offset - 0][offset - 0] = 0;
	m_tabAdj[offset + 1][offset - 0][offset - 0] = 1.0f;

	m_tabAdj[offset - 1][offset + 1][offset - 0] = 1.4f;
	m_tabAdj[offset - 0][offset + 1][offset - 0] = 1.0f;
	m_tabAdj[offset + 1][offset + 1][offset - 0] = 1.4f;

	//Rangée du haut
	m_tabAdj[offset - 1][offset - 1][offset + 1] = 2.4f;
	m_tabAdj[offset - 0][offset - 1][offset + 1] = 1.4f;
	m_tabAdj[offset + 1][offset - 1][offset + 1] = 2.4f;

	m_tabAdj[offset - 1][offset - 0][offset + 1] = 1.4f;
	m_tabAdj[offset - 0][offset - 0][offset + 1] = 1.0f;
	m_tabAdj[offset + 1][offset - 0][offset + 1] = 1.4f;

	m_tabAdj[offset - 1][offset + 1][offset + 1] = 2.4f;
	m_tabAdj[offset - 0][offset + 1][offset + 1] = 1.4f;
	m_tabAdj[offset + 1][offset + 1][offset + 1] = 2.4f;


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
				m_nodes[i][j][k].G = 0;
				m_nodes[i][j][k].F = 0;
				m_nodes[i][j][k].H = 0;
				m_nodes[i][j][k].Parent = NULL;
				m_nodes[i][j][k].Weight = 10;
				m_nodes[i][j][k].List = NO_LIST;
				m_nodes[i][j][k].Position = NYVert3Df(i, j, k);
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

int Pathfinding::DistanceManhattan(const NYVert3Df & _a, const NYVert3Df & _b)
{
	return (abs(_b.X - _a.X) + abs(_b.Y - _a.Y) + abs(_b.Z - _a.Z)) * 10;
}



bool Pathfinding::AnalyseAdjacentNodes(int _cubeType)
{
	bool foundBetterNode = false;

	m_betterNode = m_realActualNode;

	//On va regarder toutes les nodes autour de la node Actuelle.
	m_realActualNode->List = CLOSE_LIST;//On met la node actuelle(que l'on va utiliser pour essayer de trouver une node utilisable autour) dans la liste fermée pour ne plus ensuite la réutiliser.

	for (int x = -1; x <= 1;++x)
	{
		for (int y = -1; y <= 1;++y)
		{
			for (int z = -1; z <= 1;++z)
			{
				bool inX = (x + m_actualNode.X >= 0 && x + m_actualNode.X < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en X
				bool inY = (y + m_actualNode.Y >= 0 && y + m_actualNode.Y < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en Y
				bool inZ = (z + m_actualNode.Z >= 0 && z + m_actualNode.Z < MAT_HEIGHT_CUBES); ////Si on est pas en dehors de la map en Z
				if (!inX || !inY || !inZ)
				{
					cerr << "/!\ Out of bounds\n";
					return false;
				}
					
				//bool cubeType = (m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)].CubeType == _cubeType); //Si on teste le bon type de cube
				bool cubeType = (m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)].CubeType & _cubeType) != 0 ? true : false; //Si on teste le bon type de cube
				bool listOK = (m_nodes[(int)m_actualNode.X + x][(int)m_actualNode.Y + y][(int)m_actualNode.Z + z].List != CLOSE_LIST); //Si le cube testé n'est pas dans la liste fermée
				bool notCurrentCube = (x != 0 || y != 0 || z != 0); //Si on ne teste pas le cube courant

				if (inX && inY && inZ && cubeType && listOK && notCurrentCube)
				{
					m_nodeToAnalyse = &m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)];
					if (m_nodeToAnalyse->List == NO_LIST)  // si le noeud n'a jamais été visité
					{
						//On lui donne son parent
						m_nodeToAnalyse->Parent = m_realActualNode;
						//On calcul la distance Manhattan en vue de Calculer son cout F
						m_nodeToAnalyse->H = DistanceManhattan(m_nodeToAnalyse->Position, m_arrivalPosition);
						//Calcul du poid du déplacement
						m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + z]; //* factorWeight;
						//Calcul du nouveau Cout
						m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
						//On met la node dans l'open List
						m_nodeToAnalyse->List = OPEN_LIST;
						m_openList.insert(std::pair<int, Node*>(m_nodeToAnalyse->F, m_nodeToAnalyse));//On l'insère dans la liste ouverte
					}
					else if (m_nodeToAnalyse->List == OPEN_LIST && (m_nodeToAnalyse->G > m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + z]))//Sinon si il est déjà dans l'open List
					{
						m_nodeToAnalyse->Parent = m_realActualNode;
						m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + z];
						m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
					}

					//si le noeud scanné est plus proche de la fin que le meilleur noeud actuel, on a trouvé une meilleur node !
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

bool Pathfinding::FindPath(NYVert3Df _startPosition, NYVert3Df _arrivalPosition, int _cubeType, Path & _outPath)
{
	ClearNodes();
	m_openList.clear();

	bool PathNotFound = false;
	m_startPosition = _startPosition;
	m_arrivalPosition = _arrivalPosition;
	
	

	//Initialisation de la première Node actuelle (StartPos).
	m_actualNode = _startPosition;
	m_realActualNode = &m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z];

	if (_arrivalPosition == _startPosition || !(_arrivalPosition.X >= 0 && _arrivalPosition.X < MAT_SIZE_CUBES) || !(_arrivalPosition.Y >= 0 && _arrivalPosition.Y < MAT_SIZE_CUBES) || !(_arrivalPosition.Z >= 0 && _arrivalPosition.Z < MAT_HEIGHT_CUBES))
	{
		PathNotFound = true;
	}

	m_realActualNode->H = DistanceManhattan(m_actualNode, _arrivalPosition);//On calcul H
	m_realActualNode->F = m_realActualNode->H;// + m_mapsNode[layerName][m_actualNode.x + m_actualNode.y*m_width].G;//On calcul F = H + G
	m_realActualNode->List = CLOSE_LIST;//On met la premiere node dans la liste fermée

	while (m_actualNode != _arrivalPosition && !PathNotFound)
	{
		//On explore toutes les possibilités en partant d'une node.
		do
		{
			PathNotFound = !AnalyseAdjacentNodes(_cubeType);

		} 
		while (!PathNotFound);

		//On a bloqué dans la boucle d'avant, on va chercher dans la liste ouverte la prochaine node utilisable. Si on trouve aucune node utilisable, on a trouvé aucun chemin.
		if (m_openList.size() != 0)//Si on a encore des éléments dans la liste ouverte, alors on a encore une chance de trouver un chemin
		{
			PathNotFound = false;
			m_actualNode = m_openList.begin()->second->Position;
			m_realActualNode = m_openList.begin()->second;
			m_openList.erase(m_openList.begin());
		}
	}

	if (!PathNotFound)
	{
		//std::cout << "=====Path found=====\n";
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
		_outPath.SetPath(m_tempPath);
	}
	else
	{
		//std::cout << "=====Path not found=====\n";
	}

	

	return !PathNotFound;
}




//--------------------------------------------------------------------------------------------
bool Pathfinding::AnalyseAdjacentNodes2(int _step, bool _walkOnWater)
{
	bool foundBetterNode = false;

	m_betterNode = m_realActualNode;

	//On va regarder toutes les nodes autour de la node Actuelle.
	m_realActualNode->List = CLOSE_LIST;//On met la node actuelle(que l'on va utiliser pour essayer de trouver une node utilisable autour) dans la liste fermée pour ne plus ensuite la réutiliser.
	
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			bool inX = (x + m_actualNode.X >= 0 && x + m_actualNode.X < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en X
			bool inY = (y + m_actualNode.Y >= 0 && y + m_actualNode.Y < MAT_SIZE_CUBES); //Si on est pas en dehors de la map en Y
			int z = m_world->_MatriceHeights[(int)m_actualNode.X + x][(int)m_actualNode.Y + y] - 1;
			if (z < 0) z = 0;
			bool inZ = z >= 0; //Si on est pas en dehors de la map en Y
			if (!inX || !inY || !inZ)
			{
				//cerr << "/!\ Out of bounds\n";
				return false;
			}

			m_nodeToAnalyse = &m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][z];
			bool cubeType = true;
			if (!_walkOnWater)
			{
				 cubeType = m_nodeToAnalyse->CubeType != CUBE_EAU;
			}
			bool listOK = m_nodeToAnalyse->List != CLOSE_LIST; //Si le cube testé n'est pas dans la liste fermée
			bool notCurrentCube = (x != 0 || y != 0); //Si on ne teste pas le cube courant
			int step = m_nodeToAnalyse->Position.Z - m_actualNode.Z;
			bool stepOK = abs(step) <= _step ? true : false;//Si la step est ok


			//Other
			/*float offset = 0.0f;
			offset = ;
			if (offset < 0.0f)
				offset = -1.0f;
			else if (offset > 0.0f)
				offset = 1.0f;
			else
				offset = 0.0f;*/

			if (inX && inY && inZ && cubeType && listOK && notCurrentCube && stepOK)
			{
				//m_nodeToAnalyse = &m_nodes[(int)(x + m_actualNode.X)][(int)(y + m_actualNode.Y)][(int)(z + m_actualNode.Z)];
				if (m_nodeToAnalyse->List == NO_LIST)  // si le noeud n'a jamais été visité
				{
					//On lui donne son parent
					m_nodeToAnalyse->Parent = m_realActualNode;
					//On calcul la distance Manhattan en vue de Calculer son cout F
					m_nodeToAnalyse->H = DistanceManhattan(m_nodeToAnalyse->Position, m_arrivalPosition);
					//Calcul du poid du déplacement
					
					m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + (int)((step / _step))]; //* factorWeight;
					//Calcul du nouveau Cout
					m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
					//On met la node dans l'open List
					m_nodeToAnalyse->List = OPEN_LIST;
					m_openList.insert(std::pair<int, Node*>(m_nodeToAnalyse->F, m_nodeToAnalyse));//On l'insère dans la liste ouverte
				}
				else if (m_nodeToAnalyse->List == OPEN_LIST && (m_nodeToAnalyse->G > m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + (int)((step / _step))]))//Sinon si il est déjà dans l'open List
				{
					m_nodeToAnalyse->Parent = m_realActualNode;
					m_nodeToAnalyse->G = m_realActualNode->G + m_realActualNode->Weight * m_tabAdj[1 + x][1 + y][1 + (int)((step / _step))];
					m_nodeToAnalyse->F = m_nodeToAnalyse->H + m_nodeToAnalyse->G;
				}

				//si le noeud scanné est plus proche de la fin que le meilleur noeud actuel, on a trouvé une meilleur node !
				if (m_nodeToAnalyse->F <= m_betterNode->F)
				{
					foundBetterNode = true;
					m_betterNode = m_nodeToAnalyse;
				}
			}


		}
	}//Fin for

	if (foundBetterNode)
	{
		m_betterNode->List = CLOSE_LIST;
		m_realActualNode = m_betterNode;
		m_actualNode = m_realActualNode->Position;

	}

	return foundBetterNode;

}


bool Pathfinding::FindPath(NYVert2Df _startPosition, NYVert2Df _arrivalPosition, unsigned _step, Path & _outPath, bool _walkOnWater)
{
	ClearNodes();
	m_openList.clear();

	bool PathNotFound = false;
	int zStart = m_world->_MatriceHeights[(int)_startPosition.X][(int)_startPosition.Y] - 1;
	if (zStart < 0) zStart = 0;
	int zArrival = m_world->_MatriceHeights[(int)_arrivalPosition.X][(int)_arrivalPosition.Y] - 1;
	if (zArrival < 0) zArrival = 0;
	m_startPosition = NYVert3Df(_startPosition.X, _startPosition.Y, zStart);
	m_arrivalPosition = NYVert3Df(_arrivalPosition.X, _arrivalPosition.Y, zArrival);
	//NYCube * testdepart = m_world->getCube(m_startPosition.X, m_startPosition.Y, m_startPosition.Z);
	//cout << m_startPosition.X << "," << m_startPosition.Y << "," << m_startPosition.Z << " --- Cube type : " << testdepart->_Type << endl;
		/*_startPosition;
	m_arrivalPosition = _arrivalPosition;*/

	m_actualNode = m_startPosition;
	m_realActualNode = &m_nodes[(int)m_actualNode.X][(int)m_actualNode.Y][(int)m_actualNode.Z];

	if (_arrivalPosition == _startPosition || !(_arrivalPosition.X >= 0 && _arrivalPosition.X < MAT_SIZE_CUBES) || !(_arrivalPosition.Y >= 0 && _arrivalPosition.Y < MAT_SIZE_CUBES))
	{
		PathNotFound = true;
	}

	m_realActualNode->H = DistanceManhattan(m_actualNode, m_arrivalPosition);//On calcul H
	m_realActualNode->F = m_realActualNode->H;// + m_mapsNode[layerName][m_actualNode.x + m_actualNode.y*m_width].G;//On calcul F = H + G
	m_realActualNode->List = CLOSE_LIST;//On met la premiere node dans la liste fermée

	while (m_actualNode != m_arrivalPosition && !PathNotFound)
	{
		//On explore toutes les possibilités en partant d'une node.
		do
		{
			PathNotFound = !AnalyseAdjacentNodes2(_step, _walkOnWater);

		} while (!PathNotFound);

		//On a bloqué dans la boucle d'avant, on va chercher dans la liste ouverte la prochaine node utilisable. Si on trouve aucune node utilisable, on a trouvé aucun chemin.
		if (m_openList.size() != 0)//Si on a encore des éléments dans la liste ouverte, alors on a encore une chance de trouver un chemin
		{
			PathNotFound = false;
			m_actualNode = m_openList.begin()->second->Position;
			m_realActualNode = m_openList.begin()->second;
			m_openList.erase(m_openList.begin());
		}
	}

	if (!PathNotFound)
	{
		//std::cout << "=====Path found=====\n";
		//On "déplie" les nodes pour connaitre le chemin
		m_tempPath.clear();
		Node * TempNode = &m_nodes[(int)m_arrivalPosition.X][(int)m_arrivalPosition.Y][(int)m_arrivalPosition.Z];
		while (TempNode->Parent)
		{
			TempNode->Position = NYVert3Df((TempNode->Position.X * NYCube::CUBE_SIZE) + NYCube::CUBE_SIZE / 2.0f, (TempNode->Position.Y  * NYCube::CUBE_SIZE) + NYCube::CUBE_SIZE / 2.0f, ((TempNode->Position.Z + 1) * NYCube::CUBE_SIZE) + NYCube::CUBE_SIZE / 2.0f);
			m_tempPath.push_back(TempNode->Position);
			TempNode = TempNode->Parent;
		}

		std::reverse(m_tempPath.begin(), m_tempPath.end());
		_outPath.SetPath(m_tempPath);
	}
	else
	{
		//std::cout << "=====Path not found=====\n";
	}



	return !PathNotFound;
}