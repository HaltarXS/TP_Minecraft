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

public :

	Dahut(NYWorld *pWorld, NYVert2Df pos);
	~Dahut();

	virtual void UpdateIA(){}
	virtual void Draw(){}
	virtual bool States(StateMachineEvent event, MSG_Object *pMsg, int state){ return true; }
};

#endif