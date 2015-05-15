/**
 * File : Dahut.h
 * Author : Paul Gerst
 * Description : Header file of the Dahut AI class
 */

#ifndef DAHUT_H
#define DAHUT_H

#include "IABase.h"

class Dahut : public IABase
{
private :

	//Determine the cube the Dahut is climbing/walking on
	NYPoint3D m_cubeAnchor;

public :

	Dahut(NYWorld *pWorld, NYVert2Df pos);
	~Dahut();

	//Override base class methods
	virtual void UpdateIA();
	virtual void Draw();
	virtual bool States(StateMachineEvent event, MSG_Object *pMsg, int state);
};

#endif