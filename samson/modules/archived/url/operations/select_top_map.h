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

#ifndef _H_SAMSON_url_select_top_map
#define _H_SAMSON_url_select_top_map


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Vector_UInt.h>
#include <samson/modules/url/ServerHits.h>

#include "ServerHitsAccumulator.h"

namespace samson{
namespace url{


	class select_top_map : public samson::Map
	{

	  samson::system::String key_server;
	  samson::system::Vector_UInt value_times;

	  samson::system::UInt key_output;
	  samson::url::ServerHits value_serverHits;

	  ServerHitsAccumulator serverHitsAccumulator;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		  size_t num_top_urls = environment->getSizeT( "url.num_top_urls" ,  10 );

		  //OLM_M(("Init map operation with top %d urls", num_top_urls));

		  serverHitsAccumulator.init( num_top_urls );

		    for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
		    {
		      key_server.parse( inputs[0].kvs[i]->key );
		      value_times.parse( inputs[0].kvs[i]->value );

		      size_t local_hits = 0;
			  for (int i = 0 ; i < value_times.values_length ; i++)
				 local_hits += value_times.values[i].value;
		      
		      serverHitsAccumulator.add( key_server.value , local_hits );
		      
		      //OLM_M(("Pushing %s with %d hits", key_server.value.c_str() , local_hits));

		    }

		    // Emit the local top 100 servers
		    for (int i = 0 ; i < serverHitsAccumulator.size ; i++)
		    {
		        if( serverHitsAccumulator.hits[i] > 0 )
			{
			  value_serverHits.server.value = serverHitsAccumulator.servers[i];
			  value_serverHits.hits.value = serverHitsAccumulator.hits[i];

			  key_output.value = 0; // common  place for the reduce
			  writer->emit( 0 , &key_output , &value_serverHits );
			}
		    }
		}

	};


} // end of namespace samson
} // end of namespace url

#endif
