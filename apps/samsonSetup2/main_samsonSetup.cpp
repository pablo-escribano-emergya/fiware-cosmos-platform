/* ****************************************************************************
*
* FILE                     main_samsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdio.h>                 // printf, ...
#include <sys/stat.h>              // struct stat
#include <unistd.h>                // stat
#include <fcntl.h>                 // open, O_RDWR, O_CREAT, O_TRUNC, ...
#include <stdlib.h>                // free

#include "parseArgs.h"             // parseArgs
#include "logMsg.h"                // LM_*
#include "traceLevels.h"           // Trace levels

#include "engine/MemoryManager.h"  // ss::MemoryManager
#include "SamsonSetup.h"           // ss::SamsonSetup
#include "Endpoint2.h"             // Endpoint2
#include "SamsonStarter.h"         // SamsonStarter



/* ****************************************************************************
*
* Option variables
*/
char           controllerHost[256];
int            workers;
const char*    ips[100];
bool           reset;
bool           pList;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",   controllerHost,  "CONTROLLER", PaString,  PaReq,  PaND,   PaNL,  PaNL,  "Controller host"               },
	{ "-workers",     &workers,         "WORKERS",    PaInt,     PaReq,     0,     0,   100,   "number of workers"             },
	{ "-ips",          ips,             "IP_LIST",    PaSList,   PaReq,  PaND,   PaNL,  PaNL,  "list of worker IPs"            },
	{ "-reset",       &reset,           "RESET",      PaBool,    PaOpt,  false, false,  true,  "reset platform"                },
	{ "-plist",       &pList,           "P_LIST",     PaBool,    PaOpt,  false, false,  true,  "process list of platform"      },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int            logFd        = -1;
SamsonStarter*   samsonStarter  = NULL;
int            startTime;



/* ****************************************************************************
*
* plist - 
*/
static void plist(void)
{
	ss::Endpoint2::Status s;

	if ((s = samsonStarter->processList()) != ss::Endpoint2::OK)
		LM_X(1, ("Error sending Process List Message to spawners"));

	LM_M(("Got the list - I'm done"));
	exit(0);
}



/* ****************************************************************************
*
* resetAndStart - 
*/
static void resetAndStart(void)
{
	ss::Endpoint2::Status s;

	if ((s = samsonStarter->reset()) != ss::Endpoint2::OK)
		LM_X(1, ("Error sending RESET to all spawners: %s", ((ss::Endpoint2*) NULL)->status(s)));

	if (reset)
	{
		LM_M(("Resetted platform - I'm done"));
		exit(0);
	}

	if ((s = samsonStarter->procVecSend()) != ss::Endpoint2::OK)
		LM_X(1, ("Error sending Process Vector to all spawners: %s", ((ss::Endpoint2*) NULL)->status(s)));

	LM_M(("Started platform - I'm done"));
	exit(0);
}



/* ****************************************************************************
*
* readyCheck - 
*/
void readyCheck(void* callbackData, void* userParam)
{
	int unhelloed = 0;
	int now;
	int helloed = 0;

	callbackData  = NULL;
	userParam     = NULL;

	for (int ix = 0; ix < samsonStarter->networkP->epMgr->endpointCapacity(); ix++)
	{
		ss::Endpoint2* ep;

		ep = samsonStarter->networkP->epMgr->get(ix);
		if (ep == NULL)
			continue;   // Could do break here, really ...

		if (ep->typeGet() == ss::Endpoint2::Spawner)
		{
			if (ep->stateGet() != ss::Endpoint2::Ready)
				++unhelloed;
			else
				++helloed;

			LM_M(("Spawner endpoint in '%s'. helloed: %d, unhelloed: %d", ep->hostGet()->name, helloed, unhelloed));
		}			

		if (ep->typeGet() == ss::Endpoint2::Unhelloed)
			LM_W(("Endpoint %02d is Unhelloed - could it be a spawner-to-be ... ?", ix));
	}

	samsonStarter->networkP->epMgr->show("readyCheck");

	LM_M(("helloed: %d, unhelloed: %d", helloed, unhelloed));
	if (unhelloed == 0)
	{
		if (helloed != samsonStarter->spawners)
			LM_X(1, ("Helloed: %d, Spawners: %d - what has happened?", helloed, samsonStarter->spawners));

		LM_M(("All Helloes interchanged - READY TO ROLL!"));
		if (pList)
			plist();
		else
			resetAndStart();		
	}
	else
	{
		now = time(NULL);
		if ((now - startTime) > 5)
			LM_X(1, ("five seconds elapsed since connecting to spawners and still not all of them have helloed - I die"));
	}
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	memset(controllerHost, 0, sizeof(controllerHost));

	paConfig("prefix",                        (void*) "SSP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT (FUNC)");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	
	if ((long) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (long) ips[0]));

	ss::SamsonSetup::load();
	engine::MemoryManager::init(ss::SamsonSetup::shared()->memory);

	samsonStarter = new SamsonStarter();
	samsonStarter->procVecCreate(controllerHost, workers, ips);

	startTime = time(NULL);
	if (samsonStarter->connect() != ss::Endpoint2::OK)
		LM_X(1, ("Error connecting to all spawners"));

	LM_M(("Connected to all spawners"));

	samsonStarter->networkP->epMgr->callbackSet(ss::EndpointManager::Timeout,  readyCheck, NULL);
	samsonStarter->networkP->epMgr->callbackSet(ss::EndpointManager::Periodic, readyCheck, NULL);

	samsonStarter->networkP->epMgr->show("Before calling run");
	samsonStarter->run();

	return 0;
}
