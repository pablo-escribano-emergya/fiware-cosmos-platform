#ifndef NETWORK_H
#define NETWORK_H

/* ****************************************************************************
*
* FILE                     Network.h - Definition for the network interface
*
*/
#include <cstring>				// size_t
#include <vector>				// vector

#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // EndpointManager
#include "NetworkInterface.h"   // ss:NetworkInterface 



namespace ss
{

	

/* ****************************************************************************
*
* Network2 - main element that interconnects
*                       o samsonController
*                       o samsonWorker
*                       o delilah
*/
class Network2 : public NetworkInterface
{
private:
	EndpointManager*  epMgr;
	int               tmoSecs;
	int               tmoUSecs;

public:
	Network2(EndpointManager* epMgr);
	~Network2();

	void  tmoSet(int secs, int usecs);  // Set timeout for select loop
	void  run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...
};

}

#endif
