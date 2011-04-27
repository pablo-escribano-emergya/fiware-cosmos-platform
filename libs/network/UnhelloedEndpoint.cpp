/* ****************************************************************************
*
* FILE                     UnhelloedEndpoint.h
*
* DESCRIPTION              Class for unhelloed endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <pthread.h>            // pthread_t

#include "EndpointManager.h"    // EndpointManager
#include "UnhelloedEndpoint.h"   // Own interface



namespace ss
{



/* ****************************************************************************
*
* processLookup - 
*/
static Process* processLookup(ProcessVector* procVec, const char* alias)
{
	for (int ix = 0; ix < procVec->processes; ix++)
	{
		Process* p = &procVec->processV[ix];

		if (strcmp(p->alias, alias) == 0)
			return p;
	}

	return NULL;
}



/* ****************************************************************************
*
* UnhelloedEndpoint - 
*/
UnhelloedEndpoint::UnhelloedEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Unhelloed, _id, _name, _alias, _host, _port, _rFd, _wFd)
{
	if (init() != Endpoint2::OK)
		LM_X(1, ("Error setting up listen socket for endpoint '%s'", nameGet()));
}



/* ****************************************************************************
*
* ~UnhelloedEndpoint - 
*/
UnhelloedEndpoint::~UnhelloedEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status UnhelloedEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	Message::HelloData* helloP;

	switch (headerP->code)
	{
	case Message::Hello:
		helloP = (Message::HelloData*) dataP;
		s = helloDataAdd((Type) helloP->type, helloP->name, helloP->alias);
		if (s != OK)
		{
			state = ScheduledForRemoval;
			LM_RE(s, ("Bad hello data"));
		}

		if (header.type == Message::Msg)
		{
			if ((s = helloSend(Message::Ack)) != OK)
			{
				state = ScheduledForRemoval;
				LM_RE(s, ("helloSend error"));
			}
		}
		epMgr->add(this);
		break;

	default:
		LM_E(("I only treat Hello messages - removing endpoint"));
		state = ScheduledForRemoval;
		return Error;
	}

	return OK;
}



/* ****************************************************************************
*
* helloDataAdd - 
*/
Status UnhelloedEndpoint::helloDataAdd(Type _type, const char* _name, const char* _alias)
{
	Process*  proc;

	if (_alias == NULL)
		LM_RE(NullAlias, ("NULL alias"));

	if ((proc = processLookup(epMgr->procVec, _alias)) == NULL)
		LM_RE(BadAlias, ("alias '%s' not found in process vector", _alias));

	if (epMgr->hostMgr->match(host, proc->host) == false)  // Host could become a class and do its own matching
		LM_RE(BadHost, ("The host for alias '%s' must be '%s'. This endpoints host ('%s') is incorrect",
						_alias, hostname(), proc->host));

	if (epMgr->lookup(_type, _alias) != NULL)
		LM_RE(Duplicated, ("Duplicated process"));

	type = _type;
	nameSet(_name);
	aliasSet(_alias);

	return OK;
}



/* ****************************************************************************
*
* helloSend - 
*/
Endpoint2::Status Endpoint2::helloSend(Message::MessageType type)
{
	Message::HelloData hello;

	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   epMgr->me->nameGet(),    sizeof(hello.name));
	strncpy(hello.ip,     epMgr->me->hostname(),   sizeof(hello.ip));
	strncpy(hello.alias,  epMgr->me->aliasGet(),   sizeof(hello.alias));

	hello.type     = epMgr->me->typeGet();
	hello.coreNo   = 0;
	hello.workerId = 0;

	LM_T(LmtWrite, ("sending hello %s to '%s' (my name: '%s', my type: '%s')", messageType(type), name, hello.name, epMgr->me->typeName()));

	return send(type, Message::Hello, &hello, sizeof(hello));
}



/* ****************************************************************************
*
* helloRead - 
*
* Perhaps I should create a subclass 'TemporalEndpoint' to treat with all the
* 'Hello' stuff.
* When the helloes are exchanged, the endpoint can be incorporated as a normal
* endpoint
*/
Endpoint2::Status helloRead(void)
{
	
}

}
