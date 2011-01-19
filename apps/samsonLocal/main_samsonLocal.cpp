
#include "parseArgs.h"          // parseArgs

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*
#include "traces.h"				// Traces stuff: samsonInitTrace(.) , ...

#include "Endpoint.h"			// ss::EndPoint
#include "FakeEndpoint.h"
#include "NetworkFake.h"
#include "NetworkFakeCenter.h"
#include "NetworkInterface.h"

#include "DelilahConsole.h"		// ss:DelilahConsole
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "SamsonSetup.h"		// ss::SamsonSetup

#include "ModulesManager.h"		// ss::ModulesManager
#include "samson/Operation.h"	// ss::Operation
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager
#include "ProcessManager.h"		// ss::ProcessManager


#include <signal.h>				// signal(.)


/* ****************************************************************************
 *
 * Option variables
 */
char             controller[80];
int              workers;
bool             noLog;
char			 workingDir[1024]; 	



#define S01 (long int) "samson01:1234"
/* ****************************************************************************
 *
 * parse arguments
 */
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaOpt,   S01,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,     1,      1,    100,  "number of workers"   },
	{ "-nolog",      &noLog,       "NO_LOG",      PaBool,    PaOpt, false,  false,   true,  "no logging"          },
	{ "-working",     workingDir,  "WORKING",     PaString,  PaOpt,  _i SAMSON_DEFAULT_WORKING_DIRECTORY,   PaNL,   PaNL,  "Working directory"     },
	PA_END_OF_ARGS
};

/* ****************************************************************************
 *
 * logFd - file descriptor for log file used in all libraries
 */
int logFd = -1;


void *run_DelilahConsole(void* d)
{
	ss::DelilahConsole* delilahConsole = (ss::DelilahConsole*) d;

	delilahConsole->run();
	return NULL;
}

template<class C>
void *run_in_background(void* d)
{
	C* c = (C*) d;
	c->run();
	return NULL;
}

int main(int argC, const char *argV[])
{
	
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);
	
	paParse(paArgs, argC, (char**) argV, 1, false);// No more pid in the log file name
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();

	/*
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));
	for (int i = 0 ; i < 256 ; i++)
		LM_T(i,("Trace test %d",i));
	 */
	
	ss::SamsonSetup::load( workingDir );		// Load setup and create default directories

	// Init singlelton in single thread mode
	ss::MemoryManager::init();	// Memory manager
	
	ss::ProcessManager::init();		// Init process manager
	ss::ModulesManager::init();		// Init the modules manager

	ss::DiskManager::shared();		// Disk manager
	ss::FileManager::shared();		// File manager
	
	assert( workers != -1 );
	
	// Fake network element with N workers
	ss::NetworkFakeCenter center(workers);		
	
	// Create one controller, one dalilah and N workers
	ss::SamsonController controller( center.getNetwork(-1) );
	controller.runBackgroundProcesses();
	
	ss::Delilah delilah(center.getNetwork(-2));
	ss::DelilahConsole delilahConsole( &delilah );
	
	LM_M(("SamsonLocal start"));
	LM_T(LMT_SAMSON_DEMO, ("Starting samson demo (logFd == %d)", ::logFd));

	std::vector< ss::SamsonWorker* > _workers;
	for (int i = 0 ; i < workers ; i ++ )
	{
		ss::SamsonWorker *w = new ss::SamsonWorker( center.getNetwork(i) );
		_workers.push_back(w);
	}

	// Run the network center in background
	pthread_t t;
	pthread_create(&t, NULL, run_in_background<ss::NetworkFakeCenter> , &center);
	
	// Run delilah client in foreground
	delilahConsole.run();
	
	assert( false );	// We never come back to here
	
}






