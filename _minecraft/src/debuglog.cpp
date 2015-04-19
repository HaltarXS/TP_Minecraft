#include "debuglog.h"
#include "timeFSM.h"



#define MAX_DEBUG_LOG_SIZE 100


LogEntry::LogEntry( void )
{
	m_owner = INVALID_OBJECT_ID;
	m_handled = false;

	m_timestamp = -1.0f;
	strcpy_s( m_statename, "" );
	strcpy_s( m_eventmsgname, "" );
	m_receiver = INVALID_OBJECT_ID;
	m_sender = INVALID_OBJECT_ID;
	
}


DebugLog::DebugLog( void )
{

}


DebugLog::~DebugLog( void )
{

}


void DebugLog::LogStateMachineEvent( objectID id, MSG_Object * msg, char* statename, char* eventmsgname, bool handled )
{
	LogEntry * entry = new LogEntry();

	entry->m_owner = id;
	entry->m_handled = handled;
	entry->m_timestamp = g_time.GetCurTime();
	strcpy_s( entry->m_statename, statename );
	strcpy_s( entry->m_eventmsgname, eventmsgname );

	if( msg ) {
		entry->m_receiver = msg->GetReceiver();
		entry->m_sender = msg->GetSender();
	}

	m_log.push_back( entry );
	if( m_log.size() > MAX_DEBUG_LOG_SIZE ) {
		delete( m_log.front() );
		m_log.erase( m_log.begin() );
	}
}
