/* ****************************************************************************
*
* FILE                     Message.cpp - message definitions for all Samson IPC
*
*/
#include "samson/network/Message.h"            // Own interface

namespace samson
{
    namespace Message
    {
        
        
        /* ****************************************************************************
         *
         * messageCode - 
         */
        char* messageCode(MessageCode code)
        {
            switch (code)
            {
                case Hello:                            return (char*) "Hello";
                    
                case NetworkNotification:              return (char*) "NetworkNotification";    
                    
                case StatusReport:                     return (char*) "StatusReport";
                case Alert:                            return (char*) "Alert";
                    
                case PushBlock:                        return (char*) "PushBlock";
                case PushBlockResponse:                return (char*) "PushBlockResponse";
                case PopQueue:                         return (char*) "PopQueue";
                case PopQueueResponse:                 return (char*) "PopQueueResponse";
                case StreamOutQueue:                   return (char*) "StreamOutQueue";
                case WorkerCommand:                    return (char*) "WorkerCommand";
                case WorkerCommandResponse:            return (char*) "WorkerCommandResponse";
                    
                case Message:                          return (char*) "Message";

                case Unknown:                          return (char*) "Unknown";
            }
            
            return (char*) "Unknown";
        }
        
    }
}