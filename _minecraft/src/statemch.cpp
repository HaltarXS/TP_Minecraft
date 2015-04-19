#include "statemch.h"
#include "msgroute.h"
#include "global.h"
#include "database.h"
#include "singleton.h"


StateMachine::StateMachine(GameObject * owner )
{
	m_type = "";

	if(owner == NULL)
	{
		m_Owner = new GameObject(g_database.GetNewObjectID());
		g_database.Store(*m_Owner);
		m_Owner->SetStateMachine(this);
		m_IsHierarchical = false;
	}
	else
	{
		m_Owner = owner;
		m_IsHierarchical = true;
	}
	
	m_currentState = 0;
	m_stateChange = false;
	m_nextState = false;
	m_timeOnEnter = 0.0f;
	m_ccMessagesToGameObject = 0;

}

StateMachine::~StateMachine( void )
{
	if(!m_IsHierarchical)
	{
		if(Singleton<Database>::DoesSingletonExist())
		{
			g_database.Remove(m_Owner->GetID());
		}

		delete m_Owner;
	}
}

std::string StateMachine::GetType()
{
	return m_type;
}


void StateMachine::Initialize( void )
{
	Process( EVENT_Enter, 0 );
}


void StateMachine::Update( void )
{
	Process( EVENT_Update, 0 );
}


void StateMachine::Process( StateMachineEvent event, MSG_Object * msg )
{

	if( event == EVENT_Message && msg && GetCCReceiver() > 0 )
	{	// CC this message
		SendMsg( msg->GetMsgName(), GetCCReceiver() );
	}

	if( States( event, msg, m_currentState ) == false )
	{	// Current state didn't handle msg, try Global state (-1)
		States( event, msg, -1 );
	}

	// Check for a state change
	int safetyCount = 50;
	while( m_stateChange && (--safetyCount >= 0) )
	{
		assert( safetyCount > 0 && "StateMachine::Process - States are flip-flopping in an infinite loop." );

		m_stateChange = false;

		// Let the last state clean-up
		States( EVENT_Exit, 0, m_currentState );

		// Set the new state
		m_currentState = m_nextState;

		// Remember the time we entered this state
		m_timeOnEnter = g_time.GetCurTime();

		// Let the new state initialize
		States( EVENT_Enter, 0, m_currentState );
	}

}


void StateMachine::PushState(unsigned int newState)
{

	m_StatesList.push_back(newState);
	SetState(m_StatesList.back());
	m_maxSize = m_StatesList.size();
	if(m_maxSize > 10)
	{
		m_StatesList.pop_front();
	}
}


void StateMachine::PopState()
{
	if(m_StatesList.size() > 0)
	{
		m_StatesList.pop_back();
		SetState(m_StatesList.back());
	}
	
}

void StateMachine::Replace(unsigned int newState)
{
	m_StatesList.back() = newState;
	SetState(m_StatesList.back());
}

void StateMachine::SetState( unsigned int newState )
{
	m_stateChange = true;
	m_nextState = newState;
}


void StateMachine::SendMsg( MSG_Name name, objectID receiver, void* data )
{
	g_msgroute.SendMsg( 0, name, receiver, m_Owner->GetID(), -1, data );

}


void StateMachine::SendDelayedMsg( float delay, MSG_Name name, objectID receiver, void* data )
{
	g_msgroute.SendMsg( delay, name, receiver, m_Owner->GetID(), -1, data );

}


void StateMachine::SendDelayedMsgToMe( float delay, MSG_Name name, MSG_Scope scope )
{
	if( scope == SCOPE_TO_THIS_STATE ) {
		g_msgroute.SendMsg( delay, name, m_Owner->GetID(), m_Owner->GetID(), m_currentState );
	}
	else {
		g_msgroute.SendMsg( delay, name, m_Owner->GetID(), m_Owner->GetID(), -1 );
	}

}

GameObject * StateMachine::GetOwner()
{
	return m_Owner;
}

