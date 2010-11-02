#include <iostream>				        // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*
#include "coreTracelevels.h"            // LMT_*

#include "Message.h"                    // Message::WorkerStatus, ...
#include "workerStatus.h"               // Message::WorkerStatusData
#include "Macros.h"                     // EXIT, ...
#include "Packet.h"				        // ss::Packet
#include "Network.h"			        // NetworkInterface
#include "Endpoint.h"			        // EndPoint
#include "CommandLine.h"		        // CommandLine
#include "ModulesManager.h"		        // ss:ModulesManager
#include "ControllerTaskManager.h"		// ss:ControllerTaskManager
#include "ControllerTask.h"				// ss:ControllerTask
#include "SamsonController.h"	        // Own interface ss::SamsonController



namespace ss {

	SamsonController::SamsonController(int arg, const char *argv[],  NetworkInterface *_network) : 	taskManager(this)
	{
		
		std::string  setupFileName;						// Filename with setup information
		int          port;								// Local port where this controller listen
		
		
		network = _network;
		network->setPacketReceiverInterface(this);
		
		std::string  trace;
		
		// Parse input command lines
		au::CommandLine commandLine;
		commandLine.parse(arg, argv);
		
		commandLine.set_flag_int("port",      SAMSON_CONTROLLER_DEFAULT_PORT);
		commandLine.set_flag_string("setup",  SAMSON_SETUP_FILE);
		commandLine.set_flag_string("t",      "255");
		commandLine.set_flag_boolean("r");
		commandLine.set_flag_boolean("w");
		
		commandLine.parse(arg, argv);
		
		port			= commandLine.get_flag_int("port");
		setupFileName   = commandLine.get_flag_string("setup");
		lmReads			= commandLine.get_flag_bool("r");
		lmWrites		= commandLine.get_flag_bool("w");
		
		
		// Load setup
		LM_T(LMT_CONFIG, ("calling loadSetup"));			
		//std::vector <Endpoint> workerEndPoints = loadSetup(setupFileName);
		std::vector <std::string> workerPeers = getworkerPeers(setupFileName);
		
		// Define the endpoints of the network interface
		
		LM_T(LMT_CONFIG, ("workerEndPoints.size: %d", workerPeers.size()));
		network->initAsSamsonController(port, workerPeers);
		//network->initAsSamsonController(myEndPoint, workerEndPoints);
	}	



	void SamsonController::run()
	{
		data.initDataManager(data.getLogFileName());			// Init the data manager
		network->run();											// Run the network interface (blocked)
	}



	int SamsonController::receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet)
	{
		au::CommandLine     cmdLine;
		bool                success;
		std::ostringstream  output;

		switch (msgCode)
		{
		case Message::Command:
			cmdLine.parse(packet->message.command().command());
			
			if (cmdLine.get_num_arguments() == 0)
				LM_RE(1, ("No args ..."));
			
			// General status command
			if (cmdLine.get_argument(0) == "status")
			{
				Endpoint* ep;
				int       workers;
				int       ix;
				int       workersFound;

				output << "Status of Controller" << std::endl;			
				output << "== ************** ==" << std::endl;
				output << data.status();
				output << taskManager.status();

				// Information about each server
				workers      = network->getNumWorkers();
				workersFound = 0;
				ix           = 0;
				do
				{
					ep = network->endpointLookup(3 + ix); /* 0,1 and 2 occupied by ME, LISTEN and CONTROLLER */
					++ix;
					if (ep == NULL)
						continue;
					output << "Worker " << ix << std::endl;
					output << "\t- Cores:\t" << ep->status->cpuInfo.cores << std::endl;
					output << "\t- CPU Load:\t" << ep->status->cpuInfo.load << "%" << std::endl;
					output << "\t- Net (dev 0) Rcv Speed:\t" << ep->status->netInfo.iface[0].rcvSpeed << " bps" << std::endl;
					output << "\t- Net (dev 0) Snd Speed:\t" << ep->status->netInfo.iface[0].sndSpeed << " bps" << std::endl;
					output << "\t- Net (dev 1) Rcv Speed:\t" << ep->status->netInfo.iface[1].rcvSpeed << " bps" << std::endl;
					output << "\t- Net (dev 1) Snd Speed:\t" << ep->status->netInfo.iface[1].sndSpeed << " bps" << std::endl;
					++workersFound;
				} while (workersFound < workers);
				
				// Respond with controller status
				sendDalilahAnswer(packet->message.command().sender_id(), fromId, false, true, output.str());
				return 0;
			}
			
			// Try to schedule the command
			success = taskManager.addTask(fromId, packet->message.command().command(), output);
			
			// Send something back to dalilah (if error -> it is also finished)
			sendDalilahAnswer(packet->message.command().sender_id(), fromId, !success, !success,  output.str());
			break;

		case Message::WorkerTaskConfirmation:
			taskManager.notifyWorkerConfirmation(fromId, packet->message.worker_task_confirmation());
			break;

		case Message::WorkerStatus:
			int workerId;

			workerId = network->getWorkerFromIdentifier(fromId);			
			if (workerId == -1)
				LM_RE(2, ("getWorkerFromIdentifier(%d) failed", fromId));
			status[workerId] = *((Message::WorkerStatusData*) dataP);
			break;
			
		default:
			LM_X(1, ("msg code '%s' not treated ...", messageCode(msgCode)));
			break;
		}

		return 0;
	}

	void SamsonController::notificationSent(size_t id, bool success)
	{
		// Do something
	}
	
	std::vector <std::string> SamsonController::getworkerPeers(std::string fileName)
	{
		std::vector <std::string> workerPeers;
		
		FILE *file = fopen(fileName.c_str(),"r");
		if (!file)
		{
			LM_E(("Config file '%s' not found", fileName.c_str()));
			return workerPeers;
		}
		
		workerPeers.clear();
		char line[2000];
		while (fgets(line, sizeof(line), file))
		{
			au::CommandLine c;
			c.parse(line);
			
			if (c.get_num_arguments() == 0)
				continue;
			
			std::string mainCommand = c.get_argument(0);
			if (mainCommand[0] == '#')
				continue;
			
			if (mainCommand == "worker")
			{
				if (c.get_num_arguments() >= 2)	
				{
					workerPeers.push_back(c.get_argument(1));
					LM_T(LMT_CONFIG, ("added worker: '%s'", c.get_argument(1).c_str()));
				}
			}					
		}
		
		fclose(file);
		
		return workerPeers;
	}
	
	
#pragma mark Sent messages
	
	
	void SamsonController::sendDalilahAnswer(size_t sender_id, int dalilahIdentifier, bool error, bool finished, std::string answer_message)
	{
		// Get status of controller
		Packet p2;
		
		network::CommandResponse *response = p2.message.mutable_command_response();
		response->set_response(answer_message);
		response->set_error(error);
		response->set_finish(finished);
		response->set_sender_id(sender_id);
		
		network->send(this, dalilahIdentifier, Message::CommandResponse, NULL, 0, &p2);
	}
	


	void SamsonController::sendWorkerTasks(ControllerTask *task)
	{
		// Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
		
		for (int i = 3 ; i < 3 + network->getNumWorkers() ; i++)
		{
			LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", i));
			sendWorkerTask(i, task->getId(), task->getCommand());
		}
	}	
	


	void SamsonController::sendWorkerTask(int workerIdentifier, size_t task_id, std::string command)
	{
		// Get status of controller
		Packet p2;

		network::WorkerTask *t = p2.message.mutable_worker_task();
		t->set_command(command);
		t->set_task_id(task_id);

		LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", workerIdentifier));
		network->send(this, workerIdentifier, Message::WorkerTask, NULL, 0, &p2);
	}
}
