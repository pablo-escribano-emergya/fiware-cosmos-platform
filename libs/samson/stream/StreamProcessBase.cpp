

#include "logMsg/logMsg.h"                 // LM_X

#include "au/CommandLine.h"				// au::CommandLine

#include "engine/Object.h"              // engine::Object
#include "engine/Notification.h"      // engine::Notification

#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

#include "PopQueue.h"                   // samson::stream::PopQueue
#include "StreamOperation.h"            // samson::stream::StreamOperation
#include "StreamProcessBase.h"			// Own interface


namespace samson {
    
    namespace stream {
        
#pragma mark ProcessItemKVGenerator
        
        StreamProcessBase::StreamProcessBase( size_t _task_id , StreamOperationBase* _streamOperation ) : ProcessIsolated( _streamOperation->operation , ProcessIsolated::key_value )
        {
            
            // Get the task_id
            task_id = _task_id;
            
            // Set the order of the task
            task_order = 0;
            
            
            // Copy queue information for this task
            streamOperation = new StreamOperationBase( _streamOperation );

            // Copy the entire environment to the operation..
            std::map<std::string,std::string>::iterator it;
            for (it = streamOperation->environment.environment.begin() ; it != streamOperation->environment.environment.end() ; it++)
                operation_environment.set( it->first , it->second );
            
            /*
            LM_M(("StreamOperation %s %s --> %d %d ( workers %d ) " , streamOperation->name.c_str() , streamOperation->operation.c_str() ,
                  (int) streamOperation->input_queues.size() , (int) streamOperation->output_queues.size() , streamOperation->num_workers ));
            */
            
            // Set number of workers in the stream operation
            setNumWorkers( _streamOperation->num_workers );
                
        }

        StreamProcessBase::~StreamProcessBase()
        {
            if( streamOperation )
                delete streamOperation;
        }
        

        void StreamProcessBase::processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish )
        {
            //LM_M(("[%s] Processing buffer %s" , streamOperation->operation.c_str(), au::str(buffer->getSize()).c_str() ));
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues[output] );
        }
        
        void StreamProcessBase::sendBufferToQueue( engine::Buffer *buffer , int outputWorker , std::string queue_name  )
        {
	  if( queue_name == "null")
	    return;

            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( 0 );
            
            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_size( buffer->getSize() );

            
            std::vector<std::string> queue_names = au::split( queue_name , ',' );
            for ( size_t i = 0 ; i < queue_names.size() ; i++)
            {
                pb->add_queue( queue_names[i] );
            }
            
            
            // Send the packet using the "notification_send_to_worker"
            engine::Notification *notification = new engine::Notification( notification_send_to_worker , packet );
            notification->environment.setInt("outputWorker", outputWorker );
            engine::Engine::shared()->notify( notification );
            
            
        }
        
        
        
        void StreamProcessBase::processOutputTXTBuffer( engine::Buffer *buffer , bool finish )
        {
            //LM_M(("[%s] Processing buffer %s" , streamOperation->operation.c_str(), au::str(buffer->getSize()).c_str() ));

            int output = 0;
            int outputWorker = -1;  // Send always to myself ( txt is not distributed )
            
            sendBufferToQueue( buffer , outputWorker , streamOperation->output_queues[output] );
            
        }
        
        
    }
}