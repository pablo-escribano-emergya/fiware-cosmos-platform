/* ****************************************************************************
*
* FILE                     main_samsonController.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>                // read
#include <fcntl.h>                 // open, O_RDONLY, ...
#include <sys/stat.h>              // struct stat

#include "au/LockDebugger.h"       // au::LockDebugger

#include "parseArgs/parseArgs.h"
#include "au/LockDebugger.h"

#include "engine/MemoryManager.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"
#include "engine/Engine.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/platformProcesses.h"
#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonVars.h"
#include "samson/network/Network2.h"
#include "samson/network/Endpoint2.h"
#include "samson/network/EndpointManager.h"
#include "samson/controller/SamsonController.h"



/* ****************************************************************************
*
* Option variables
*/
SAMSON_ARG_VARS;

bool version;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	SAMSON_ARGS,

	{ "-version", &version,  "SS_CONTROLLER_VERSION",  PaBool,    PaOpt,    false,    false,   true,  "print version string and exit" },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* global variables
*/
int                   logFd    = -1;
samson::Network2*     networkP = NULL;



/* ****************************************************************************
*
* exitFunction - 
*/
void exitFunction(void)
{
	LM_W(("Freeing up stuff before EXITING"));

	if (networkP != NULL)
	{
		LM_M(("deleting Network instance"));
		delete networkP;
		networkP = NULL;
	}

	if (progName)
	{
		LM_M(("Freeing progName"));
		free(progName);
		progName = NULL;
	}
}



/* ****************************************************************************
*
* manDescription - 
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "\nsamsonController is a key component in a SAMSON system\n";
static const char* manDescription      = 
    "\n"
    "samsonController is a key component in a SAMSON system. It a single point of synch and control\n"
    "Multiple samsonWorker's and delilah's can connect to this element to form a SAMSON cluster\n"
    "\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	paConfig("builtin prefix",                (void*) "SS_CONTROLLER_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paConfig("man synopsis",                  (void*) manSynopsis);
	paConfig("man shortdescription",          (void*) manShortDescription);
	paConfig("man description",               (void*) manDescription);
	paConfig("man exitstatus",                (void*) manExitStatus);
	paConfig("man author",                    (void*) manAuthor);
	paConfig("man reportingbugs",             (void*) manReportingBugs);
	paConfig("man copyright",                 (void*) manCopyright);
	paConfig("man version",                   (void*) manVersion);

	paParse(paArgs, argC, (char**) argV, 1, false);

	if (version)
	{
		printf("%s\n", SAMSON_VERSION);
		exit(1);
	}

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	atexit(exitFunction);
	atexit(google::protobuf::ShutdownProtobufLibrary);

	samson::platformProcessesPathInit("/opt/samson");

    // Make sure this singlelton is created just once
    au::LockDebugger::shared();
    
	// Init singletons
	au::LockDebugger::shared();             // Lock usage debugging (necessary here where there is only one thread)
	samson::SamsonSetup::init();  // Load setup and create all directories
    samson::SamsonSetup::shared()->setWorkingDirectory(samsonWorking);
    
    
	engine::Engine::init();
	engine::DiskManager::init(1);
	engine::ProcessManager::init(samson::SamsonSetup::getInt("general.num_processess"));
	engine::MemoryManager::init(samson::SamsonSetup::getUInt64("general.memory"));
    
	samson::ModulesManager::init();         // Init the modules manager

	networkP  = new samson::Network2(samson::Endpoint2::Controller);

	networkP->runInBackground();
   	samson::SamsonController controller(networkP);

    LM_M(("Running"));
	// Run the engine function
    while( true )
        sleep(10);
}
