
#ifndef _H_STREAM_PROCESS_BASE
#define _H_STREAM_PROCESS_BASE


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// samson::ProcessAssistantSharedFile
#include "au/Lock.h"				// au::Lock
#include "engine/ProcessItem.h"		// samson::ProcessItem
#include "samson/isolated/ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "samson/network/NetworkInterface.h"		// samson::NetworkInterface
#include "samson/isolated/ProcessWriter.h"			// samson::ProcessWriter
#include "samson/common/samson.pb.h"                      // samson::network::...
#include "samson/module/OperationController.h"     // samson::OperationController
#include "engine/Engine.h"                         // samson::Engine
#include "samson/isolated/SharedMemoryManager.h"            // samson::SharedMemoryManager
#include "samson/common/MemoryTags.h"                       // MemoryOutputNetwork
#include "samson/isolated/ProcessIsolated.h"                // samson::ProcessIsolated
#include "samson/stream/Queue.h"    // samson::Stream::Queue

namespace samson
{
    
    namespace stream
    {
        
        
        
        class WorkerTask;
        class WorkerTaskManager;
        class SharedMemoryItem;
        class ProcessWriter;
        class ProcessTXTWriter;
        
        /**
         A particular process that runs in as isolated mode generating key-values
         A shared memory area is used to exchange data between the background process and the foreground thread
         When necessary, a code is sent between both to flush content of this shared memory segment
         */
        
        class StreamProcessBase : public ::samson::ProcessIsolated
        {
            
        public:
            
            
            size_t task_id;                         // Id of the operation
            size_t task_order;                      // Order of the task
            
            Environment operation_environment;                // Environment of the operation
            
            // Information about how to process this queue ( from controller )
            network::StreamQueue *streamQueue;
            
            // Information about pop queue operation( parser Out from delilah )
            PopQueue *pq;
            
        public:
            
            StreamProcessBase( size_t _task_id , network::StreamQueue *streamQueue );
            StreamProcessBase( size_t _task_id , PopQueue *pq );
            virtual ~StreamProcessBase();
            
            
            // Function to be implemented ( running on a different process )
            void runIsolated();
            
            // Function to generate the key-values with the Writer element ( back process )
            virtual void generateKeyValues( KVWriter *writer ){};
            
            // Function to generate the txt buffers at the output ( back process )
            virtual void generateTXT( TXTWriter *writer ){};
            
            
            // Function to process the output of the operations
            void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish );
            void processOutputTXTBuffer( engine::Buffer *buffer , bool finish );
            
        };
        
    }
    
}

#endif