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

#ifndef _H_SAMSON_webp_update_user
#define _H_SAMSON_webp_update_user


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/webp/Log.h>
#include <samson/modules/webp/User.h>


namespace samson{
namespace webp{


	class update_user : public samson::Reduce
	{

	   samson::system::String user_id;
	   samson::webp::User user;

	   samson::webp::Log log;


	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.String webp.Log  
//  input: system.String webp.User  
//  output: system.String webp.User
//  
// helpLine: Update profile for this user
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		   if( inputs[1].num_kvs > 0 )
		   {
			  user_id.parse( inputs[1].kvs[0]->key );
			  user.parse( inputs[1].kvs[0]->value );
			  user.update(); // Update counter with time after the last update
		   }
		   else if ( inputs[0].num_kvs > 0 )
		   {
			  user_id.parse( inputs[0].kvs[0]->key );
			  user.init( user_id.value );
		   }
		   else
			  return; // No key values at 0 or 1


		   for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		   {
			  log.parse( inputs[0].kvs[i]->value );
			  
			  for ( int c = 0 ; c <  log.categories_length ; c++ )
			  {
				 // Add this category to user profile
				 user.add_hit( &log.categories[c] );
			  }

			  if( i == ( inputs[0].num_kvs - 1 ) )
			  {
				 // Take the last log
				 user.last_log.copyFrom( &log );

			  }

		   }

// Always emit state at the output
		   writer->emit( 0 , &user_id , &user );

		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace webp
} // end of namespace samson

#endif
