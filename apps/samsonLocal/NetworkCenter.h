#ifndef _H_NETWORK_FAKE_CENTER
#define _H_NETWORK_FAKE_CENTER

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream
#include <list>					// std::list
#include <iostream>				// std::cout

#include "au/Token.h"				// au::Token
#include "au/TokenTaker.h"                          // au::TokenTake

#include "samson/network/Packet.h"				// samson::Packet
#include "samson/delilah/Delilah.h"			// ss:Delilah
#include "samson/worker/SamsonWorker.h"		// samson::SamsonWorker
#include "samson/controller/SamsonController.h"	// samson:: SasonController

#include "FakeEndpoint.h"		// samson::EndPoint
#include "NetworkFake.h"        // NetworkFake

namespace samson {
	
	
	class NetworkFakeCenterPacket
	{
	public:

		Message::MessageCode msgCode;
		Packet *packet; 
		int from;
		int to;
		PacketSenderInterface* sender;
		
		NetworkFakeCenterPacket(Message::MessageCode _msgCode, Packet *_packet, int _from , int _to , PacketSenderInterface* _sender )
		{
			msgCode = _msgCode;
			from = _from;
			to = _to;
			packet = _packet;	// Copy the packet
			sender = _sender;
		}
		
		~NetworkFakeCenterPacket()
		{
            // If the packet has been finally sent, this should be NULL
			// Remove the packet since it is responsability of the delegate
            
			if( packet)
				delete packet;
		}
								 
	};
	
	/**
	 Center of operation of the NetworkFakeCenter
	 */
	
	class NetworkFakeCenter
	{
	public:
		
		std::map<int,NetworkFake*> network; 
		std::map<int,FakeEndpoint*> endpoint; 

		au::Token token;			// Lock to protect the pending packet list
		
		std::vector<NetworkFakeCenterPacket*> pendingPackets;
		
		int num_workers;
		
		NetworkFakeCenter( int _num_workers ) : token("NetworkFakeCenter")
		{
			num_workers = _num_workers;
			
			for (int i = 0 ; i < num_workers ; i++)
			{
				// Create a fake Network interface element
				network.insert( std::pair<int,NetworkFake*>( i , new NetworkFake(i , this) ) );
				
				// Create the right endpoint
				endpoint.insert( std::pair<int,FakeEndpoint*>( i , new FakeEndpoint("worker" , i ) ) );
			}

			network.insert( std::pair<int,NetworkFake*>( -1 , new NetworkFake(-1 , this) ) );
			endpoint.insert( std::pair<int,FakeEndpoint*>( -1 , new FakeEndpoint("controller" , -1 ) ) );

			network.insert( std::pair<int,NetworkFake*>( -2 , new NetworkFake(-2 , this) ) );
			endpoint.insert( std::pair<int,FakeEndpoint*>( -2 , new FakeEndpoint("dalilah" , -2 ) ) );
			
		}
		
		~NetworkFakeCenter()
		{
			// Clear everything
			
			for( std::map<int,NetworkFake*>::iterator n = network.begin() ; n != network.end() ; n++ )
				delete n->second;
			network.clear();

			for( std::map<int,FakeEndpoint*>::iterator e = endpoint.begin() ; e != endpoint.end() ; e++ )
				delete e->second;
			endpoint.clear();
			
			// Just in case there are pending packets
			for ( size_t i = 0 ; i < pendingPackets.size() ; i++ )
				delete pendingPackets[i];

			pendingPackets.clear();
			
		}
		
		NetworkFake* getNetwork( int worker_id )
		{
			std::map<int,NetworkFake*>::iterator i =  network.find( worker_id );
			if( i == network.end() )
			  LM_X(1,("Error in NetworkFake::getNetwork for worker %d", worker_id));
			return i->second;
		}
		
		FakeEndpoint* getEndpoint( int worker_id )
		{
			std::map<int,FakeEndpoint*>::iterator i =  endpoint.find( worker_id );
			if( i == endpoint.end() )
			  LM_X(1,("Error in NetworkFake::getEndpoint for endpoint %d",worker_id));

			return i->second;
		}
		
		void runInBackground();
		
		void run()
		{
			while( true )
			{
				// Send packets to the rigth directions
                std::vector<NetworkFakeCenterPacket*> sendingPackets;
                
                {
                    // Protect retaining the token
                    au::TokenTaker tt( &token );
                    
                    sendingPackets.insert( sendingPackets.end() , pendingPackets.begin() , pendingPackets.end() );
                    pendingPackets.clear();
                    
                }
				
				if( sendingPackets.size() > 0 )
				{
					
					for (std::vector<NetworkFakeCenterPacket*>::iterator p = sendingPackets.begin() ; p < sendingPackets.end() ; p++)
					{
						NetworkFakeCenterPacket* pp = *p;
						processPendingPacket(pp);
                        pp->packet = NULL;
						delete pp;
					}
				}
				else
                {
                    au::TokenTaker tt(&token);
                    tt.stop();
                }
			}
		}
		
		void addPacket(NetworkFakeCenterPacket *p)
		{
            au::TokenTaker tt( &token );

			pendingPackets.push_back(p);

            
			// Wake up the background thread to process this packages
			tt.wakeUpAll();
			
		}
		
		
		void processPendingPacket( NetworkFakeCenterPacket *p )
		{
			// We look the endpoint worker id and use that to send the packet
			// NetworkFake* network = getNetwork(p->to);
			// network->receiver->receive( ... );

			NetworkFake* network = getNetwork( p->to );
			p->packet->fromId = p->from;
			p->packet->msgCode = p->msgCode;
			network->receiver->_receive( p->packet );			
		}
	};
}

#endif