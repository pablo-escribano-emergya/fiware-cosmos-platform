#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Message.h"            // Message::MessageCode, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "Delilah.h"			// Own interfce
#include "Packet.h"				// ss::Packet

namespace ss {


	void* runThreadConsole(void *p)
	{
		Delilah *d = ((Delilah*)p);
		d->runConsole();
		return NULL;
	}
	
	void Delilah::run()
	{
		
		// Run a console
		pthread_t t2;
		pthread_create(&t2, NULL, runThreadConsole, this);
		
		// Main run_loop to the network interface
		network->run();	
		
	}
/*
	void Delilah::test()
	{
		while( ! network->ready() && !finish )
		{
			showMessage("Awaiting network interface ready");
			//LM_T(LMT_READY, ("Awaiting network interface ready"));
			sleep(1);
		}
		
		// Testing to sent something to the controller
		while( !finish )
		{
			Packet p;
			p.message.set_command("Hello there");	// Init the command inside the message
			p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
			network->send( &p, network->controllerGet(), NULL );
			
			sleep(2);
		}		
	}
*/	
	
	size_t Delilah::sendMessageToController( std::string message )
	{
		Packet p;

		network::Command * command = p.message.mutable_command();
		command->set_sender_id( local_id++ );
		command->set_command( message );
		
		network->send(this, network->controllerGetIdentifier(), Message::Command, NULL, 0, &p);
		
		return (local_id - 1);
	}
	
	int Delilah::receivedMessage( size_t id , bool error , bool finished , std::string message )
	{
		// Todo with the received packet
		std::ostringstream txt;
		txt << "----------------------------------------------------------------" << std::endl;
		txt << "Answer for command " << id << ": ";
		
		if( finished )
			txt << "(FINISHED)";

		txt << std::endl;
		txt << "----------------------------------------------------------------" << std::endl;
		txt << message << std::endl;
		txt << "----------------------------------------------------------------" << std::endl;
		
		if (error)
		{
			console->writeErrorOnConsole(txt.str());
			return 1;
		}

		console->writeOnConsole(txt.str());
		return 0;
	}
	

	int Delilah::receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet)
	{
		if (msgCode == Message::CommandResponse)
		{
			std::string message  = packet->message.command_response().response();
			bool        error    = packet->message.command_response().error();
			bool        finish   = packet->message.command_response().finish();
			size_t      id       = packet->message.command_response().sender_id();
			
			return receivedMessage(id, error, finish, message);
		}

		LM_RE(1, ("Unknown message code %d", msgCode));
	}
	
	void Delilah::notificationSent(size_t id, bool success)
	{
		// Do something
	}

}

