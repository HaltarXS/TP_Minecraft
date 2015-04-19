#include "timeFSM.h"
#include "database.h"
#include "msgroute.h"
#include "debuglog.h"

class FSM
{
private:
	Time* master_time = NULL;
	Database* master_database = NULL;
	MsgRoute* master_msgroute = NULL;
	DebugLog* master_debuglog = NULL;


	FSM()
	{
		std::cout << "Creating Master FSM\n";
		Time* master_time = new Time();
		Database* master_database = new Database(); 
		MsgRoute* master_msgroute = new MsgRoute();
		DebugLog* master_debuglog = new DebugLog();
	}

	static FSM * m_singleton;

public:
	static FSM* getSingleton()
	{
		static FSM myFsm;
		return &myFsm;
	}

	~FSM()
	{
		std::cout << "Deleting Master FSM\n";
		delete(master_time);
		delete(master_database);
		delete(master_msgroute);
		delete(master_debuglog);
	}
};
