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

#ifndef _H_SAMSON_c4_filter_solved
#define _H_SAMSON_c4_filter_solved


#include <samson/module/samson.h>
#include <samson/modules/c4/Board.h>
#include <samson/modules/c4/Info.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace c4{


	class filter_solved : public samson::Map
	{

	   samson::c4::Board board;
	   samson::c4::Info info;
	   samson::system::Void void_value;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: c4.Board c4.Info  
//  output: c4.Board system.Void
// 
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		   for ( size_t i = 0 ; i < inputs[0].num_kvs ;i++ )
		   {
			  board.parse( inputs[0].kvs[i]->key );
			  info.parse( inputs[0].kvs[i]->value );

			  if ( info.winner.value  != C4_NONE )
				 writer->emit( 0 , &board , &void_value );
		   }
		   
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace c4
} // end of namespace samson

#endif
