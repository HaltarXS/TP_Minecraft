#ifndef __MSGROUTE_H__
#define __MSGROUTE_H__

#include "msg.h"
#include "global.h"
#include "singleton.h"
#include <list>


typedef std::list<MSG_Object*> MessageContainer;

class MsgRoute : public Singleton <MsgRoute>
{
public:

	MsgRoute( void );
	~MsgRoute( void );

	void DeliverDelayedMessages( void );
	void SendMsg( float delay, MSG_Name name, objectID receiver, objectID sender, int state, void* data=NULL );

private:

	MessageContainer m_delayedMessages;

	void RouteMsg( MSG_Object & msg );

};


#endif	// __MSGROUTE_H__