
#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include "au/list.h"        // au::list
#include "au/map.h"         // au::map

#include "engine/EngineNotification.h"  // engine::NotificationListener

#include <string>           // std::string

namespace samson {
    namespace stream {
        
        class QueueTask;
        
        class QueueTaskManager : public ::engine::NotificationListener
        {
            size_t id;                              // Id of the current task

            au::list< QueueTask > queueTasks;           // List of pending task to be executed
            au::map< size_t , QueueTask > runningTasks; // Map of running tasks
            
        public:
            
            QueueTaskManager();
            
            void add( QueueTask* task );
            
            void runTasksIfNecessary();
            
            std::string getStatus();
            
            // Notifications
            void notify( engine::Notification* notification );
            bool acceptNotification( engine::Notification* notification );
            
            
        };
        
    }
    
}

#endif
