/* ****************************************************************************
*
* FILE                      Endpoint.cpp
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string.h>             // strchr
#include <stdlib.h>             // atoi
#include <unistd.h>             // gethostname

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*, ...

#include "Endpoint.h"           // Own interface



namespace ss {



/* ****************************************************************************
*
* hostnameGet - 
*/
void Endpoint::hostnameGet(void)
{
	char hn[128];

	if (gethostname(hn, sizeof(hn)) == 0)
	{
		hostname = std::string(hn);
		LM_T(LMT_CONFIG, ("hostname: '%s'", hostname.c_str()));
	}
	else
		LM_P(("gethostname"));
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string name, std::string ip, unsigned short port, int fd)
{
	this->name     = name;
	this->type     = type;
	this->ip       = ip;
	this->port     = port;
	this->fd       = fd;
	this->state    = (fd == -1)? Disconnected : Connected;
	this->workers  = 0;

	hostnameGet();
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string ipAndPort)
{
	char* port = strchr((char*) ipAndPort.c_str(), ':');

	if (port == NULL)
	{
		this->name  = "local accepter";
		this->port  = atoi(ipAndPort.c_str());
		this->ip    = "127.0.0.1";
	}
	else
	{
		this->name  = ipAndPort;

		*port = 0;
		++port;
		this->ip    = ipAndPort;
		this->port  = atoi(port);
	}

	this->fd       = -1;
	this->state    = Disconnected;
	this->workers  = 0;
	this->type     = type;

	hostnameGet();
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, unsigned short port)
{
	this->name     = "no name";
	this->ip       = "localhost";
	this->port     = port;
	this->fd       = -1;
	this->state    = Disconnected;
	this->workers  = 0;
	this->type     = type;

	hostnameGet();
}



/* ****************************************************************************
*
* stateName - 
*/
char* Endpoint::stateName(void)
{
	switch (state)
	{
	case Free:            return (char*) "Free";
	case Me:              return (char*) "ME";
	case FutureWorker:    return (char*) "FutureWorker";
	case Listening:       return (char*) "Listening";
	case Connected:       return (char*) "Connected";
	case Closed:          return (char*) "Closed";
	case Disconnected:    return (char*) "Disconnected";
	case Reconnecting:    return (char*) "Reconnecting";
	}

	return (char*) "Unknown";
}



/* ****************************************************************************
*
* reset - 
*/
void Endpoint::reset(void)
{
	close(fd);

	fd       = -1;
	state    = Free;
	type     = Unknown;
	name     = "no name";
	workers  = 0;
}

}
