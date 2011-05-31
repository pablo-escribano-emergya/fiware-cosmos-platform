#ifndef _H_CONTROLLER_DATA_MANAGER
#define _H_CONTROLLER_DATA_MANAGER

#include "au/Lock.h"				// au::Lock
#include <sstream>				// std::ostringstream
#include "samson/module/KVFormat.h"	// samson:: KVFormat
#include "au/Format.h"				// au::Format
#include <map>					// std::map
#include "samson/data/DataManager.h"		// samson::DataManager
#include "au/map.h"				// au::map
#include "samson/common/samson.pb.h"			// samson::network::...
#include "MonitorParameter.h"	// samson::MonitorBlock
#include "samson/common/coding.h"				// KVInfo

namespace samson {
	
	
	class Queue;
	class ControllerTask;
	class DataManagerCommandResponse;
	class SamsonController;
	class ControllerTaskInfo;
	
	class ActiveTask
	{
	public:

		std::set<std::string> files;		// List of files to not delete from disk
		void addFiles( Queue *q );			// Add all the files of this queue to the list of protected files
		
	};
	
	/**
	 Data manager at the controller
	 */
	
	class ControllerDataManager : public DataManager
	{
		//au::Lock lock;	(lock is included in DataManager generic object )
		
		KVInfo info_kvs;										// Global info of the entire system ( all types of key-values )
		KVInfo info_txt;										// Global info of the entire system ( txt files )
		
		au::map< std::string , Queue> queues;						// List of ALL KeyValue queues
		au::map< size_t , ActiveTask > tasks;						// List of active tasks ( information about the files that should not be deleted )
		
        
        // List of stream - queues contained in the system
        au::map< std::string , network::StreamQueue > stream_queues;    

        SamsonController *controller;
		
	public:

		ControllerDataManager( SamsonController *controller );
		~ControllerDataManager();
		
		/**
		 Get a string describing status
		 Usefull for debuggin
		 */
		
		std::string getStatus();

		/**
		 Get the fileName of the log file
		 */
		
		static std::string getLogFileName( );

		void fill( network::QueueList *ql , std::string command );
		void fill( network::DownloadDataInitResponse* response , std::string queue );
		
		void retreveInfoForTask( size_t job_id , ControllerTaskInfo *info , bool clear_inputs );		
		
		static std::string getAddFileCommand(int worker , std::string fileName , size_t size , size_t kvs , std::string queue )
		{
			std::ostringstream command;
			command << "add_file " <<  worker << " " << fileName << " " << size << " " << kvs << " " << queue;
			return command.str();
		}		

		static std::string getRemoveFileCommand(int worker , std::string fileName , size_t size , size_t kvs , std::string queue )
		{
			std::ostringstream command;
			command << "remove_file " <<  worker << " " << fileName << " " << size << " " << kvs << " " << queue;
			return command.str();
		}		
		
		KVInfo get_info_kvs()
		{
			return info_kvs;
		}
		
		KVInfo get_info_txt()
		{
			return info_txt;
		}

		void getQueuesMonitorInfo( std::vector<QueueMonitorInfo> &queues_names );
				
	private:

		
		void _retreveInfoForTask( ControllerTaskInfo *info );		
		
		virtual void _clear();
		virtual DataManagerCommandResponse _run( size_t task, std::string command );
		virtual void _beginTask( size_t task );
		virtual void _cancelTask( size_t task );
		virtual void _finishTask( size_t task );
		

		
		// Handy functions
		static void copyQueue( Queue * q_from , network::FullQueue * q_to );

		
	};

}

#endif
