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

#ifndef _H_SAMSON_mob2_cluster_get_majPoi_by_node
#define _H_SAMSON_mob2_cluster_get_majPoi_by_node


#include <samson/module/samson.h>
#include <samson/modules/mob2/TwoInt.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


	class cluster_get_majPoi_by_node : public samson::Reduce
	{
		// Inputs
		samson::system::UInt nodelbl;
		TwoInt poilbl_count; // num1: poi label	num2: counter
		// Outputs
		samson::system::UInt poilbl;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt mob2.TwoInt  
output: system.UInt system.UInt

extendedHelp: 		Get max majority POI label by node label

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			uint64_t max, label;
			max = label = 0;

			nodelbl.parse(inputs[0].kvs[0]->key);
			for(uint64_t i=0; i<inputs[0].num_kvs; i++)
			{
				poilbl_count.parse(inputs[0].kvs[i]->value);
				if(poilbl_count.num2.value > max)
				{
					label = poilbl_count.num1.value;
					max = poilbl_count.num2.value;
				}
			}
			poilbl.value = label;
			writer->emit(0, &nodelbl,&poilbl);
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
