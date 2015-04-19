#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <iostream>

#define g_time Time::GetSingleton()
#define g_factory Factory::GetSingleton()
#define g_database Database::GetSingleton()
#define g_msgroute MsgRoute::GetSingleton()
#define g_debuglog DebugLog::GetSingleton()


#define INVALID_OBJECT_ID 0

typedef unsigned int objectID;



#endif	// __GLOBAL_H__