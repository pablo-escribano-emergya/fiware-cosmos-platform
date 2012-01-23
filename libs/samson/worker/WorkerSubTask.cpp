
#include "au/file.h"


#include "engine/DiskOperation.h"					// samson::DiskOperation
#include "engine/DiskManager.h"            // Notifications
#include "engine/ProcessManager.h"         // Notifications
#include "engine/MemoryManager.h"          // Notifications
#include "engine/Notification.h"            // ENGINE::Notification


#include "WorkerSubTask.h"			// Own interface
#include "WorkerTaskManager.h"		// samson::WorkerTaskManager
#include "WorkerTask.h"				// samson::WorkerTask
#include "engine/ProcessItem.h"			// samson::ProcessItem
#include "ProcessOperation.h"       // samson::ProcessOperation
#include "ProcessCompact.h"         // samson::ProcessCompact
#include "ProcessGenerator.h"		// samson::ProcessGenerator
#include "WorkerTask.h"						// samson::WorkerTask
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "ProcessOperation.h"				// samson::ProcessOperation
#include "ProcessParser.h"					// samson::ProcessParser

#include "samson/worker/SamsonWorker.h"                   // samson::SamsonWorker

namespace samson
{
	
	WorkerSubTask::WorkerSubTask( WorkerTask *_task )
	{
		task = _task;
        
        num_read_operations = 0;
		num_read_operations_confirmed = 0;
        
        num_processes = 0;
        num_processes_confirmed = 0;
        
		description = "Unknown";                  // Unknown   
        status = init;                      // Initial status
        buffer = NULL;                      // By default we have no memory
        
	}
    
    void WorkerSubTask::releaseResources()
    {
        if( buffer )
        {
            engine::MemoryManager::shared()->destroyBuffer(buffer);
            buffer = NULL;
        }
     }
        
    
    WorkerSubTask::~WorkerSubTask()
    {
        releaseResources();
        
    };		
    
    void WorkerSubTask::notify( engine::Notification* notification )
    {
        if( notification->isName(notification_memory_request_response) )
        {
            // New buffer to be used to send data to the workers
            engine::MemoryRequest *memoryRequest = (engine::MemoryRequest *) notification->extractObject();
            
            // Remove at the engine::OperationsContainer
            erase( memoryRequest );
            
            if( !memoryRequest )
                LM_X(1, ("Internal error: Memory request returnes without an object"));
            
            if ( !memoryRequest->buffer )
            {
                error.set( "Memory request returned without the allocated buffer. This can be caused by a 'kill' command.");
            }
            else
            {
                // collect the buffer
                buffer = memoryRequest->buffer;
            }
            
            delete memoryRequest;
            
            run();
        }
        else if ( notification->isName( notification_disk_operation_request_response ) )
        {
            
            if( !notification->containsObject() )
                LM_W(("Received a notification_disk_operation_request_response without DiskOperation object"));
            else
            {
                
                engine::DiskOperation* operation = (engine::DiskOperation*)notification->extractObject();
                
                // Remove at the engine::OperationsContainer
                erase( operation );
                
                // Copy the error ( if any )
                error.set( &operation->error );
                delete operation;
                
                num_read_operations_confirmed++;
                
                run();
                
            }
        }
        else if ( notification->isName(notification_process_request_response) )
        {
            
            if( !notification->containsObject() )
                LM_W(("Received a notification_process_request_response without ProcessItem object"));
            else
            {
                
                engine::ProcessItem* item = (engine::ProcessItem*)notification->extractObject();
                
                // Remove at the engine::OperationsContainer
                erase( item );
                
                // Copy the error ( if any )
                error.set( &item->error );
                
                delete item;
                
                num_processes_confirmed++;
                run();
                
            }
            
        }
        else
            LM_X(1,("WorkerSubTask received an unexpected notification"));
    }
    
    void WorkerSubTask::run()
    {        

        // Spetial case for any kind of error
        if ( (status != finished) && error.isActivated() )
        {
            status = finished;
            
            // Notification that this sub-task is finished
            engine::Notification * notification  = new engine::Notification( notification_sub_task_finished ,NULL, task->getEngineId() );
            setNotificationCommandEnvironment(notification);
            notification->environment.set("target", "WorkerTask");      // Modify to "send" this notification to the worker task
            engine::Engine::shared()->notify( notification );
            return;
        }
        
        switch (status) {
            case init:
            {
                // Memory request if necessary
                size_t memory = getRequiredMemory();
                status = waiting_memory;
                if( !buffer && (memory > 0) )
                {
                    addMemoryRequest( memory );
                    return;
                }
                else
                    run(); // Recursive call for next step if no buffer is required for this sub task
                break;
            }
                
            case waiting_memory:
            {
                status = waiting_reads;
                run_read_operations();

                if( num_read_operations == 0 )
                    run(); // Recursive calls if no reads are necessary for this task
                
                break;
            }
         
            case waiting_reads:
            {
                if( num_read_operations_confirmed == num_read_operations )
                {

                    status = waiting_process;
                    
                    // Send a progress report to controller
                    task->sendUpdateMessageToController( info , FullKVInfo(0,0) );
                    
                    run_process();
                    
                    if( num_processes == 0 )
                        run();  // Recursive call if no process is required
                }
                break;
            }
                
            case waiting_process:
            {
                if( num_processes_confirmed == num_processes )
                {

                    status = finished;
                    
                    // Send a progress report to controller
                    task->sendUpdateMessageToController( FullKVInfo(0,0) , info );
                    
                    // Notification that this sub-task is finished
                    engine::Notification * notification  = new engine::Notification( notification_sub_task_finished ,NULL, task->getEngineId() );
                    setNotificationCommandEnvironment(notification);
                    notification->environment.set("target", "WorkerTask");      // Modify to "send" this notification to the worker task
                    engine::Engine::shared()->notify( notification );
                }
                break;
            }
                
            case finished:
            {
                // Nothing to do
            }
                
        }
        
        
        

    }

    
    void WorkerSubTask::setNotificationCommandEnvironment( engine::Notification *notification)
    {
        notification->environment.set( "target" , "WorkerSubTask" );
        notification->environment.setInt("worker",  task->taskManager->worker->network->getWorkerId() );
        notification->environment.setSizeT( "task_id" , task_id ); 
        notification->environment.setSizeT( "sub_task_id" , sub_task_id ); 
    }
    
    
    void WorkerSubTask::addMemoryRequest( size_t size )
    {
        
        engine::MemoryRequest *memoryRequest = new engine::MemoryRequest( size , 0.5 , getEngineId() );
        
        // add to the engine::OperationsContainer
        add( memoryRequest );
        
        engine::MemoryManager::shared()->add( memoryRequest  );
    }
    
    
    void WorkerSubTask::addReadOperation( engine::DiskOperation *operation )
    {
        // Add at the engine::OperationsContainer
        add( operation );
        
        // Increase the counter of operations to read
        num_read_operations++;
        
        // add the operation to the DiskManager ( notification will come back )
        engine::DiskManager::shared()->add( operation );
    }
    
    void WorkerSubTask::addProcess(  engine::ProcessItem* processItem )
    {
        // Add at the engine::OperationsContainer
        add( processItem );
        
        // Increase the counter of operations to read
        num_processes++;
        
        // Set myself as the target listener
        engine::ProcessManager::shared()->add( processItem , getEngineId() ) ;
        
    }
    
    
    void WorkerSubTask::getInfo( std::ostringstream& output)
    {
        au::xml_open( output , "worker_subtask" );
        
        au::xml_simple( output , "description" , description );
        
        if( error.isActivated() )
            au::xml_simple( output , "error" , error.getMessage() );
        
        
        switch (status) {
                
            case init: 
                au::xml_simple( output , "state" , "init" );
                break;
            case waiting_memory: 
                au::xml_simple( output , "state" , "waiting_memory" );
                break;
            case waiting_reads:
                au::xml_simple( output , "state" , "waiting_reads" );
                break;
            case waiting_process:
                au::xml_simple( output , "state" , "waiting_process" );
                break;
            case finished: 
                au::xml_simple( output , "state" , "finished" );
                break;
        }
        
        au::xml_close( output , "worker_subtask" );
    }
    
    
    
#pragma mark GeneratorSubTask
        
	GeneratorSubTask::GeneratorSubTask(WorkerTask * task , int _num_operation , int _num_operations ) : WorkerSubTask( task  )
	{
		description = "Generator ";
        
        num_operation = _num_operation;
        num_operations = _num_operations;
        
	}
	
	void GeneratorSubTask::run_process()
	{
		addProcess( new ProcessGenerator( task , num_operation , num_operations ) );
	}
	
	
#pragma mark OrganizerSubTask
	
	OrganizerSubTask::OrganizerSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );
		
		description = "Operation Organizer"; // Organizer
		
	}
	
	// Function to get all the read operations necessary for this task
    void OrganizerSubTask::run_read_operations()
	{
		
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			engine::DiskOperation *item = getFileMangerReadItem( task->reduceInformation->file[f] );
            addReadOperation( item );
		}
	}
	
	// Function to get the ProcessManagerItem to run

	void OrganizerSubTask::run_process()
	{
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the reduce in multiple WorkerTaskItems to process each set of hash-groups
		// Division is done in such a way that any of the parts do not get overloaded
		
		// Number of paralel processes in this system
		int num_process = SamsonSetup::shared()->getInt("general.num_processess");
		
		// Maximum size per hash-group
		size_t max_size_per_group = reduceInformation->total_size / ( 4 * num_process );
		
		// Another limit for memory reasons
		size_t max_size_per_group2 = ( SamsonSetup::shared()->getUInt64("general.memory") /4) - reduceInformation->total_num_input_files*KVFILE_NUM_HASHGROUPS*sizeof(KVInfo) - sizeof(KVHeader);
		if( max_size_per_group2 < max_size_per_group)
			max_size_per_group = max_size_per_group2;
		
		size_t limit_size_per_group = engine::MemoryManager::shared()->getMemory()/2;
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->info_hg[0].size;	// Total size for this operation

		int hg_set = 0 ;                                        // Counter of the hash-group
        
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->info_hg[hg].size;
			LM_T(LmtDisk,("Hashgroup:%d, current_hg_size:%lu", hg, current_hg_size));
			
			if( current_hg_size > limit_size_per_group )
			{
				LM_W(("Max size for a hash-group(%d) exceeded. (current_hg_size(%lu) > limit_size_per_group(%lu = sharedMemory(%lu)/2)", hg, current_hg_size, limit_size_per_group, engine::MemoryManager::shared()->getMemory()));
				task->setError("Max size for a hash-group exceeded. Operation not supported");
				return;
			}
			
			if( ( ( total_size + current_hg_size  ) > max_size_per_group ) )
			{
				
				if( total_size > 0 )
                {
                    OperationSubTask * tmp = new OperationSubTask( task , item_hg_begin , hg  );
                    tmp->hg_set = hg_set++;
					task->addSubTask( tmp  );
                }
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
        OperationSubTask * tmp = new OperationSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS );
        tmp->hg_set = hg_set++;
		task->addSubTask( tmp );
		
	}
	
    engine::DiskOperation * OrganizerSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		engine::DiskOperation *item = engine::DiskOperation::newReadOperation(  SamsonSetup::shared()->dataFile( file->fileName ) , offset , size , file->getSimpleBufferForInfo() , getEngineId() );
		return item;
	}	
	
	
#pragma mark OperationSubTask
	
	OperationSubTask::OperationSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files*( ( num_hash_groups )*sizeof(KVInfo) + sizeof(KVHeader) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
        {
			memory_requested += task->reduceInformation->info_hg[hg].size;
            info.append( task->reduceInformation->info_hg[hg] );    // Accumulated total input data
        }
		
		description = "Operation";// Operation
	}
	
	OperationSubTask::~OperationSubTask()
	{
	}
	
	size_t OperationSubTask::getRequiredMemory()
	{
		return memory_requested;
	}
	
	void OperationSubTask::run_read_operations()
	{
        // If no memory, means that no process is required
		if( memory_requested == 0)
            return;
        
		// Pointer to the buffer
		char* data = buffer->getData();
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			memcpy(data+offset, &header, sizeof(KVHeader));
			offset+= sizeof(KVHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			engine::DiskOperation *item 
			= engine::DiskOperation::newReadOperation( \
									   SamsonSetup::shared()->dataFile( reduceInformation->file[f]->fileName ) , \
									  reduceInformation->file[f]->getFileOffset( hg_begin ), \
									  header.info.size, \
									  buffer->getSimpleBufferAtOffset(offset),\
                                       getEngineId()               
                                                      );
			
			addReadOperation( item );
			
			offset += header.info.size;
		}
		
		
	}
	
	// Function to get the ProcessManagerItem to run
	void OperationSubTask::run_process()
	{
        // If no memory, means that no process is required
		if( memory_requested == 0)
            return;

		ProcessOperation * item = new ProcessOperation( this );
        item->hg_set = hg_set;
        addProcess(item);
	}
	
	
#pragma mark  ParserSubTask
	
	
	ParserSubTask::ParserSubTask( WorkerTask * task ,  std::string _fileName  ) : WorkerSubTask( task  )
	{
		// Filename to parse
		fileName = _fileName;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		fileSize = au::sizeOfFile( SamsonSetup::shared()->dataFile( fileName ) );	
		
        info.append( fileSize , 1 );    // Accumulated total input data
        
        
		description = "Parser"; // Parser
	}
	
	ParserSubTask::~ParserSubTask()
	{
	}
	
	size_t ParserSubTask::getRequiredMemory()
	{
		return fileSize;
	}
	
	
	void ParserSubTask::run_read_operations()
	{
        
        if( fileSize > 0)
        {
            if( !buffer )
                LM_X(1,("Intern error: No buffer in read operations of task"));
            // Single file to be parsed
            engine::DiskOperation *item = engine::DiskOperation::newReadOperation(  SamsonSetup::shared()->dataFile(fileName) , 0, fileSize, buffer->getSimpleBuffer() , getEngineId() );
            addReadOperation(item);
        }
		
	}
	
	// Function to get the ProcessManagerItem to run
	void ParserSubTask::run_process()
	{
		addProcess(new ProcessParser( this ) );
	}
	
#pragma mark SystemSubTask
	
	SystemSubTask::SystemSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );
		description = "System"; // System
		
		// add all the File elements to be removed when the operation is complete
		for (int i = 0 ; i < task->workerTask.input_queue(0).file_size() ; i++)
		{
            /*
			network::QueueFile qf;
			qf.set_queue( task->workerTask.input_queue(0).queue().name() );
			qf.mutable_file()->CopyFrom( task->workerTask.input_queue(0).file(i) );
			task->removeFile( qf );
             */
            
            LM_TODO(("Reimplement using notifications!!"));
		}
	}
	
	// Function to get all the read operations necessary for this task
	void SystemSubTask::run_read_operations()
	{
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			engine::DiskOperation *item = getFileMangerReadItem( task->reduceInformation->file[f] );
            addReadOperation(item);
		}
	}
	
	// Function to get the ProcessManagerItem to run
	
	void SystemSubTask::run_process()
	{
		
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the content so, that we generate necessary files
		size_t max_item_content_size = SamsonSetup::shared()->getUInt64("general.max_file_size");
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->info_hg[0].size;	// Total size for this operation
		
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->info_hg[hg].size;
			
			if( ( ( total_size + current_hg_size  ) > max_item_content_size ) )
			{
				
				if( total_size > 0 )
					task->addSubTask( new CompactSubTask( task , item_hg_begin , hg  ) );
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
		task->addSubTask( new CompactSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS ) );
		
		// No real process for this sub-task
	}
	
	engine::DiskOperation * SystemSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		engine::DiskOperation *item = engine::DiskOperation::newReadOperation(  SamsonSetup::shared()->dataFile(file->fileName) , offset , size , file->getSimpleBufferForInfo() , getEngineId() );
		//item->tag = task->task_id;
		return item;
	}		
	
#pragma mark -
	
	
	CompactSubTask::CompactSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files * ( sizeof(KVHeader) + ( num_hash_groups )*sizeof(KVInfo) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
			memory_requested += task->reduceInformation->info_hg[hg].size;
		
		description = "System";// System
	}
	
	CompactSubTask::~CompactSubTask()
	{
	}
	
	
	
	size_t CompactSubTask::getRequiredMemory()
	{
		return memory_requested;
	}
	
	
	void CompactSubTask::run_read_operations()
	{
		
		// Pointer to the buffer
		char* data = buffer->getData();
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			memcpy(data+offset, &header, sizeof(KVHeader));
			offset+= sizeof(KVHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			engine::DiskOperation *item 
			= engine::DiskOperation::newReadOperation( \
									   SamsonSetup::shared()->dataFile( reduceInformation->file[f]->fileName ) , \
									  reduceInformation->file[f]->getFileOffset( hg_begin ), \
									  header.info.size, \
									  buffer->getSimpleBufferAtOffset(offset) , \
                                      getEngineId()
                                                      );
			
            // Add this item to be read
            addReadOperation(item);
            
			offset += header.info.size;
		}
		
	}
	
	// Function to get the ProcessManagerItem to run
	void CompactSubTask::run_process()
	{
		addProcess( new ProcessCompact( this ) );
	}
	
	
}