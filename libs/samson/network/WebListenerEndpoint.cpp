/* ****************************************************************************
*
* FILE                     WebListenerEndpoint.h
*
* DESCRIPTION              Class for listener endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD

#include "samson/common/status.h"
#include "samson/network/EndpointManager.h"    // EndpointManager
#include "WebWorkerEndpoint.h"                 // WebWorkerEndpoint
#include "WebListenerEndpoint.h"               // Own interface



namespace samson
{



/* ****************************************************************************
*
* ip2string - convert integer ip address to string
*/
static void ip2string(int ip, char* ipString, int ipStringLen)
{
	snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
			 ip & 0xFF,
			 (ip & 0xFF00) >> 8,
			 (ip & 0xFF0000) >> 16,
			 (ip & 0xFF000000) >> 24);
}



/* ****************************************************************************
*
* WebListenerEndpoint - 
*/
WebListenerEndpoint::WebListenerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, WebListener, _id, _host, _port, _rFd, _wFd)
{
	if (init() != OK)
		LM_X(1, ("Error setting up listen socket for endpoint '%s'", name()));
}



/* ****************************************************************************
*
* ~WebListenerEndpoint - 
*/
WebListenerEndpoint::~WebListenerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* init - 
*/
Status WebListenerEndpoint::init(void)
{
	int                 reuse = 1;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if (type != WebListener)
		LM_RE(NotListener, ("Cannot prepare an Endpoint that is not a listener ..."));

	if (rFd != -1)
		LM_W(("This listener already seems to be prepared ... Continuing anuway"));

	if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RP(SocketError, ("socket"));

	fcntl(rFd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(rFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(rFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		::close(rFd);
		rFd = -1;
		LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
	}

	if (listen(rFd, 10) == -1)
	{
		::close(rFd);
		rFd = -1;
		LM_RP(ListenError, ("listen to port %d", port));
	}

	state = Endpoint2::Ready;
	return OK;
}



/* ****************************************************************************
*
* accept - 
*/
WebWorkerEndpoint* WebListenerEndpoint::accept(void)
{
	int                 fd;
	struct sockaddr_in  sin;
	char                hostName[64];
	unsigned int        len         = sizeof(sin);
	int                 hostNameLen = sizeof(hostName);
	WebWorkerEndpoint*  ep          = NULL;

	memset((char*) &sin, 0, len);

	if ((fd = ::accept(rFd, (struct sockaddr*) &sin, &len)) == -1)
		LM_RP(NULL, ("accept"));

	ip2string(sin.sin_addr.s_addr, hostName, hostNameLen);

	Host* hostP = epMgr->hostMgr->lookup(hostName);
	if (hostP == NULL)
		hostP = epMgr->hostMgr->insert(NULL, hostName);

	ep = new WebWorkerEndpoint(epMgr, 0, hostP, 0, fd, fd);
	epMgr->add(ep);

	return ep;
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Status WebListenerEndpoint::msgTreat2(void)
{
	WebWorkerEndpoint* ep;

	ep = accept();
	if (ep == NULL)
		LM_RE(AcceptError, ("Endpoint2::accept returned NULL"));

	return OK;
}

}