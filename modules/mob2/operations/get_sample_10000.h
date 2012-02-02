
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_get_sample_10000
#define _H_SAMSON_mob2_get_sample_10000


#include <stdint.h>
#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


class get_sample_10000 : public samson::Map
{
	//Inputs
	samson::system::UInt cellId;
	samson::cdr::mobCdr cdr;
	//Outputs
	samson::system::UInt nodeId;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt cdr.mobCdr
	output: system.UInt cdr.mobCdr

	extendedHelp: 		Extract a sample of around 10000 clients

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			cellId.parse(inputs[0].kvs[i]->key);
			if(cellId.value == 0)
			{
				continue;
			}
			cdr.parse(inputs[0].kvs[i]->value);
			int nod = (cdr.phone.value % 100000)/100;
			if(nod == 925)
			{
				nodeId.value = cdr.phone.value;
				writer->emit(0, &nodeId,&cdr);
			}
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif