#ifndef __STATEMCH_H__
#define __STATEMCH_H__

#include <assert.h>
#include "msg.h"
#include "gameobject.h"
#include "debuglog.h"
#include "timeFSM.h"
#include "global.h"
#include <deque>
#include <string>
#include <map>


//State Machine Language Macros (put these keywords in the file USERTYPE.DAT in the same directory as MSDEV.EXE)
#define BeginStateMachine		if( state < 0 ) { char statename[64] = "STATE_Global"; if(0) {
#define EndStateMachine			return( true ); } } else { assert( 0 && "Invalid State" ); return( false ); } return( false );
#define State(a)				return( true ); } } else if( a == state ) { char statename[64] = #a; if(0) { 
#define OnMsg(a)				return( true ); } else if( EVENT_Message == event && msg && a == msg->GetMsgName() ) { g_debuglog.LogStateMachineEvent( m_Owner->GetID(), msg, statename, #a, true );
#define OnEvent(a)				return( true ); } else if( a == event ) { g_debuglog.LogStateMachineEvent( m_Owner->GetID(), msg, statename, #a, true );
#define OnUpdate				OnEvent( EVENT_Update )
#define OnEnter					OnEvent( EVENT_Enter )
#define OnExit					OnEvent( EVENT_Exit )



enum StateMachineEvent { EVENT_INVALID,
						 EVENT_Update,
						 EVENT_Message,
						 EVENT_Enter,
						 EVENT_Exit
};

typedef enum { 
	NO_MSG_SCOPING,
	SCOPE_TO_THIS_STATE
} MSG_Scope;

enum States_FSM
{ 
	STATE_Initialize,
	STATE_Move,
	STATE_Sleep,
	STATE_Eat,
	STATE_Dead,
	STATE_Egg,
	STATE_FindPath,
	STATE_Suicide,
	STATE_Reproduction,
	STATE_Attack,
	STATE_Wait,
	STATE_FindSnow,
	STATE_FindTarget,
	STATE_Dance,
	STATE_Follow
};

class StateMachine
{
public:

	StateMachine( GameObject * owner = NULL);
	~StateMachine( void );

	std::string GetType();

	void Initialize( void );
	void Update( void );
	
	void PushState(unsigned int newState);
	void PopState();
	void Replace(unsigned int newState);

	void SendMsg( MSG_Name name, objectID receiver, void* data = NULL );
	void SendDelayedMsg( float delay, MSG_Name name, objectID receiver, void* data = NULL );
	void SendDelayedMsgToMe( float delay, MSG_Name name, MSG_Scope scope );

	int GetState( void )						{ return( m_currentState ); }
	float GetTimeInState( void )				{ return( g_time.GetCurTime() - m_timeOnEnter ); }				

	void SetCCReceiver( objectID id )			{ m_ccMessagesToGameObject = id; }
	void ClearCCReceiver( void )				{ m_ccMessagesToGameObject = 0; }
	objectID GetCCReceiver( void )				{ return( m_ccMessagesToGameObject ); }

	void Process( StateMachineEvent event, MSG_Object * msg );

	GameObject * GetOwner();

protected:

	GameObject * m_Owner;
	bool m_IsHierarchical;
	std::string m_type;
	std::map<const int ,std::string> m_stateNames;
	unsigned int m_currentState;

private:

	unsigned short m_maxSize;
	std::deque<int> m_StatesList;

	void SetState( unsigned int newState );

	//unsigned int m_currentState;
	unsigned int m_nextState;
	bool m_stateChange;
	float m_timeOnEnter;
	objectID m_ccMessagesToGameObject;

	//Method to override !
	virtual bool States( StateMachineEvent event, MSG_Object * msg, int state ) = 0;

};


#endif	// __STATEMCH_H__