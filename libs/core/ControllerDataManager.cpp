

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat
#include "ControllerQueue.h"				// ss::ControllerQueue
#include "au_map.h"							// au::insertInMap
#include "DataManager.h"					// ss::DataManagerCommandResponse
#include "SamsonController.h"				// ss::SamsonController
#include "KVInfo.h"							// ss::KVInfo
#include "ModulesManager.h"					// Utility functions
#include "ObjectWithStatus.h"				// getStatusFromArray(.)
#include "samson/Operation.h"				// ss::Operation

namespace ss {
	
	
	DataManagerCommandResponse ControllerDataManager::_run( std::string command )
	{
		DataManagerCommandResponse response;
		
		au::CommandLine  commandLine;
		commandLine.parse( command );
		
		if( commandLine.get_num_arguments() == 0)
		{
			response.output = "No command found";
			response.error = true;
			return response;
		}

		if( commandLine.get_argument(0) == "add_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 4 )
			{
				response.output = "Usage: add_queue name <keyFormat> <valueFormat>";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			std::string keyFormat= commandLine.get_argument( 2 );
			std::string	valueFormat = commandLine.get_argument( 3 );
			
			if( !controller->modulesManager.checkData( keyFormat ) )
			{
				response.output = "Unsupported data format " + keyFormat + "\n";
				response.error = true;
				return response;
			}
			
			if( !controller->modulesManager.checkData( valueFormat ) )
			{
				std::ostringstream output;
				output << "Unsupported data format " + keyFormat + "\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			// Check if queue exist
			if( queues.findInMap( name ) != NULL )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			KVFormat format = KVFormat::format( keyFormat , valueFormat );
			ControllerQueue *tmp = new ControllerQueue(name , format);
			queues.insertInMap( name , tmp );
			
			response.output = "OK";
			return response;
		}

		if( commandLine.get_argument(0) == "remove_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: remove_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			
			// Check if queue exist
			ControllerQueue *tmp =  queues.extractFromMap(name);
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
				delete tmp;
			
			response.output = "OK";
			return response;
		}
		
// DATA QUEUES
		
		if( commandLine.get_argument(0) == "add_data_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: add_data_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );

			
			// Check if queue exist
			if( data_queues.findInMap( name ) != NULL )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			DataQueue *tmp = new DataQueue(name );
			data_queues.insertInMap( name , tmp );
			
			response.output = "OK";
			return response;
		}
		
		if( commandLine.get_argument(0) == "remove_data_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: remove_data_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			
			// Check if queue exist
			DataQueue *tmp =  data_queues.extractFromMap(name);
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
				delete tmp;
			
			response.output = "OK";
			return response;
		}		
		
#pragma mark Add data		
		
		if( commandLine.get_argument(0) == "add_data_file" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 5 )
			{
				response.output = "Usage: add_data_file worker fileName size queue";
				response.error = true;
				return response;
			}
			
			int worker = (int)strtol(commandLine.get_argument( 1 ).c_str(), (char **)NULL, 10);
			
			std::string fileName	= commandLine.get_argument( 2 );
			size_t size = strtoll(commandLine.get_argument( 3 ).c_str(), (char **)NULL, 10);
			
			std::string queue		= commandLine.get_argument( 4 );
			
			// Check valid queue
			DataQueue *q =  data_queues.findInMap(queue);
			if( !q )
			{
				std::ostringstream output;
				output << "Data Queue " << queue << " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}

			q->addFile( worker , fileName , size );
			response.output = "OK";
			return response;
		}			
		
		
		
		
		response.error = true;
		response.output = std::string("Unknown command: ") +  command;
		return response;
	}
	
	void ControllerDataManager::_un_run( std::string command )
	{
		// Undo a particular action
	}
	
	
	std::string ControllerDataManager::getLogFileName(   )
	{
		std::ostringstream fileName;
		fileName << "/var/samson/log/log_controller";
		return fileName.str();
	}
	
	std::string ControllerDataManager::getStatus()
	{
		std::ostringstream o;
		
		lock.lock();
		o << getStatusFromArray( queues );
		lock.unlock();
		
		return o.str();
		
	}		
	
	void ControllerDataManager::helpQueues( network::HelpResponse *response , network::Help help )
	{
		lock.lock();
		std::map< std::string , ControllerQueue*>::iterator i;
		for (i = queues.begin() ; i!= queues.end() ;i++)
		{
			//o << i->first << i->second->str() << "\n";

			
			if( !help.has_name() || i->first == help.name() )
			{
				
				ControllerQueue *queue = i->second;
				network::Queue *q = response->add_queue();
				q->set_name( i->first );
				
				// Format
				fillKVFormat( q->mutable_format() , queue->format() );
				
				//Info
				fillKVInfo( q->mutable_info(), queue->info() );
				
			}
			
			
		}
	
		std::map< std::string , DataQueue*>::iterator ii;
		for (ii = data_queues.begin() ; ii!= data_queues.end() ;ii++)
		{
			DataQueue *queue = ii->second;
			
			if( !help.has_name() || queue->getName() == help.name() )
			{
				network::DataQueue *q = response->add_data_queue();
				q->set_name( queue->getName() );
				q->set_size( queue->getSize() );	
			}
		}		
		
		lock.unlock();
	}
	
	void ControllerDataManager::retreveInfoForTask( ControllerTaskInfo *info )		
	{
		lock.lock();
		_retreveInfoForTask( info );
		lock.unlock();
		
	}
	
	void ControllerDataManager::_retreveInfoForTask( ControllerTaskInfo *info )		
	{
		
		std::ostringstream error_message;
		
		// Check inputs	
		for (int i = 0 ; i < (int)info->inputs.size() ; i++)
		{
			std::string queue_name =  info->inputs[i] ;
			
			ControllerQueue *q = queues.findInMap( queue_name );
			if( q )
			{
				KVFormat queue_format = q->format();
				KVFormat parameter_format = info->operation->getInputFormat(i);
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->setError( error_message.str() );
					return; 
				}
			}
			else
			{
				error_message << "Unknown queue " << info->inputs[i];
				info->setError( error_message.str() );
				return; 
			}
		}
		
		// Check output	
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			std::string queue_name = info->outputs[i];
			
			ControllerQueue *q = queues.findInMap( queue_name );
			if( q )
			{
				KVFormat queue_format = q->format();
				KVFormat parameter_format = info->operation->getOutputFormat(i);
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->setError( error_message.str() );
					return; 
				}
			}
			else
			{
				error_message << "Unknown queue " << info->outputs[i];
				info->setError( error_message.str() );
				return; 
			}
		}
		
		
	}
	
	
}
