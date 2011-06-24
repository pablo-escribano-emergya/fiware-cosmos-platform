

#include "logMsg/logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "engine/DiskManager.h"                    // Own interface
#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/EngineNotificationElement.h"      // engine::EngineNotificationElement
#include "au/Descriptors.h"                         // au::Descriptors


namespace engine
{
    
    
#pragma ---
    
    static DiskManager *diskManager = NULL;
    
    void destroy_diskManager()
    {
        LM_M(("DiskManager terminating"));

        if( diskManager )
        {
            delete diskManager;
            diskManager = NULL;
        }
    }
    
    void DiskManager::init( int _num_disk_operations )
    {
        // Now it is only possible to run with 1
        _num_disk_operations = 1;
        
        if( diskManager )
            LM_X(1,("Please, init diskManager only once"));
        
        atexit(destroy_diskManager);
        
        diskManager = new DiskManager (_num_disk_operations );
    }
    
    
    DiskManager* DiskManager::shared()
    {
        if (!diskManager)
            LM_X(1, ("Please init DiskManager before using it"));
        return diskManager;
    }
    
#pragma mark DiskManager
    
    DiskManager::DiskManager( int _num_disk_operations )
    {
		pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // Number of parallel disk operations
        num_disk_operations = _num_disk_operations;
    }
    
    DiskManager::~DiskManager()
    {
		pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
	void DiskManager::add( DiskOperation *operation )
	{
        // Set the pointer to myself
        operation->diskManager = this;
        
		pthread_mutex_lock(&mutex);
        
		// Insert the operation in the queue of pending operations
        pending_operations.insert( _find_pos(operation), operation );
        
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}

	void DiskManager::cancel( DiskOperation *operation )
	{
        
		pthread_mutex_lock(&mutex);

        if( pending_operations.extractFromList( operation ) )
        {
            // Add a notification for this operation ( removed when delegate is notified )
            Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listenerId );
            notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
            Engine::add(notification);            
        }
        
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}
	
    
	void DiskManager::finishDiskOperation( DiskOperation *operation )
	{
        // Callback received from background process
        
		pthread_mutex_lock(&mutex);
        
		running_operations.erase( operation );
		diskStatistics.add( operation->getType() , operation->getSize() );
		
		pthread_mutex_unlock(&mutex);
		
		// Add a notification for this operation to the required target listener
        Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listenerId );
        Engine::add(notification);    
		
		// Check if there are more operation to be executed
		checkDiskOperations();
	}
	
	// Check if we can run more disk operations
	void DiskManager::checkDiskOperations()
	{
		pthread_mutex_lock(&mutex);
		
		while( ( pending_operations.size() > 0 ) && ( running_operations.size() < (size_t)num_disk_operations ) )
		{
			// Extract the next operation
			DiskOperation *operation = pending_operations.extractFront();
			
			running_operations.insert( operation );
			
			// Run in background
			operation->runInBackGround();
		}
		
		pthread_mutex_unlock(&mutex);
		
	}

    int DiskManager::getNumOperations()
    {
		pthread_mutex_lock(&mutex);
        size_t num_operations = pending_operations.size() + diskManager->running_operations.size();
		pthread_mutex_unlock(&mutex);
        
        return num_operations;
        
    }

    
    size_t DiskManager::getReadRate()
    {
		pthread_mutex_lock(&mutex);
        size_t rate = diskStatistics.item_read.getLastMinuteRate();
		pthread_mutex_unlock(&mutex);
        
        return rate;
    }
    
    size_t DiskManager::getWriteRate()
    {
		pthread_mutex_lock(&mutex);
        size_t rate = diskStatistics.item_write.getLastMinuteRate();
		pthread_mutex_unlock(&mutex);
        
        return rate;
    }
    
    std::string DiskManager::str()
	{
		
		pthread_mutex_lock(&mutex);
		
        
		// Disk Manager
		// ----------------------------------------------------------------------------
		std::ostringstream disk_manager_status;
        
		disk_manager_status << "\n\tRunning: ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getDescription() << "] ";
		
		disk_manager_status << "\n\tQueued: ";
        
		for ( std::list<DiskOperation*>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getShortDescription() << "] ";
        
        {
            au::Descriptors descriptors;
            
            for ( au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
            {
                switch ( (*i)->getType() ) {
                    case DiskOperation::read:
                        descriptors.add("reads");
                        break;
                    case DiskOperation::write:
                        descriptors.add("writes");
                        break;
                    case DiskOperation::append:
                        descriptors.add("appends");
                        break;
                    case DiskOperation::remove:
                        descriptors.add("removes");
                        break;
                }
            }
            
            disk_manager_status << descriptors.str();
            
        }
        
		disk_manager_status << "\n\tStatistics: ";
		disk_manager_status << diskStatistics.getStatus();
		
		pthread_mutex_unlock(&mutex);
        
        return disk_manager_status.str();
		
	}	

    
    au::list<DiskOperation>::iterator DiskManager::_find_pos( DiskOperation *diskOperation )
    {
        for (au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
        {
            if( diskOperation->compare(*i) )
                return i;
        }
        return pending_operations.end();
    }
    
    
    void DiskManager::quit()
    {
		pending_operations.clearList();		// Remove pending operations ( will never be notified )
		
        while( running_operations.size() > 0)
        {
            LM_M(("Waiting %d running disk operations to finish " , running_operations.size() ));
            sleep(1);
        }
        
    }
    
}
