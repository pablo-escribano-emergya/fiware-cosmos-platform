
#ifndef _H_CONTROLLER_TASK_INFO
#define _H_CONTROLLER_TASK_INFO

#include <string>
#include <vector>
#include "CommandLine.h"				// au::CommandLine
#include "samson.pb.h"

namespace ss {

	class Operation;
	class Job;

	class ControllerTaskInfo
	{
		
	public:
		
		ControllerTaskInfo( Job *job , Operation *operation  , au::CommandLine *cmdLine );

		~ControllerTaskInfo();
		
		Job *job;	// Pointer to a job ( used to retrieve environment variables )
		
		void setError( std::string _error_message);
				
		std::string operation_name;				// Name of the operation
		Operation * operation;					// Pointer to the operation
		
		std::vector<std::string> inputs;				// Name of the input queues
		std::vector<std::string> outputs;				// Name of the output queues

		// Output generated by Data Manager
		std::vector<network::FileList*> input_files;		// List of files for each input
		
		// Output queues with format ( inseted by DataManager )
		std::vector<network::Queue*> output_queues;		
		
		bool error;
		std::string error_message;
	};
	
}

#endif