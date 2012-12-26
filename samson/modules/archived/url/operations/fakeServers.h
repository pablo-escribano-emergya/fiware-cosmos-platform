/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_fakeServers
#define _H_SAMSON_url_fakeServers


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

namespace samson{
namespace url{


	class fakeServers : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
		  samson::system::String server;
		    samson::system::UInt id; 

                   // Recover the number of servers from the environment
                   size_t num_servers = environment->getSizeT( "url.num_servers" ,  1000000 );
		   for ( size_t i = 0 ; i < num_servers ; i++)
		   {
		     std::ostringstream o;
		     o << "ServerName_" << i;
		     server.value = o.str();

		     id.value = i;

		     writer->emit( 0 , &server , &id );

		     // It can also be convenient to have the reversed data-set
		     writer->emit( 1, &id, &server);
		    
		   }

		}


	};


} // end of namespace samson
} // end of namespace url

#endif
