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

#ifndef _H_SAMSON_sort_reduce_identity
#define _H_SAMSON_sort_reduce_identity


#include <samson/module/samson.h>


namespace samson{
namespace sort{


	class reduce_identity : public samson::Reduce
	{
		samson::system::UInt val;
		samson::system::Void dummy;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			//OLM_T(LMT_User01, ("reduce_identity: inputs[0].num_kvs:%lu", inputs[0].num_kvs));
			for (size_t i = 0; (i < inputs[0].num_kvs); ++i)
			{
				val.parse(inputs[0].kvs[i]->key);
				writer->emit(0, &val, &dummy);
			}
		}


	};


} // end of namespace samson
} // end of namespace sort

#endif
