/**
 * File : Viewcone.cpp
 * Author : Paul Gerst
 * Description : Source file describing a class that will
 * verify if points are inside a view cone
 */

#include <math.h>

#include "Viewcone.h"

Viewcone::Viewcone(float angle, float maxDistance) :
m_cosAngle(cos(angle / 360.0f * M_PI)),
m_coneBaseRadius(tan(angle / 360.0f * M_PI) * maxDistance),
m_maxDistance(maxDistance)
{
}

void Viewcone::SetOrientation(const NYVert3Df &orientation)
{
	m_orientation = orientation;
	m_orientation.normalize();
}

void Viewcone::SetPosition(const NYVert3Df &position)
{
	m_position = position;
}

bool Viewcone::IsInSight(const NYVert3Df &point) const
{
	// Compute distance vector between position and point
	NYVert3Df distance(point.X - m_position.X, point.Y - m_position.Y, point.Z - m_position.Z);
	float length = distance.getSize();

	// Check for distance first
	if(length > m_maxDistance)
	{
		return false;
	}
	else
	{
		// Normalize vector, without using the Normalize() method,
		// since we already computed its size
		distance.X /= length;
		distance.Y /= length;
		distance.Z /= length;

		// Compute angle between vectors and return if the point
		// is not inside the cone
		float cosAngle = distance.scalProd(m_orientation);
		if(cosAngle < m_cosAngle)
		{
			return false;
		}
	}

	return true;
}

void Viewcone::DebugDraw(const NYColor &color) const
{
	// Compute rotation axis and angle for the cone to match view orientation
	NYVert3Df coneDir(0.0f, 0.0f, -1.0f);
	NYVert3Df normal = coneDir.vecProd(m_orientation);
	float angle = acos(coneDir.scalProd(m_orientation));
	angle = (angle / (2.0f * M_PI)) * 360.0f;

	// Draw cone
	glPushMatrix();
	glColor3f(color.R, color.V, color.B);
	glTranslatef(m_position.X + m_orientation.X * m_maxDistance,
				 m_position.Y + m_orientation.Y * m_maxDistance,
				 m_position.Z + m_orientation.Z * m_maxDistance);
	glRotatef(angle, normal.X, normal.Y, normal.Z);
	glutWireCone(m_coneBaseRadius, m_maxDistance, 10, 10);
	glPopMatrix();
}