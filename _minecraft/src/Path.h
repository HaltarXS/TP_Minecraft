//BY Guillaume Pastor
//Class Path pour le Pathfinding
#pragma once
#include <vector>
#include "ny_utils.h"
#include "engine/utils/types_3d.h"
#include "engine/render/camera.h" 
#include "engine/render/text_engine.h" 
#include "engine/log/log.h"

class Path
{
private:
	std::vector<NYVert3Df> m_wayPoints;
	
public:
	Path();
	~Path();

	inline void SetPath(const std::vector<NYVert3Df> & _ThePath)
	{
		m_wayPoints.clear();
		m_wayPoints = _ThePath;
	}


	inline std::vector<NYVert3Df> GetPath()
	{
		return m_wayPoints;
	}

	inline void PrintPath()
	{
		for (std::vector<NYVert3Df>::iterator it = m_wayPoints.begin(); it != m_wayPoints.end(); ++it)
		{
			std::cout << it->X << "," << it->Y << "," << it->Z << endl;
		}
	}

	inline NYVert3Df & GetWaypoint(const int index)
	{
		return m_wayPoints[index];
	}

	inline int GetSize()
	{
		return m_wayPoints.size();
	}

	inline void DrawPath()
	{
		
		for (int i = 1; i < m_wayPoints.size(); ++i)
		{
			glBegin(GL_LINES);
			glColor3d(1, 0, 0);
			glVertex3d(m_wayPoints[i - 1].X, m_wayPoints[i - 1].Y, m_wayPoints[i - 1].Z);
			glVertex3d(m_wayPoints[i].X, m_wayPoints[i].Y, m_wayPoints[i].Z);
			glEnd();
		}
	}

	inline void Clear()
	{
		m_wayPoints.clear();
	}

};

