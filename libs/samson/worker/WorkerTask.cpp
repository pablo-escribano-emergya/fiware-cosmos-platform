
#include "au/Descriptors.h"         // au::Descriptors


#include "engine/MemoryManager.h"				// samson::MemoryRequest
#include "engine/Engine.h"						// samson::Engine
#include "engine/DiskOperation.h"				// samson::DiskOperation
#include "engine/Buffer.h"                     // engine::Buffer
#include "engine/ProcessManager.h"             // Notifications
#include "engine/DiskManager.h"                // Notifications
#include "engine/Notification.h"                // engine::Notification


#include "WorkerTask.h"			// OwnInterface
#include "WorkerTaskManager.h"			// samson::WorkerTaskManager
#include "samson/network/Packet.h"						// samson::Packet
#include "WorkerSubTask.h"				// samson::WorkerSubTask
#include "BufferVector.h"
#include "samson/worker/SamsonWorker.h"				// samson::SamsonWorker
#include "DataBufferProcessItem.h"		// samson::DataBufferProcessItem

namespace samson
{
	
	WorkerTask::WorkerTask( WorkerTaskManager *_taskManager )
	{
        num_file_output = 0;            // Init this counter to give name to the generated files
            
		taskManager = _taskManager;		// Pointer to the task manager
		reduceInformation = NULL;		// By default this is not used
		
		// Initial status of this operation
		status = pending_definition;
		
		// Init the counter to give an id to each sub tasks
		subTaskId  = 0;
		
		// Status of confirmation from all the workers
		num_workers = taskManager->worker->network->getNumWorkers();
		num_finished_workers = 0;
		
        // Init counter of pending things
        num_process_items = 0;
        num_disk_operations = 0;
		num_subtasks = 0;
        
		complete_message = NULL;
        
        // Add as a listener for notification_sub_task_finished
        // listen( notification_sub_task_finished );
	}
	
	WorkerTask::~WorkerTask()
	{
		if( reduceInformation )
			delete reduceInformation;
		
		if( complete_message )
			delete complete_message;
        
        // Remove pending tasks ( if canceled before any of them finish )
        subTasks.clearMap();
        
        // Remove buffers in the output queues for killed jobs
        queueBufferVectors.clearMap();
        
        // Clear colection of output files ( delete is called for each one )
        outputFiles.clearMap(); 
        outputRemoveFiles.clearMap();
	}
	
	
	void WorkerTask::setupAndRun(Operation::Type _type , const network::WorkerTask &task)
	{
		if( status != pending_definition)
			LM_X(1,("Internal error: Unexpected status of a WorkerTask at setup" ));
		
		// Copy of the message form the controller
		workerTask = task;
        
		// Copy the type of operation
		type = _type;
		
		// Get the operation and task_id from the message
		operation = task.operation();	// Save the operation to perform		
		task_id = task.task_id();		// Save the task id
		job_id = task.job_id();		// Save the job id
		
		// Messages prepared to be send to the controller
        //LM_M(("Worker task : Operation %s" , operation.c_str()));
		
		complete_message = new network::WorkerTaskConfirmation();
		complete_message->set_task_id( task_id );
		complete_message->set_type( network::WorkerTaskConfirmation::complete );
		
		// Set status to runnign until all the tasks are completed ( or error )
		status = running;
		
		switch (type) {
				
			case Operation::generator :
            {
                // Generate num_cors generators
                size_t num_generator_operations = SamsonSetup::shared()->getInt("general.num_processess");
                
                for ( size_t i = 0 ; i < num_generator_operations ; i++)
				{
					GeneratorSubTask * tmp = new GeneratorSubTask( this , i , num_generator_operations );
					addSubTask( tmp );
				}
				break;
            }
				
			case Operation::parser:
			{
				// An item per file
				if( workerTask.input_queue_size() != 1)
					LM_X(1,("Internal error: Trying to parse multiple inputs in one operation"));
				
				for (size_t i = 0 ; i < (size_t) workerTask.input_queue(0).file_size() ; i++)
                {
                    //LM_M(("Added subtask %s" , workerTask.input_queue(0).file(i).name().c_str()));
					addSubTask( new ParserSubTask( this, workerTask.input_queue(0).file(i).name() ) );
                }
			}
				break;
				
			case Operation::map :
			case Operation::parserOut :
			case Operation::parserOutReduce :
			case Operation::reduce :
			{
				OrganizerSubTask * tmp = new OrganizerSubTask( this );
				addSubTask( tmp );
			}
				break;
				
				
			case Operation::system:
			{
				// Spetial system operations 
				SystemSubTask *tmp = new SystemSubTask( this );
				addSubTask( tmp );
			}
				break;
				
			default:
				LM_X(1,("Internal error"));
				break;
		}
		
		
		// For those tasks with any particular task
		check();	
	}
	
	void WorkerTask::addSubTask( WorkerSubTask *subTask )
	{
        if( !subTask )
            LM_X(1,("WorkerTask tried to add a NULL subTask"));
        
		// Information about ids
        subTask->task_id = task_id;
		subTask->sub_task_id = subTaskId++;
        
		subTasks.insertInMap(subTask->sub_task_id , subTask);
        num_subtasks++;
        
        subTask->run();   // Start the subtask querying a memory request / read operation / process / etc
	}
    
    void WorkerTask::notify( engine::Notification* notification )
    {
        //LM_M(("WorkerTask: %s", notification->getDescription().c_str() ));
        
        if( notification->isName(notification_disk_operation_request_response) )
        {
            if( !notification->containsObject() )
                LM_X(1,("Error since WorkerTasks receive a notification_disk_operation_request_response without an object '%s'",notification->getDescription().c_str() ));
            else
            {
                
                engine::DiskOperation *diskOperation =  (engine::DiskOperation*) notification->extractObject();
                diskOperation->destroyBuffer();
                delete diskOperation;
                
                // Internal operations to process this finish
                num_disk_operations--;
                check();
                
            }
        }
        else if( notification->isName( notification_process_request_response ) )
        {
            num_process_items--;
            
            if ( !notification->containsObject() )
                LM_W(("WorkerTask receive a notification_process_request_response without an object"));
            else
            {
                
                DataBufferProcessItem* tmp = (DataBufferProcessItem*) notification->extractObject();
                
                // New file to be saved
                std::string queue_name = tmp->bv->queue->name();
                std::string fileName = newFileName( );
                
                // Add a file as output for this operation
                addKVFile(fileName, queue_name, tmp->buffer);
                
                delete tmp;
            }
            
        }
        else if( notification->isName(notification_sub_task_finished) )
        {
            
            size_t sub_task_id = notification->environment.getSizeT("sub_task_id", 0);
            WorkerSubTask *subTask = subTasks.findInMap( sub_task_id );

            //LM_M(("Subtask finished notification %lu %lu. Pending subtasks %d", task_id , sub_task_id , num_subtasks));
            
            if( subTask )
            {
                
                //LM_W(("Subtask finished %lu ( %s )" , task_id , subTask->error.str().c_str() ));

                num_subtasks--;
                
                // Copy the error if necessary
                if( subTask->error.isActivated() )
                    setError( subTask->error.getMessage() );

                // Release resources
                subTask->releaseResources();
                
                // Not delite until the end of the task ( new policy )
                // delete subTasks.extractFromMap(sub_task_id);
            }
            else
            {
                LM_W(("Subtask finished and not found in a vector of %d positions", (int)subTasks.size() ));
            }
            
            check();
        }
        else
            LM_X(1, ("WorkerTask received an unexpected notification"));
    }
	
	void WorkerTask::check()
	{
		
		/**
		 Evolution of the status
		 
		 pending_definition,			// Pending to receive message from the controller
		 running,                       // Running operation
		 local_content_finished,		// Output content is completed ( a message is send to the other workers to notify ) 
		 all_content_finish,			// The content from all the workers is received ( file are starting to be saved )
		 finish,						// All the output files are generated ( not saved ). Controller is notified about this to continue scripts
		 completed                      // Output content is saved on disk ( task can be removed from task manager )
		 */
		
        if( status == pending_definition) 
            return; // Nothing to do until the task is defined
        
		if ( status == running )
		{
			// If no more tasks, then set to finish and send a message to the rest of workers
			if( num_subtasks == 0 )
            {				
                // Send a close message to all the workers
                sendCloseMessages();
                status = local_content_finished;
                //LM_M(("Task %lu: Status changed to local content finished", task_id));
            }
		}
		
		if( status == local_content_finished )
		{
			if( num_finished_workers == num_workers )
			{
				// FLush to emit all the pending buffers to a file
				flush();
				
				// Now the status is waiting for the all the workers confirm ( me included ) finish generating data
				status = all_content_finish;	
                //LM_M(("Task %lu: Status changed to all content finished", task_id));

			}
		}
		
		
		if (status == all_content_finish )
		{
			if( num_process_items == 0 )	 // No more files to be generated
			{
				status = finish;
				
				// Send a message indicating that operation finished
				sendFinishTaskMessageToController();
                //LM_M(("Task %lu: Status changed to finished", task_id));
			}
		}
		
		
		if ( status == finish)
			if( num_disk_operations == 0 )
			{
				// If no more elements to write
				status = completed;
				
				//Send a message to the controller to notify about this
				sendCompleteTaskMessageToController();
                //LM_M(("Task %lu: Status changed to completed", task_id));
			}
		
		
		
		// Spetial condition
		if( status != completed)
			if( error.isActivated() )
			{
				status = completed;
				
				// Send complete message with error
				sendCompleteTaskMessageToController();
				
			}
		
	}
		
	void WorkerTask::finishWorker( int worker_from )
	{
        
		num_finished_workers++;
        
        /*
        LM_M(("Finish worker message received for task %lu from worker_from %d. Now the state is %d / %d workers have finished producing content ", 
                task_id , worker_from , num_finished_workers , num_workers ));
         */
        
		check();
	}
	
    
    const char* getStatusString( WorkerTask::WorkerTaskStatus status )
    {
		switch (status) {
			case WorkerTask::pending_definition:
                return "definition";
				break;
			case WorkerTask::running:
                return "running";
				break;
			case WorkerTask::finish:
                return "finish";
				break;
			case WorkerTask::completed:
                return "completed";
				break;
			case WorkerTask::local_content_finished:
                return "local content finish";
				break;
			case WorkerTask::all_content_finish:
                return "all content finish";
				break;
		}
        
        return "Unknown";
    }
    
    void WorkerTask::getInfo( std::ostringstream& output)
	{
        
        au::xml_open(output , "worker_task");
        
        au::xml_simple( output , "task_id" , task_id );

        au::xml_simple( output , "operation" , operation );
        

        au::xml_simple( output , "status" , getStatusString(status) );

        if( status != completed )
        {
            au::xml_simple( output , "num_workers" , num_workers );
            au::xml_simple( output , "num_finished_workers" , num_finished_workers );
        }
        
        if ( status == running )
        {
            au::xml_simple( output , "num_disk_operations" , num_disk_operations );
            au::xml_simple( output , "num_process_items" , num_process_items );
            au::xml_simple( output , "num_subtasks" , num_subtasks ); 
        }
        
        if( canBeRemoved() )
            au::xml_simple( output , "can_be_removed" , "yes" );
        else
            au::xml_simple( output , "can_be_removed" , "NO" );
        
        au::xml_iterate_map( output , "worker_subtasks" , subTasks );
        
        au::xml_close(output , "worker_task");
	}
	
	

	
#pragma mark Buffers processing
	
	void WorkerTask::addBuffer( network::WorkerDataExchange& workerDataExchange , engine::Buffer *buffer )
	{
        //LM_M(("Received a buffer from worker %d hash-set %d (%s)",  workerDataExchange.worker() , workerDataExchange.hg_set() , workerDataExchange.finish()?"finished":"not finished" ));
        
        network::Queue queue = workerDataExchange.queue();
        bool txt = workerDataExchange.txt();
		std::string queue_name = queue.name();
        
        if( txt )
        {
            // Add directly the append operation for each hash-group set
            int hg_set = workerDataExchange.hg_set();
            addFile( newFileNameForTXTOutput( hg_set ) , queue_name , KVInfo( buffer->getSize() , 0 ) , buffer );
        }
        else
        {
        
            QueueuBufferVector* bv = queueBufferVectors.findInMap( queue_name );
            
            if( !bv )
            {
                bv = new QueueuBufferVector(  queue , txt );
                queueBufferVectors.insertInMap( queue_name , bv  );
            }
            
            // Add the buffer to the queue vector
            bv->addBuffer( workerDataExchange, buffer );
            
            // Check if there is enougth data to produce a new file
            QueueuBufferVector* new_bv = bv->getQueueuBufferVectorToWrite();
            
            // Schedule this new buffer vector to be writted to disk
            if( new_bv )
            {
                DataBufferProcessItem* tmp = new DataBufferProcessItem( new_bv );
                addProcessItem(tmp);
            }
        }
	}
    
    void WorkerTask::addKVFile( std::string fileName , std::string queue , engine::Buffer *buffer )
    {
        KVHeader * header = (KVHeader*) ( buffer->getData() );
        addFile(fileName, queue, header->info , buffer );
    }
    
    
    void WorkerTask::addFile( std::string fileName , std::string queue , KVInfo info )
    {
        WorkerTaskOutputFile * outputFile = outputFiles.findInMap( fileName ); 
        
        if ( !outputFile )
        {
            outputFile = new WorkerTaskOutputFile( fileName , queue , taskManager->worker->network->getWorkerId() );
            outputFiles.insertInMap( fileName, outputFile );
        }

        // Append generated content for this file
        outputFile->info.append( info );
        
    }
    
	// add a buffer to be saved as a key-value file
	void WorkerTask::addFile(  std::string fileName , std::string queue , KVInfo info , engine::Buffer *buffer )
	{
        
        // Add the file to be reported to the controller
        addFile( fileName , queue , info );

		// Submit the operation
        engine::DiskOperation *operation = engine::DiskOperation::newAppendOperation(  buffer ,  SamsonSetup::shared()->dataFile( fileName ) , getEngineId() );
        addDiskOperation(operation);
	}
	
	void WorkerTask::removeFile(  std::string fileName , std::string queue )
	{
        WorkerTaskOutputFile * outputFile = outputRemoveFiles.findInMap( fileName ); 
        
        if ( !outputFile )
        {
            outputFile = new WorkerTaskOutputFile( fileName , queue , taskManager->worker->network->getWorkerId() );
            outputRemoveFiles.insertInMap( fileName, outputFile );
        }
	}
	
	void WorkerTask::flush()
	{
		au::map<std::string , QueueuBufferVector>::iterator iter;
		
		for ( iter = queueBufferVectors.begin() ; iter != queueBufferVectors.end() ; iter++ )
        {
            DataBufferProcessItem* tmp = new DataBufferProcessItem( iter->second );
            addProcessItem(tmp);
        }
        
        // Not remove, since they have been transfered to DataBufferProcessItem
		queueBufferVectors.clear();
	}	
	
	
#pragma mark Messages

	void WorkerTask::sendCloseMessages( )
	{
		NetworkInterface *network = taskManager->worker->network;

		int num_workers = network->getNumWorkers();
		
		for (int s = 0 ; s < num_workers ; s++)
		{				
			Packet *p = new Packet( Message::WorkerDataExchangeClose );
			network::WorkerDataExchangeClose *dataMessage =  p->message->mutable_data_close();
			dataMessage->set_task_id(task_id);
			network->send( network->workerGetIdentifier(s) , p);
		}
	}	

	void WorkerTask::sendUpdateMessageToController( FullKVInfo running_info , FullKVInfo processed_info )
	{
		
		Packet *p = new Packet( Message::WorkerTaskConfirmation );

		network::WorkerTaskConfirmation *confirmation_message = p->message->mutable_worker_task_confirmation();
        
        confirmation_message->set_task_id( task_id );
        
        confirmation_message->set_type( network::WorkerTaskConfirmation::update );
        
        network::KVInfo * process = confirmation_message->mutable_progressprocessed();
        process->set_size( processed_info.size );
        process->set_kvs( processed_info.kvs );

        network::KVInfo * running = confirmation_message->mutable_progressrunning();
        running->set_size( running_info.size );
        running->set_kvs( running_info.kvs );
        
		taskManager->worker->network->sendToController( p );	
    }	
	
    
	void WorkerTask::sendFinishTaskMessageToController( )
	{		
		
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet( Message::WorkerTaskConfirmation );
		network::WorkerTaskConfirmation *finish_message = p->message->mutable_worker_task_confirmation();
        
        finish_message->set_task_id( task_id );

        // Add a file for each generated file
        for ( au::map<std::string , WorkerTaskOutputFile>::iterator f = outputFiles.begin( ) ; f!=outputFiles.end() ; f++)
            f->second->fill( finish_message->add_add_file( ) );
        
		
		if( error.isActivated() )
		{
			finish_message->set_type( network::WorkerTaskConfirmation::error );
			finish_message->set_error_message( error.getMessage() );
		}
		else
			finish_message->set_type( network::WorkerTaskConfirmation::finish );
        
		taskManager->worker->network->send(  network->controllerGetIdentifier(), p);
		
	}	
	
	void WorkerTask::sendCompleteTaskMessageToController( )
	{	
		NetworkInterface *network = taskManager->worker->network;
		
		Packet *p = new Packet(Message::WorkerTaskConfirmation);
		network::WorkerTaskConfirmation *confirmation = p->message->mutable_worker_task_confirmation();

		// Copy all the information from the prepared message
		confirmation->CopyFrom( *complete_message );
		
		if( error.isActivated() )
		{
            //LM_M(("Sending a WorkerTaskConfirmationError message for task %lu", task_id ));
			confirmation->set_type( network::WorkerTaskConfirmation::error );
			confirmation->set_error_message( error.getMessage() );
		}
		else
		{
            //LM_M(("Sending a WorkerTaskConfirmation message for task %lu", task_id ));
			confirmation->set_type( network::WorkerTaskConfirmation::complete );
		}
		
		taskManager->worker->network->send( network->controllerGetIdentifier(), p);
	}
	
		
#pragma mark FileName
	
	// Get a new file Name for a file

	std::string WorkerTask::newFileName( )
	{
		// Get the worker id
		int worker_id = taskManager->worker->network->getWorkerId();
        
		// Create the nme of the file
        char fileName[2000];

#ifdef __LP64__
        sprintf(fileName, "worker_%d_job_%lu_task_%lu_file_%010d", worker_id, job_id, task_id, num_file_output++);
#else
        sprintf(fileName, "worker_%d_job_%d_task_%d_file_%010d", worker_id, job_id, task_id, num_file_output++);
#endif

		return fileName;
        
	}
    
    std::string WorkerTask::newFileNameForTXTOutput( int hg_set )
    {
		// Get the worker id
		int worker_id = taskManager->worker->network->getWorkerId();

		// Create the nme of the file
        char fileName[256];

#ifdef __LP64__
        snprintf(fileName, sizeof(fileName), "worker_%d_job_%lu_task_%lu_file_txt_%010d" , worker_id , job_id, task_id , hg_set );
#else
		snprintf(fileName, sizeof(fileName), "worker_%d_job_%d_task_%d_file_txt_%010d" , worker_id , job_id, task_id , hg_set );
#endif
		return fileName;
    }
    
	
	
#pragma mark KILL	
	
	void WorkerTask::kill( std::string message )
	{
        //LM_W(("Kill task %lu: (%s)" , task_id , message.c_str() ));

        // Cancel my own tasks
        cancelEngineOperations();
        
        // Remove all subtasks
        au::map<size_t,WorkerSubTask>::iterator s;
        for ( s = subTasks.begin() ; s != subTasks.end() ; s++ )
        {
            WorkerSubTask *subTask = s->second;
            subTask->cancelEngineOperations();
        }
 		
	}
    
    void WorkerTask::setError(std::string _error_message)
    {
        //LM_W(("Seting error for task %lu: %s" , task_id , _error_message.c_str() ));
        
        // Set the error of the operation
        error.set( _error_message );

        // Set the flag of completed to cancel this task automatically
        status = completed;
        
        // Send the confirmation to the controller
        sendCompleteTaskMessageToController();

        // Check everything
        check();
    }
    
    bool WorkerTask::canBeRemoved()
    {
        if( status != completed )
            return false;
        
        if( hasPendingEngineOperations() )
            return false;
        
        // Remove all subtasks
        au::map<size_t,WorkerSubTask>::iterator s;
        for ( s = subTasks.begin() ; s != subTasks.end() ; s++ )
        {
            WorkerSubTask *subTask = s->second;
            if (subTask->hasPendingEngineOperations() )
                return false;
        }
     
        return true;
    }
    
#pragma Notifications
    
    void WorkerTask::addDiskOperation( engine::DiskOperation *operation )
    {
        num_disk_operations++;
        engine::DiskManager::shared()->add( operation );
    }

    void WorkerTask::addProcessItem( engine::ProcessItem *item )
    {
        num_process_items++;
        engine::ProcessManager::shared()->add( item , getEngineId() );
    }
    
    
#pragma mark Basic notifications
    
    void WorkerTask::setNotificationCommonEnvironment( engine::Notification *notification )
    {
        notification->environment.set("target" , "WorkerTask" );
        notification->environment.setSizeT("task_id", task_id );
        notification->environment.setInt("worker",  taskManager->worker->network->getWorkerId() );
        
    }    

    

	
}