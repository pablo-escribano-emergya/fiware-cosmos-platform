
/* ****************************************************************************
 *
 * FILE            gpb.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  read and write operations using google prototol buffer to comunicate using a pipe
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_GPB
#define _H_AU_GPB

#include <sys/time.h>           // struct timeval
#include <sys/types.h>          // fd_set
#include <sys/select.h>         // select, ...

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/network/Endpoint.h"           // Endpoint
#include "samson/network/iomMsgAwait.h"        // Own interface

#include <assert.h>


#include "au/status.h"  // au::Status

#include "au/namespace.h"

NAMESPACE_BEGIN(au)

Status iomMsgAwait( int fd, int secs )
{
    struct timeval  tv;
    struct timeval* tvP;
    int             fds;
    fd_set          rFds;
    
    if (secs == -1)
        tvP = NULL;
    else
    {
        tv.tv_sec  = secs;
        tv.tv_usec = 0;
        
        tvP        = &tv;
    }
    
    do
    {
        FD_ZERO(&rFds);
        FD_SET(fd, &rFds);
        fds = select(fd + 1, &rFds, NULL, NULL, tvP);
    } while ((fds == -1) && (errno == EINTR));
    
    if (fds == -1)
        LM_RP( Select , ("iomMsgAwait: select returns -1 with errno:%d for fd:%d in %d seconds", errno, fd, secs));
    else if (fds == 0)
        LM_RE( Timeout , ("iomMsgAwait: timeout in select returns 0 for fd:%d in %d seconds", fd, secs));
    else if ((fds > 0) && (!FD_ISSET(fd, &rFds)))
        LM_X(1, ("iomMsgAwait: some other fd has a read pending - this is impossible ! (select for fd:%d)", fd));
    else if ((fds > 0) && (FD_ISSET(fd, &rFds)))
        return OK;
    
    LM_X(1, ("iomMsgAwait: Other very strange error"));
    return Error;
}    


struct GPBHeader
{
    int magic;
    ssize_t size;
    
    void init( ssize_t _size )
    {
        size = _size;
        magic = 8537465;
    }
    
    bool check( )
    {
        return ( magic == 8537465 );
    }
    
};



// Read a google protocol buffer message    

template <class T>
au::Status readGPB( int fd , T** t , int time_out )
{
    
    LM_T(LmtIsolated, ("Reading a GPB message from fd:%d with timeout %d", fd, time_out ));
    
    Status iom = iomMsgAwait( fd , time_out );// Wait until this is ready
    
    if (iom != OK)
    {
        if( iom == Timeout)
	    {
	        //Trazas Goyo
	        LM_E(("readGPB(): Error(%d) in iomMsgAwait from fd:%d by Timeout after %d secs\n", iom, fd, time_out));
            return GPB_Timeout;   // Timeout
	    }
        else
	    {
	        //Trazas Goyo
	        LM_E(("readGPB(): Error(%d) in iomMsgAwait from fd:%d after %d secs\n", iom, fd, time_out));
            return iom;
	    }
    }
    LM_T(LmtIsolated,("readGPB(): iomMsgAwait returned OK on fd:%d", fd));
    
    GPBHeader header;
    int nb = read( fd , &header , sizeof(header) );
    
    if( nb == 0)
	{
	    //Trazas Goyo
	    LM_E(("readGPB(): Error reading header from fd:%d, nb(%d) == 0 bytes read\n", fd, nb));
        return GPB_ReadError;
	}
    
    if( nb < 0)
	{
	    //Trazas Goyo
	    LM_E(("readGPB(): Error reading header from fd:%d, nb(%d) < 0 bytes read\n", fd, nb));
        return GPB_ReadError;
	}
    
    if( !header.check() )
	{
	    //Trazas Goyo
	    LM_E(("readGPB(): Error corrupted header from fd:%d, nb(%d) bytes read\n", fd, nb));
        return GPB_CorruptedHeader;
	}
    
    // If not received the rigth size, return NULL
    if (nb != sizeof(header))
	{
	    //Trazas Goyo
	    LM_E(("readGPB(): Error corrupted header from fd:%d, nb(%d) != sizeof_header(%d) bytes read\n", fd, nb, sizeof(header)));
        return GPB_WrongReadSize;   // Error reading the size
	}
    
    //LM_M(("Reading a GPB message from fd:%d (size:%d)", fd , (int) size ));
    
    if( header.size > 1000000)
    {
        LM_W(("Large size %l for a background process message",header.size));
        //return 3;    // Too much bytes to read
    }
    
    void *data  = (void*) malloc( header.size );
    
    nb = read( fd , data , header.size );
    
    if( nb != header.size )
    {
        free( data );
        return GPB_ReadError;
    }
    
    *t = new T();
    bool parse = (*t)->ParseFromArray( data , header.size );
    
    free( data );
    
    if( !parse )
    {
        LM_W(("Error parsing a GPB message of %d bytes", header.size));
        delete (*t);
        return GPB_ReadErrorParsing; // Error parsing the document
    }
    
    return OK;   // Correct
    
}

// Write a google protocol buffer message    

template <class T>
Status writeGPB( int fd , T* t  )
{
    LM_T(LmtIsolated,("Writing a GPB message to fd:%d ( Size:%d )", fd , (int) t->ByteSize() ));
    
    if( !t->IsInitialized()  )
        return GPB_NotInitializedMessage;
    
    GPBHeader header;
    header.init(  t->ByteSize() );
    
    if ( header.size > 100000 )
    {
        LM_W(("Large size %l for a background process message",header.size));
        //return 3;
    }
    
    void *data = (void*) malloc( header.size );
    
    bool serialize = t->SerializeToArray( data , header.size );
    
    if( !serialize )
    {
        free( data );
	    LM_E(("Error serializing data, header.size:%d, preparing for fd:%d\n", header.size, fd));
        return GPB_WriteErrorSerializing;
    }
    
    // If not received the rigth size, return NULL
    int nw = write( fd , &header , sizeof(header) );
    if (nw != sizeof(header))
    {
        free( data );
	    LM_E(("Error writing header: nw(%d) != sizeof(header)(%d) on fd:%d\n", nw, sizeof(header), fd));
        return GPB_WriteError;
    }
    
    // If not received the rigth size, return NULL
    nw = write( fd , data , header.size );
    if (nw != header.size)
    {
        free( data );
	    LM_E(("Error writing data: nw(%d) != header.size(%d) on fd:%d\n", nw, header.size, fd));
        return GPB_WriteError;
    }
    
    free( data );
    
    return OK;
} 

NAMESPACE_END

#endif