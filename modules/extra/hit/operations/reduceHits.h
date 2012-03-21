
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_hit_reduceHits
#define _H_SAMSON_hit_reduceHits

#include  <time.h>

#include <samson/module/samson.h>
#include <samson/modules/hit/Hit.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

#include "HitCollectionManager.h"

namespace samson{
namespace hit{


	class reduceHits : public samson::Reduce
	{

	   samson::system::String concept;
	   samson::hit::Hit hit;

	   samson::system::UInt tmp_count;

       // In memory accumulator of top elements per category
	   MultiHitCollectionManager manager;

	   // Current time-stamp shared by all key-values processed here
	   time_t current_time;

	   size_t time_span;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  in  system.String system.UInt
//  in  system.String hit.Hit
//  out  system.String hit.Hit
//  out  system.String hit.Hit
//  
// helpLine: Simple accumulator in stream mode
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		   // Get a common time stamp for all the key-values processed here
		   current_time = time(NULL);

           // Get time span for environment variables
		   time_span = environment->getSizeT( "time_span" ,  300 ); // By default 5 minuts average
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{


		   if( inputs[1].num_kvs > 0 )
		   {
			  concept.parse( inputs[1].kvs[0]->key );
			  hit.parse( inputs[1].kvs[0]->value );
			  hit.setTime( current_time , time_span ); // Update state to current time
		   }
		   else
		   {			  
			  concept.parse( inputs[0].kvs[0]->key );
			  hit.init( concept.value , current_time );
		   }

		   
		   // Take all input words into acocunt...
		   for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		   {
			  tmp_count.parse( inputs[0].kvs[i]->value );
			  hit.count.value += tmp_count.value;
		   }


		   if( hit.count.value >= 0.1 )
		   {

			  // Store in the manager to emit when finishing...
			  manager.add( &hit );
		   
			  // Emit the state to keep track
			  //OLM_M(("Emiting %s %lu" , concept.value.c_str() ,  count.value ));

			  writer->emit( 1 , &concept , &hit );

		   }
		}

		void finish( samson::KVWriter *writer )
		{
		   //OLM_M(("Emiting output hits...."));
		   manager.emit_hits( writer );
		}



	};


} // end of namespace hit
} // end of namespace samson

#endif