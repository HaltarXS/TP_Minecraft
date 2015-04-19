#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "global.h"

class StateMachine;

class GameObject
{
public:

	GameObject( objectID id );
	~GameObject( void );

	objectID GetID( void )							{ return( m_ID ); }
	StateMachine* GetStateMachine( void )			{ return( m_StateMachine ); }
	void SetStateMachine( StateMachine * mch )		{ m_StateMachine = mch; }

private:

	objectID m_ID;

	StateMachine* m_StateMachine;

};


#endif	// __GAMEOBJECT_H__