#ifndef _H_WORKER_TASK_MANAGER
#define _H_WORKER_TASK_MANAGER

#include <list>							// std::list
#include "au/map.h"						// au::map
#include "samson/common/samson.pb.h"					// WorkerTask
#include "au/Token.h"						// au::Token
#include "samson/common/samson.pb.h"					// samson::network::...
#include "engine/ProcessItem.h"				// samson::ProcessManagerDelegate
#include "engine/Buffer.h"						// samson::Buffer
#include "engine/MemoryRequest.h"				// samson::MemoryRequest & MemoryRequestDelegate
#include "engine/Object.h"         // samson::EngineNotification

namespace samson {


	class SamsonWorker;
	class DataBufferItem;
	class WorkerTask;
	class NetworkInterface;
	    
	class WorkerTaskManager : public engine::Object 		// Receive notifications from the process manager
	{
		
	public:
		
		SamsonWorker *worker;

	private:

		au::map<size_t,WorkerTask> task;					// List of tasks
				
	public:
		
		WorkerTaskManager(SamsonWorker *_worker);
        ~WorkerTaskManager();
        
		// Add a task from a message comming from the controller
		void addTask( const network::WorkerTask &task );

		// Kill a task
		void killTask( const network::WorkerTaskKill &task_kill );
		
        
		// Add a buffer from other workers ( associated to a particular task and output queue)
		void addBuffer( size_t task_id , network::WorkerDataExchange& workerDataExchange , engine::Buffer* buffer );
				
		// Notification that a worker has finish produccing data for a task
		void finishWorker( int worker_from , size_t task_id );

		
		// Check if a particular task is still active
		bool checkTask( size_t task_id );

		// xml information
        void getInfo( std::ostringstream& output);
	
        // Notification from the engine about finished tasks
        void notify( engine::Notification* notification );
 
        // Add files of the running tasks
        void addCurrentGeneratedFiles( std::set<std::string> &file_names );
        
	};
	 
}

#endif