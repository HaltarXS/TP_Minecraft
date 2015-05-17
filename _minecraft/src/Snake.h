/**
* File : Snake.h
* Author : Vincent Swaenepoel
* Description : Header file of the Snake AI class
*/

#ifndef SNAKE_H
#define SNAKE_H

#include "IABase.h"
class Snake : public IABase
{
private:
	int m_size;

public:
	Snake(NYWorld *pWorld, NYVert2Df pos);
	~Snake();

	void Move();
	void Eat();
	void Split();
};

#endif

