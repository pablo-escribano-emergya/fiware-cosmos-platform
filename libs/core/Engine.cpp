

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "Engine.h"							// Own interface
#include "EngineElement.h"					// ss::EngineElement
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessItem.h"					// ss::ProcessItem
#include "DiskOperation.h"					// ss::DiskOperation
#include "EngineNotificationElement.h"      // ss::EngineNotificationElement

namespace ss
{

    
#pragma mark ENGINE
	
	Engine *e=NULL;		// Static singlelton for thir worker
	
	void*runEngineBakground(void*e)
	{
		((Engine*)e)->run();
		return NULL;
	}
	
    void destroy_engine()
    {
        if( e )
        {
            e->quit();   // Quit if still not quited
            delete e;
        }
        e = NULL;
    }
	
	Engine::Engine()
	{
		_quit = false;									// By default, this flag is false
		_running =  false;								// Not running until "run" method is called
		
		pthread_mutex_init(&elements_mutex, 0);			// Mutex to protect elements
		pthread_cond_init(&elements_cond, 0) ;			// Conditional to block the thread while waiting the next event
        
        // Add the MemoryManager as a listner for the memory_request channel
        notificationSystem.add( notification_memory_request, &memoryManager );
        
        // Add the processManager as a listener for process request
        notificationSystem.add( notification_process_request , &processManager );

        // Add the diskManager as a listener for disk operations
        notificationSystem.add( notification_disk_operation_request , &diskManager );
        
	}
	
	Engine::~Engine()
	{
        
        LM_M(("Destroying Samson Engine"));
        
		pthread_mutex_destroy(&elements_mutex);			// Mutex to protect elements
		pthread_cond_destroy(&elements_cond) ;			// Conditional to block the thread while waiting the next event
        
        // clear the pending elements to be executed
        elements.clearList();
        
	}
	
	void Engine::init()
	{
		if( e )
			LM_X(1,("Error initializing twice the Samson Engine"));
		
		LM_T( LmtEngine , ("Engine init"));

		e = new Engine();
	
        
        // At exit destroy
        atexit( destroy_engine  );

	}

	Engine *Engine::shared()
	{
		if( !e )
			LM_X(1,("SamsonEngine not initialized"));
		return e;
	}

	
	void Engine::runInBackground()
	{
		pthread_create(&t, 0, runEngineBakground, e);	
	}

	void Engine::run()
	{
		// Keep the thread for not calling quit from the same thread
		t = pthread_self();	
		
		// Add the Nothing Engine Element to loop continuously
		add( new ss::NothingEngineElement() );
		
		LM_T( LmtEngine , ("Engine run"));
		_running =  true;
		
		while( true )
		{
			
			pthread_mutex_lock(&elements_mutex);

			time_t now = time(NULL);
			//LM_M(("Loop of the engine time: %u elements: %d", now , elements.size() ));
			
			if( elements.size() == 0)
			{
				// No more things to do
				LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quiting ..."));
				pthread_mutex_unlock(&elements_mutex);

				_running = false;	// Flag to indicate that the engine is not running any more
				
				return;
			}

			if( now >= elements.front()->getThriggerTime() )
			{
				//LM_M(("Something to execute...."));
				
				EngineElement *element = elements.front();
				elements.pop_front();

				// Unlock the thread
				pthread_mutex_unlock(&elements_mutex);
				
				// Execute the item

				LM_T( LmtEngine, ("[START] Engine executing %s" , element->getDescription().c_str()));

				element->run();

				LM_T( LmtEngine, ("[DONE] Engine executing %s" , element->getDescription().c_str()));
				
				
				if( element->isRepeated() )
				{
					// Insert again
					element->Reschedule();
					add( element );
				}
				else
					delete element;
				
			}
			else
			{
				int seconds = elements.front()->getThriggerTime() - now;
				
				struct timeval tv;
				struct timespec ts;
				gettimeofday(&tv, NULL);
				ts.tv_sec = tv.tv_sec + seconds;
				ts.tv_nsec = 0;
				
				//LM_M(("Sleeping %d seconds",  seconds ));
				
				pthread_cond_timedwait(&elements_cond, &elements_mutex , &ts );
				pthread_mutex_unlock(&elements_mutex);
				
				//LM_M(("Waking up after sleeping %d seconds",  seconds ));
				
			}
		}
		
	}
	
	void Engine::quit()
	{
        
        LM_M(("Quiting samson engine...."));
        
       if( ! _running )
            return; // Not necessary to quit anything.

		// Flag to avoid more "adds"
		_quit = true;	
        
        LM_M(("Quiting process manager...."));
        
        // Quit the process manager means remove all pending processes and wait for the current ones.
        processManager.quit();

        LM_M(("Quiting disk manager...."));
        
        // Remove pending disk operations and wait for the running ones
        diskManager.quit();
        
		pthread_mutex_lock(&elements_mutex);

		// Remove All elements
		elements.clearList();		
		
		// Wake up the main thread to process this element if necessary	
		pthread_cond_signal(&elements_cond);
		
		pthread_mutex_unlock(&elements_mutex);
			
		// If we are calling quit from another thread, we will wait until the main thead is finished
		if( pthread_self() != t )
		{
			
			LM_M(("Waiting samson engine to finish...."));
			
			while( _running )
				sleep(1);
			
			LM_M(("Samson engine completelly finished"));
		}
		
	}
			
	void Engine::add( EngineElement *element )
	{
		pthread_mutex_lock(&elements_mutex);
		
		if( _quit )
		{
			// Not adding this element
			delete element;
		}
		else
		{
			elements.insert( _find_pos( element ) ,  element );

			// Wake up the main thread to process this element if necessary	
			pthread_cond_signal(&elements_cond);
		}
		
		pthread_mutex_unlock(&elements_mutex);
		
	}
	
	
	std::list<EngineElement*>::iterator Engine::_find_pos( EngineElement *e)
	{
		for (std::list<EngineElement*>::iterator i = elements.begin() ; i != elements.end() ; i++)
		{
			if( (*i)->getThriggerTime() > e->getThriggerTime() )
				return i;
		}
		
		return elements.end();
	}
	
#pragma mark -----
    
    void Engine::notify( EngineNotification*  notification )
    {
        // Add an element to notify latter
        add( new EngineNotificationElement( notification ) );
    }
	
#pragma mark ----
	
	void Engine::fill(network::WorkerStatus*  ws)
	{
		// Fill the memory manager stuff
		memoryManager.fill( ws );
        diskManager.fill( ws );
        processManager.fill( ws );
		
	}	
	
}