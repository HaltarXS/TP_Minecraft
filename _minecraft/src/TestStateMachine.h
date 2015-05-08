//MADE BY GUILLAUME PASTOR
#pragma once
#include "statemch.h"
#include "engine\render\renderer.h"
#include "IABase.h"
#include <random>

class TestStateMachine : public IABase
{
private:
	//Add here your custom variables
	const float m_sleepDuration = 3.0f;
	float m_timerSleep = 0.0f;

	const float m_moveDuration = 10.0f;
	float m_timerMove = 0.0f;

	int m_life = 100;

	objectID m_entityToCommunicateWith;

public:
	TestStateMachine();
	~TestStateMachine();

	
	//The method from the IABase class to override
	virtual void UpdateIA();

	virtual void Draw();

	void SetEntityToCommunicateWith(objectID OtherEntity);//TEST ONLY

	//The method from the StateMachine class to override
	virtual bool States(StateMachineEvent event, MSG_Object * msg, int state);
};

