#include <stdio.h>
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <errno.h>

#include "logMsg/logMsg.h"          // LM_X
#include "logMsg/traceLevels.h"

#include "au/TokenTaker.h"

#include "SocketConnection.h" // Own interface

namespace au 
{
    
    SocketConnection::SocketConnection( int _fd , std::string _host , int _port ) : 
    FileDescriptor(  (_port==-1)?_host:au::str("%s:%d" , _host.c_str() , _port ) ,  _fd ) 
    {
        host = _host;
        port = _port;
    }
    
    // Debug information information
    std::string SocketConnection::str()
    {
        return au::str("Socket %s (fd=%d)" , getHostAndPort().c_str() , getFd() );
    }
        
    std::string SocketConnection::getHost()
    {
        return host;
    }
    
    int SocketConnection::getPort()
    {
        return port;
    }
    
    std::string SocketConnection::getHostAndPort()
    {
        if( port != -1 )
            return au::str("%s:%d" , host.c_str() , port );
        else
            return au::str("%s" , host.c_str() );
        
    }

    
    Status SocketConnection::newSocketConnection( std::string host , int port , SocketConnection** socket_connection )
    {
        int fd;
        struct hostent*     hp;
        struct sockaddr_in  peer;
        
        if ((hp = gethostbyname( host.c_str() )) == NULL )
            LM_RE(GetHostByNameError, ("gethostbyname(%s): %s", host.c_str() , strerror(errno)));
        
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            LM_RE(SocketError, ("socket: %s", strerror(errno)));
        
        memset((char*) &peer, 0, sizeof(peer));
        
        peer.sin_family      = AF_INET;
        peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
        peer.sin_port        = htons(port);
        
        LM_T(LmtSocketConnection, ("Connecting to worker at %s:%d", host.c_str(), port));

        // Try several times....
        int retries = 10;
        int tri     = 0;
        
        while (1)
        {
            if ( ::connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1 )
            {
                usleep(50000);
                if (tri > retries)
                {
                    ::close(fd);
                    LM_T(LmtSocketConnection,("Cannot connect to %s, port %d (even after %d retries)", host.c_str(), port, retries));
                    return ConnectError;
                }
                
                ++tri;
            }
            else
                break;
        }
        
        *socket_connection = new SocketConnection( fd , host, port ); 
        return OK;
    }
    
    
}