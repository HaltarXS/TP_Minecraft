#ifndef __TIME_FSM_H__
#define __TIME_FSM_H__

#include "singleton.h"


class Time : public Singleton <Time>
{
public:

	Time( void );
	~Time( void ) {}

	void MarkTimeThisTick( void );
	float GetElapsedTime( void )				{ return( m_TimeLastTick ); }
	float GetCurTime( void )					{ return( m_CurrentTime ); }

	void SetGameSpeed( float value );
	float GetGameSpeed( void )					{ return( m_GameSpeed ); }


private:

	float m_StartTime;
	float m_CurrentTime;
	float m_TimeLastTick;
	float m_GameSpeed;

	unsigned int GetExactTimeInMilliseconds( void );

};







#endif	// __TIME_H__