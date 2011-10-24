
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_simple_mobility_reduce_cdrs
#define _H_SAMSON_simple_mobility_reduce_cdrs

#include "au/string.h"

#include <samson/module/samson.h>
#include <samson/modules/simple_mobility/Position.h>
#include <samson/modules/simple_mobility/User.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace simple_mobility{


	class reduce_cdrs : public samson::Reduce
	{

	   samson::system::UInt key;            // User identifier
	   samson::simple_mobility::User user;    // State information

	   samson::simple_mobility::Position position;  // Updated position

	   samson::system::String message;            // Message emited at the output

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt simple_mobility.Position  
input: system.UInt simple_mobility.User  
output: system.UInt system.String
output: system.UInt simple_mobility.User

helpLine: Update internal state
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
           if ( inputs[1].num_kvs == 0 )
           {
              // New user state
              user.init();
              key.parse( inputs[0].kvs[0]->key );
           }
           else
           {
			  // Previous state
			  user.parse( inputs[1].kvs[0]->value ); // We only consider one state
			  key.parse( inputs[1].kvs[0]->key );
           }

		   if( inputs[0].num_kvs > 0)
		   {

			  // Update in the position
			  position.parse( inputs[0].kvs[ inputs[0].num_kvs - 1 ]->value );

			  // Emit whatever is necessary
			  if( user.isTracking() )
			  {
                 message.value = au::str("Update osition of user %lu to [%d,%d] %s", key.value , position.x.value , position.y.value , position.time.str().c_str() );
				 writer->emit( 0 , &key,  &message );				 
			  }

			  // update state
			  user.position.copyFrom( &position );

		   }


		   // Emit the state at the output
		   writer->emit( 1 , &key , &user );


		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace simple_mobility
} // end of namespace samson

#endif
