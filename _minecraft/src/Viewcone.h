/**
 * File : Viewcone.h
 * Author : Paul Gerst
 * Description : Header file describing a class that will
 * verify if points are inside a view cone
 */

#ifndef VIEWCONE_H
#define VIEWCONE_H

#include <engine\utils\types_3d.h>

#include "IABase.h"

class Viewcone
{
private :

	// Pre-computed cosine of the cone's angle
	float m_cosAngle;

	// Cone radius, for drawing purposes
	float m_coneBaseRadius;

	float m_maxDistance;
	NYVert3Df m_position;
	NYVert3Df m_orientation;

public :

	// Basic constructors and destructors
	// Note : angle should be specified in degrees
	Viewcone(float angle, float maxDistance);
	~Viewcone(){}

	// Position and orientation setters
	// Note : calling SetOrientation will include a vector normalization
	void SetOrientation(const NYVert3Df &orientation);
	void SetPosition(const NYVert3Df &position);

	// Main method that will test if a point is inside a cone
	bool IsInSight(const NYVert3Df &point) const;
	
	// Debug method that will draw the view cone
	void DebugDraw(const NYColor &color = NYColor(1.0f, 0.0f, 0.0f, 1.0f)) const;
};

#endif