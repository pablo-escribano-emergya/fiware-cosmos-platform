#ifndef SAMSON_CONTROLLER_H
#define SAMSON_CONTROLLER_H

/* ****************************************************************************
*
* FILE                     SamsonController.h
*
* DESCRIPTION				Main class for the worker elemen
*
*/
#include <iostream>						// std::cout
#include "logMsg.h"						// lmInit, LM_*
#include "Macros.h"						// EXIT, ...
#include "Network.h"					// NetworkInterface
#include "Endpoint.h"					// Endpoint
#include "CommandLine.h"				// au::CommandLine
#include "samsonDirectories.h"			// File to load setup
#include "ControllerDataManager.h"		// ss::ControllerDataManager
#include "ModulesManager.h"				// ss::ModulesManager
#include "ControllerTaskManager.h"		// ss::ControllerTaskManager
#include "traces.h"						// LMT_CONFIG, ...
#include "samson.pb.h"					// network::...
#include "workerStatus.h"               // Message::WorkerStatusData
#include "Message.h"                    // Message::WorkerStatus, ...
#include "JobManager.h"					// ss::JobManager

namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface
	{

		// Elements inside the SamsonController
		
		NetworkInterface*     network;					// Network interface
		ModulesManager        modulesManager;			// Manager of the modules ( to check data types and map/reduce/scripts functions)

		ControllerDataManager data;						// Data manager for the controller
		
		JobManager jobManager;							// Top level job manager
		ControllerTaskManager taskManager;				// Internal task manager to submit tasks ( simple string command to be executed also by workers )
		
		// Status information of the workers
		Message::WorkerStatusData status[100];		    // Status update from all workers
		
		friend class ControllerTaskManager;
		friend class Job;
		friend class JobManager;
		friend class ControllerDataManager;
		
	public:
		SamsonController( int arg , const char *argv[] ,  NetworkInterface *_network );
		
		// Main run loop
		void run();
		
		// PacketReceiverInterface
		int receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet);
		void notifyWorkerDied( int worker );

		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

	private:
		
		// Send a message back to dalilah
		void sendDelilahAnswer( size_t sender_id , int dalilahIdentifier , bool error , bool finished, std::string answer_message );
		
		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , size_t task_id , ControllerTask *task );
		
		//Internal functions to get help
		
		void getHelp( std::ostringstream &output , std::string &c);
		void getStatus(std::ostringstream &output);
		
		
	};
	
}

#endif
