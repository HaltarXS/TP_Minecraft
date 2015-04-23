//BY Guillaume Pastor & Brahim Berkati
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

	inline void setPath(const std::vector<NYVert3Df> & _ThePath)
	{
		m_wayPoints = _ThePath;
	}

	inline std::vector<NYVert3Df> getPath()
	{
		return m_wayPoints;
	}
};

