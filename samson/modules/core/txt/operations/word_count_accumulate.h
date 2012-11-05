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

#ifndef _H_SAMSON_txt_word_count_accumulate
#define _H_SAMSON_txt_word_count_accumulate


#include <samson/module/samson.h>

#include "samson/modules/system/String.h"
#include "samson/modules/system/UInt.h"

namespace samson{
namespace txt{


	class word_count_accumulate : public samson::Reduce
	{

	public:
		
		samson::system::String word;
		samson::system::UInt count;
		samson::system::UInt total;


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			
			if( inputs[0].num_kvs != 0 )
			  word.parse( inputs[0].kvs[0]->key );
			else if( inputs[1].num_kvs != 0 )
			  word.parse( inputs[1].kvs[0]->key );
			else
			  return; // No input at all
			
			total.value = 0;
			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				count.parse( inputs[0].kvs[i]->value );
				total.value += count.value;
			}
			for (size_t i = 0 ; i < inputs[1].num_kvs ; i++)
			{
				count.parse( inputs[1].kvs[i]->value );
				total.value += count.value;
			}
			
			writer->emit( 0, &word , &total );
		}


	};


} // end of namespace samson
} // end of namespace system

#endif
