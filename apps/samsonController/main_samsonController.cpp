


#include "SamsonController.h"	// ss::SamsonController
#include "traces.h"				// LMT_*

/* ****************************************************************************
 *
 * main - main routine for the samsonController
 */
#define LOG_FORMAT "TYPE:EXEC:FILE[LINE]:FUNC: TEXT"
int main(int argc, const char* argv[])
{
	ss::samsonInitTrace( argc , argv );
	
	ss::Network network;		// Real network element
	ss::SamsonController controller(argc, argv , &network);
	controller.run();
}
