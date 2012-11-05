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

#ifndef _H_SAMSON_mob2_repbts_filter_num_comms
#define _H_SAMSON_mob2_repbts_filter_num_comms


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/mob2/Node_Bts_Day.h>
#include <samson/modules/system/UInt.h>
#include "mob2_environment_parameters.h"



namespace samson{
namespace mob2{


class repbts_filter_num_comms : public samson::Reduce
{
	// Inputs
	samson::system::UInt node;
	Node_Bts_Day input;
	// Outputs
	samson::system::UInt ncomms;

	// Configuration thresholds
	unsigned int conf_min_number_total_calls;
	unsigned int conf_max_number_total_calls;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob2.Node_Bts_Day
	input: system.UInt cdr.mobCdr
	input: system.UInt cdr.mobCdr
	output: system.UInt system.UInt

	extendedHelp: 		Count of number of coms by client, and filter according the total number communications
	Output1: Clients in filter

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
		conf_min_number_total_calls = environment->getInt(MOB2_PARAMETER_MIN_NUMBER_TOTAL_CALLS, MOB2_PARAMETER_MIN_NUMBER_TOTAL_CALLS_DEFAULT);
		conf_max_number_total_calls = environment->getInt(MOB2_PARAMETER_MAX_NUMBER_TOTAL_CALLS, MOB2_PARAMETER_MAX_NUMBER_TOTAL_CALLS_DEFAULT);
	}

	////////
	// Filter clients by total number of comms
	////////

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		uint64_t num_comms_info, num_comms_noinfo, num_comms_nobts;
		num_comms_info = num_comms_noinfo = 0;

		if(inputs[0].num_kvs == 0) // There is not any cdr with cell info --> No output
		{
			return;
		}

		//OLM_T(LMT_User06, ("Before parse: inputs[0].num_kvs:%lu, inputs[1].num_kvs:%lu, inputs[2].num_kvs:%lu", inputs[0].num_kvs, inputs[1].num_kvs, inputs[2].num_kvs));
		node.parse(inputs[0].kvs[0]->key);
		//OLM_T(LMT_User06, ("key:%lu", node.value));

		// Num of communications with bts info
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			input.parse(inputs[0].kvs[i]->value);
			num_comms_info += input.count.value;
		}

		// Num of communications without bts info
		num_comms_noinfo = inputs[1].num_kvs;
		num_comms_nobts = inputs[2].num_kvs;

		// Filter by total num of communications
		if((num_comms_info + num_comms_noinfo + num_comms_nobts) < conf_min_number_total_calls)
		{
			ncomms.value = num_comms_info + num_comms_noinfo + num_comms_nobts;
			//      writer->emit(1, &node,&ncomms);
		}
		else if((num_comms_info + num_comms_noinfo + num_comms_nobts) > conf_max_number_total_calls)
		{
			ncomms.value = num_comms_info + num_comms_noinfo + num_comms_nobts;
			//      writer->emit(2, &node,&ncomms);
		}
		else
		{
			ncomms.value = num_comms_info;
			//ncomms.value = num_comms_info + num_comms_noinfo + num_comms_nobts;
			writer->emit(0, &node,&ncomms);
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
