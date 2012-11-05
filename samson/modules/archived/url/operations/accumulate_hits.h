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

#ifndef _H_SAMSON_url_accumulate_hits
#define _H_SAMSON_url_accumulate_hits


#include <samson/module/samson.h>
#include <samson/modules/system/Void.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/Vector_UInt.h>


#define ACCUMUATION_BUFFER_SIZE      12

namespace samson{
namespace url{


	class accumulate_hits : public samson::Reduce
	{

	  samson::system::String      key_server;
	  samson::system::Vector_UInt value_times;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		    if( inputs[0].num_kvs == 0 )
		      key_server.parse( inputs[1].kvs[0]->key );
		    else
		      key_server.parse( inputs[0].kvs[0]->key );


		    // Parse the previous vector of hits or clear the vector of hits
		    if ( inputs[1].num_kvs > 0 )
			{
		      value_times.parse( inputs[1].kvs[0]->value ); // Parse the content of hits
			  if( value_times.values_length != ACCUMUATION_BUFFER_SIZE )
			  {
				 char error_line[2000];
				 sprintf(error_line, "Invalid length for the accumulated buffer %d instead of %d", value_times.values_length ,  ACCUMUATION_BUFFER_SIZE);
				 tracer->setUserError( error_line );
				 exit(0);
			  }
			}
		    else
			{
		      value_times.valuesSetLength( ACCUMUATION_BUFFER_SIZE );
			  for (int i = 0 ; i < ACCUMUATION_BUFFER_SIZE ; i++ )
				 value_times.values[i].value = 0;
			}

		    //OLM_M(("Accumulating hits for server %s. Previous hits %d. New hits %d", key_server.value.c_str() , value_times.values_length , (int) inputs[0].num_kvs  ));

/*
			OLM_M(("Processing %s: %d %d %d %d %d %d %d %d %d %d", key_server.value.c_str()  , 
				  value_times.values[0].value,
				  value_times.values[1].value,
				  value_times.values[2].value,
				  value_times.values[3].value,
				  value_times.values[4].value,
				  value_times.values[5].value,
				  value_times.values[6].value,
				  value_times.values[7].value,
				  value_times.values[8].value,
				   value_times.values[9].value ));
*/

		    // Add the new hits
			size_t total_size = 0 ;
			for ( int i = (ACCUMUATION_BUFFER_SIZE-1); i > 0 ; i--)
			{
			   value_times.values[i].value = value_times.values[i-1].value;
			   total_size += value_times.values[i].value;
			}

			value_times.values[0].value = inputs[0].num_kvs;   // Number of hits for this last update
			total_size += value_times.values[0].value;

/*
			OLM_M(("X Processing %s: %d %d %d %d %d %d %d %d %d %d", key_server.value.c_str()  , 
				  value_times.values[0].value,
				  value_times.values[1].value,
				  value_times.values[2].value,
				  value_times.values[3].value,
				  value_times.values[4].value,
				  value_times.values[5].value,
				  value_times.values[6].value,
				  value_times.values[7].value,
				  value_times.values[8].value,
				   value_times.values[9].value ));
*/

			// Emit only if any position in the vector if > 0

			if( total_size > 0 )
			  writer->emit( 0 , &key_server , &value_times );

		}


	};


} // end of namespace samson
} // end of namespace url

#endif
