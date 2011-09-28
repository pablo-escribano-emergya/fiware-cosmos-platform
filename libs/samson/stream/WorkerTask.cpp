
#include <algorithm>

#include "engine/Notification.h"
#include "engine/ProcessManager.h"  


#include "samson/common/EnvironmentOperations.h"        // copyEnvironment
#include "samson/common/SamsonSetup.h"                  // SamsonSetup

#include "samson/network/Packet.h"                      // samson::Packet

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/StreamManager.h"                // samson::stream::StreamManager
#include "samson/stream/Queue.h"
#include "samson/stream/QueueItem.h"
#include "samson/stream/BlockList.h"

#include "samson/stream/ParserQueueTask.h"

#include "WorkerTask.h"     // Own interface

namespace samson {
    namespace stream
    {
        WorkerCommand::WorkerCommand( int _fromId , size_t _delilah_id ,  const network::WorkerCommand& _command )
        {
            streamManager = NULL;
            
            //Identifiers to notify when finished
            fromId = _fromId;
            delilah_id = _delilah_id;
            notify_finish = true;
            
            // Copy the original message
            originalWorkerCommand = new network::WorkerCommand();
            originalWorkerCommand->CopyFrom( _command );
            
            // Extract enviroment properties
            copyEnviroment( originalWorkerCommand->environment() , &enviroment );
            
            // Extract command for simplicity
            command = originalWorkerCommand->command();
            
            // Parse command
            cmd.set_flag_boolean("clear_inputs");
            cmd.set_flag_uint64("min_size", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_uint64("max_latency", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            
            cmd.parse( command );
            
            // Original value for the falg
            finished = false;
            
            // No pending process at the moment
            num_pending_processes = 0;
        }
        
        WorkerCommand::WorkerCommand( std::string _command )
        {
            streamManager = NULL;
            
            // Not necessary to notify
            originalWorkerCommand = NULL;
            notify_finish = false;
            
            // Get directly the command to run
            command = _command;
            
            // Parse command
            cmd.set_flag_boolean("clear_inputs");
            cmd.set_flag_uint64("min_size", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.set_flag_uint64("max_latency", 0);         // Set a minimum size to run ( necessary for automatoc maps / parser / reduce / ....
            cmd.parse( command );
            
            // Original value for the falg
            finished = false;
            
            // No pending process at the moment
            num_pending_processes = 0;
        }
        
        WorkerCommand::~WorkerCommand()
        {
            if( originalWorkerCommand )
                delete originalWorkerCommand;
        }
        
        void WorkerCommand::setStreamManager( StreamManager * _streamManager )
        {
            streamManager = _streamManager;
        }
        
        
        bool WorkerCommand::isFinished()
        {
            return finished;
        }
        
        void WorkerCommand::run()
        {
            
            // Main command
            std::string main_command = cmd.get_argument(0);
            
            if( main_command == "reload_modules" )
            {
                // Spetial operation to reload modules
                ModulesManager::shared()->reloadModules();
                finishWorkerTask();
                return;
            }
            
            if ( cmd.get_argument(0) == "rm_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    for ( int i = 1 ; i < cmd.get_num_arguments() ; i++ )
                    {
                        std::string queue_name = cmd.get_argument(i);
                        streamManager->remove_queue(  queue_name );
                    }
                    finishWorkerTask();
                }
                
                return;
            }

            if ( cmd.get_argument(0) == "cp_queue" )
            {
                if( cmd.get_num_arguments() < 3 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string from_queue_name = cmd.get_argument(1);
                    std::string to_queue_name = cmd.get_argument(2);
                    
                    streamManager->cp_queue(  from_queue_name , to_queue_name );
                    
                    finishWorkerTask();
                }
                
                return;
            }

            if ( cmd.get_argument(0) == "set_queue_property" )
            {
                if( cmd.get_num_arguments() < 4 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    std::string property = cmd.get_argument(2);
                    std::string value = cmd.get_argument(3);

                    Queue *queue = streamManager->getQueue( queue_name );
                    queue->setProperty( property , value );
                    
                    finishWorkerTask();
                }
                
                return;
            }            
            
            
            
            if ( cmd.get_argument(0) == "pause_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    streamManager->pause_queue( queue_name );
                    finishWorkerTask();
                }
                return;
            }
            
            if ( cmd.get_argument(0) == "play_queue" )
            {
                if( cmd.get_num_arguments() < 2 )
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                else
                {
                    std::string queue_name = cmd.get_argument(1);
                    streamManager->play_queue( queue_name );
                    finishWorkerTask();
                }
                
                return;
            }

            if( cmd.get_argument(0) == "run_stream_update_state" )
            {
                // Parameters used to thrigger or not the automatic-update
                size_t min_size = cmd.get_flag_uint64("min_size");          // Minimum size to run an operation
                size_t max_latency = cmd.get_flag_uint64("max_latency");    // Max acceptable time to run an operation
                
                // Operation size    
                size_t default_size = SamsonSetup::shared()->getUInt64("general.memory") / SamsonSetup::shared()->getUInt64("general.num_processess");
                size_t operation_size = (min_size!=0)?min_size:default_size;
                
                // Spetial command to update a state with a blocking reduce operation
                // Usage: run_stream_update_state operation input state
                
                if( cmd.get_num_arguments() < 4 )
                {
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                    return;
                }
                
                std::string operation_name = cmd.get_argument(1);
                std::string input_name = cmd.get_argument(2);
                std::string state_name = cmd.get_argument(3);
                
                Operation *op = ModulesManager::shared()->getOperation(  operation_name );
                if( !op )
                {
                    finishWorkerTaskWithError( au::str("Unknown operation %s" , operation_name.c_str() ) ); 
                    return;
                }
                if( ( op->getType() != Operation::reduce ) || (op->getNumInputs() != 2 ) )
                {
                    finishWorkerTaskWithError( au::str("Only reduce operations with 2 inputs for run_stream_update_state operations" ) ); 
                    return;
                }
                if( op->getInputFormat( op->getNumInputs()-1 )  != op->getOutputFormat( op->getNumOutputs()-1 ) )
                {
                    finishWorkerTaskWithError( au::str("%s is not a valid reduce operation since last input and output formats are not the same" ) ); 
                    return;
                }
                
                Queue *state = streamManager->getQueue( state_name );
                Queue *input = streamManager->getQueue( input_name );
                
                
                // Checking formats for the inputs and state
                KVFormat input_queue_format = input->format;
                KVFormat operation_input_format = op->getInputFormat(0);
                if( input_queue_format != KVFormat("*","*") )
                    if( input_queue_format != operation_input_format )
                    {
                        finishWorkerTaskWithError( au::str("Not valid input format %s != %s" , input_queue_format.str().c_str() , operation_input_format.str().c_str()  ) );
                        return;
                    }

                KVFormat state_queue_format = state->format;
                KVFormat operation_state_format = op->getInputFormat(1);
                if( state_queue_format != KVFormat("*","*") )
                    if( state_queue_format != operation_state_format )
                    {
                        finishWorkerTaskWithError( au::str("Not valid state format %s != %s" , state_queue_format.str().c_str() , operation_state_format.str().c_str()  ) );
                        return;
                    }
                
                
                // Check what operations can be done....
                state->setMinimumNumDivisions();
                int num_divisions = state->num_divisions;
                
                // Make sure, input is divided at least as state
                if( input->num_divisions < state->num_divisions)
                    input->num_divisions = state->num_divisions;
                
                for ( int i = 0 ; i < num_divisions ; i++)
                {
                    KVRange range = rangeForDivision( i , num_divisions );
                    
                    BlockList *stateBlockList = state->getStateBlockListForRange( range );
                    BlockList *inputBlockList = NULL;
                    
                    if ( stateBlockList )
                    {
                        
                        BlockInfo state_block_info = stateBlockList->getBlockInfo();
                        
                        
                        inputBlockList = input->getInputBlockListForRange( range , operation_size );
                        BlockInfo input_block_info = inputBlockList->getBlockInfo();
                        size_t latency = input_block_info.min_time_diff();

                        //LM_M(("Considering a reduce-state operation size %lu ( min %lu ) latency %lu ( max %lu )" , input_block_info.size , min_size , latency , max_latency ));

                        // Let see if operation is canceled for small size
                        
                        bool cancel_operation = false;
                        
                        if( input_block_info.size < operation_size )
                        {
                            //LM_M(("Caneled operation for size"));
                            cancel_operation = true;
                        }
                        
                        if( cancel_operation )
                        {
                            if ( (max_latency > 0) && ( latency > max_latency) )
                            {
                                //LM_M(("Reduce state operation reopened for latency %lu max %lu" , latency , max_latency));
                                cancel_operation = false;
                            }
                            
                        }
                        
                        // with no data, remove always
                        if( input_block_info.size  == 0 )
                            cancel_operation = true;
                         
                        // TODO: If size is not enougth, we can cancel operation here...
                        if( !cancel_operation )
                        {
                            network::StreamOperation *operation = getStreamOperation( op );
                            
                            ReduceQueueTask * task = new ReduceQueueTask( streamManager->queueTaskManager.getNewId() , *operation , range );
                            task->addOutputsForOperation(op);
                            
                            // Spetial flag to indicate update_state mode ( process different output buffers )
                            task->update_state_mode = true;
                            
                            task->getBlockList("input_0")->copyFrom( inputBlockList );
                            task->getBlockList("input_1")->copyFrom( stateBlockList );
                            
                            // remove input from the original queue
                            input->list->remove( inputBlockList );
                            
                            // Set the working size to get statictics at ProcessManager
                            task->setWorkingSize();
                            
                            // Add me as listener and increase the number of operations to run
                            task->addListenerId( getEngineId() );
                            num_pending_processes++;
                            
                            // Schedule tmp task into QueueTaskManager
                            streamManager->queueTaskManager.add( task );
                            
                            delete operation;
                        }
                        else
                        {
                            // Unlock state since it has not been used un any operation
                            state->unlockStateBlockList( stateBlockList );
                        }
                        
                    }
                    else
                    {
                        //LM_M(("State not ready in range %s" , range.str().c_str() ));
                    }
                    
                    // Remove list used here
                    if( inputBlockList)
                        delete inputBlockList;
                    if( stateBlockList )
                        delete stateBlockList;
                    
                }
                
                if( num_pending_processes == 0 )
                    finishWorkerTask();
                
                return;
            }
            
            if( cmd.get_argument(0) == "run_stream_operation" )
            {
                bool clear_inputs =  cmd.get_flag_bool("clear_inputs");
                
                size_t min_size = cmd.get_flag_uint64("min_size");          // Minimum size to run an operation
                size_t max_latency = cmd.get_flag_uint64("max_latency");    // Max acceptable time to run an operation
                
                // Operation size    
                size_t default_size = SamsonSetup::shared()->getUInt64("general.memory") / SamsonSetup::shared()->getUInt64("general.num_processess");
                size_t operation_size = (min_size!=0)?min_size:default_size;
                
                //LM_M(("Worker command %s --> min size %lu" , command.c_str() , min_size));
                
                if( cmd.get_num_arguments() < 2 )
                {
                    finishWorkerTaskWithError( au::str("Not enougth parameters for command %s" , main_command.c_str() ) ); 
                    return;
                }
                
                std::string operation_name = cmd.get_argument(1);
                
                Operation *op = ModulesManager::shared()->getOperation(  operation_name );
                if( !op )
                {
                    finishWorkerTaskWithError( au::str("Unknown operation %s" , operation_name.c_str() ) ); 
                    return;
                }
                
                // Check input output arguments...
                if ( cmd.get_num_arguments() <  ( 2 + ( op->getNumInputs() + op->getNumOutputs() ) ) )
                {
                    finishWorkerTaskWithError( au::str("Not enougth arguments for operation %s" , operation_name.c_str() ) ); 
                    return;
                }

                // Check formats
                int pos_argument = 2; 
                for (int i = 0 ; i < op->getNumInputs() ; i++ )
                {
                    KVFormat operation_format = op->getInputFormat(i);
                    std::string queue_name = cmd.get_argument(pos_argument++);
                    KVFormat queue_format = streamManager->getQueue( queue_name )->format;
                    
                    if( queue_format != KVFormat("*","*") )
                        if( queue_format != operation_format )
                        {
                            finishWorkerTaskWithError( au::str("Format for input %d ( operation %s ) is %s while queue %s has format %s" 
                                                               , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                            return;
                        }
                    
                }
                for (int i = 0 ; i < op->getNumOutputs() ; i++ )
                {
                    KVFormat operation_format = op->getOutputFormat(i);
                    std::string queue_name = cmd.get_argument(pos_argument++);
                    KVFormat queue_format = streamManager->getQueue( queue_name )->format;
                    
                    if( queue_format != KVFormat("*","*") )
                        if( queue_format != operation_format )
                        {
                            finishWorkerTaskWithError( au::str("Format for output %d ( operation %s ) is %s while queue %s has format %s" 
                                                               , i , operation_name.c_str() , operation_format.str().c_str() , queue_name.c_str() , queue_format.str().c_str() ) );
                            return;
                        }
                }
                
                switch (op->getType()) {
                        
                    case Operation::reduce:
                    {
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queues
                        std::vector< Queue* > queues;
                        BlockInfo block_info;
                        int num_divisions = 1;
                        
                        
                        for (int i = 0 ; i < op->getNumInputs() ; i++ )
                        {
                            std::string input_queue_name = operation->input_queues(i);
                            Queue*queue = streamManager->getQueue( input_queue_name );
                            queue->update( block_info );
                            queues.push_back( queue );
                            
                            if( queue->num_divisions > num_divisions )
                                num_divisions = queue->num_divisions;
                        }
                        
                        // For each range, create a set of reduce operations...
                        for ( int r = 0 ; r < num_divisions ; r++ )
                        {
                            KVRange  range = rangeForDivision( r , num_divisions );
                            
                            
                            //Create the reduce operation
                            ReduceQueueTask *tmp = new ReduceQueueTask( streamManager->queueTaskManager.getNewId() , *operation , range ); 
                            tmp->addOutputsForOperation(op);
                            
                            // Take data from each input
                            for (int q = 0 ; q < (int) queues.size() ; q++ )
                            {
                                BlockList* list = tmp->getBlockList( au::str("input_%d" , q ) ); 
                                list->copyFrom( queues[q]->list , range );
                            }
                            
                            // Set the working size to get statictics at ProcessManager
                            tmp->setWorkingSize();
                            
                            // Add me as listener and increase the number of operations to run
                            tmp->addListenerId( getEngineId() );
                            num_pending_processes++;
                            
                            // Schedule tmp task into QueueTaskManager
                            streamManager->queueTaskManager.add( tmp );
                            
                        }
                        
                        
                        delete operation;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        
                        return;
                    }
                        break;             
                        
                    case Operation::parser:
                    case Operation::parserOut:
                    case Operation::map:
                    {
                        
                        network::StreamOperation *operation = getStreamOperation( op );
                        
                        // Get the input queue
                        std::string input_queue_name = operation->input_queues(0);
                        Queue *queue = streamManager->getQueue( input_queue_name );
                        
                        
                        bool cancel_operation = false;
                        while( !cancel_operation && !queue->list->isEmpty() )
                        {
                            // Create the map operation
                            size_t id = streamManager->queueTaskManager.getNewId();
                            

                            QueueTask *tmp = NULL;

                            switch ( op->getType() ) {
                                case Operation::parser:
                                {
                                    tmp = new ParserQueueTask( id , *operation ); 
                                }
                                    break;

                                case Operation::map:
                                {
                                    tmp = new MapQueueTask( id , *operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                }
                                    break;

                                case Operation::parserOut:
                                {
                                    tmp = new ParserOutQueueTask( id , *operation , KVRange(0,KVFILE_NUM_HASHGROUPS) ); 
                                }
                                    break;
                                    
                                default:
                                    LM_X(1,("Internal error"));
                                    break;
                            }
                            
                            
                            tmp->addOutputsForOperation(op);
                            
                            // Extract the rigth blocks from queue
                            tmp->getBlockList("input_0")->copyFrom( queue->list , operation_size );
                            tmp->setWorkingSize();

                            BlockInfo operation_block_info = tmp->getBlockList("input_0")->getBlockInfo();
                            //LM_M(("Operation size %lu Min size %lu Latency %lu ( max %lu )" , operation_block_info.size , min_size , operation_block_info.min_time_diff() , max_latency ));
                            
                            // If there is not enougth size, do not run the operation
                            if( (min_size>0) && ( operation_block_info.size < min_size ) )
                            {
                                //LM_M(("Parse operation canceled for small size %s ( min size %s )", au::str( operation_block_info.size ).c_str() , au::str(min_size).c_str()));
                                cancel_operation = true;
                            }
                            
                            if( cancel_operation )
                            {
                                // Check if latency is too high....
                                if( ( max_latency > 0 ) && ( (size_t)operation_block_info.min_time_diff() > max_latency ) )
                                {
                                    //LM_M(("Parse operation reopen for high latency %lu ( max %lu )", (size_t) operation_block_info.min_time_diff() , max_latency ));
                                    cancel_operation = false;       // No cancel since there is too much latency
                                }
                            }
                            
                            if( cancel_operation )
                            {
                                // Delete the operation since it has been canceled
                                delete tmp;
                            }
                            else
                            {
                                // Remove from the original queue
                                if( clear_inputs )
                                    queue->list->remove( tmp->getBlockList("input_0") );
                                
                                // Add me as listener and increase the number of operations to run
                                tmp->addListenerId( getEngineId() );
                                num_pending_processes++;
                                
                                // Schedule tmp task into QueueTaskManager
                                streamManager->queueTaskManager.add( tmp );
                            }                     
                            
                        }
                        
                        delete operation;
                        if( num_pending_processes == 0 )
                            finishWorkerTask();
                        
                        return;
                    }
                        break;
                        
                        
                    default:
                    {
                        finishWorkerTaskWithError( au::str("run_stream_operation OK but it is still not implemented"  ) ); 
                    }
                        break;
                }
                
                
            }
            
            finishWorkerTaskWithError( au::str("Unknown command %s" , main_command.c_str()  ) ); 
            
        }
        
        void WorkerCommand::finishWorkerTaskWithError( std::string error_message )
        {
            error.set( error_message );
            finishWorkerTask();
        }
        
        void WorkerCommand::finishWorkerTask()
        {
            if( finished )
            {
                LM_W(("Trying to send a finish message for an already finished workerTask"));
                return;
            }
            
            if ( notify_finish )
            {
                Packet*           p = new Packet( Message::WorkerCommandResponse );
                network::WorkerCommandResponse* c = p->message->mutable_worker_command_response();
                c->mutable_worker_command()->CopyFrom( *originalWorkerCommand );
                
                // Put the error if any
                if( error.isActivated() )
                    c->mutable_error()->set_message( error.getMessage() );
                
                // Set delilah id
                p->message->set_delilah_id( delilah_id );
                
                // Send the packet
                streamManager->worker->network->send( fromId , p );
            }
            
            // Set the finished flag
            finished = true;
            
        }
        
        network::StreamOperation *WorkerCommand::getStreamOperation( Operation *op )
        {
            
            int pos_argument = 1;   // We skipt the "run_stream_operation" or "run_stream_update_state" parameter
            
            // Create the StreamOperation elements
            network::StreamOperation *operation  = new network::StreamOperation();
            operation->set_name("Manual operation");
            operation->set_operation( cmd.get_argument(pos_argument++) );
            
            
            for (int i = 0 ; i < op->getNumInputs() ; i++)
                operation->add_input_queues( cmd.get_argument( pos_argument++ ) );
            for (int i = 0 ; i < op->getNumOutputs() ; i++)
                operation->add_output_queues( cmd.get_argument( pos_argument++ ) );
            operation->set_num_workers( streamManager->worker->network->getNumWorkers() ); 
            
            return operation;
            
        }
        
        void WorkerCommand::notify( engine::Notification* notification )
        {
            if( notification->isName( notification_process_request_response ) )
            {
                num_pending_processes--;
                
                if( notification->environment.isSet("error") )
                    error.set( notification->environment.get("error" , "no_error") );
                
                checkFinish();
                return;
            }
            
            LM_W(("Unexpected notification at WorkerCommand"));
            
        }
        
        void WorkerCommand::checkFinish()
        {
            if( error.isActivated() )
                finishWorkerTask();
            else if( num_pending_processes <= 0 )
                finishWorkerTask();
        }

        void WorkerCommand::getInfo( std::ostringstream& output)
        {
            au::xml_open(output ,  "worker_command" );
            
            au::xml_simple( output , "command" , command );
            
            au::xml_close(output ,  "worker_command" );
        }
        
        
    }
}
