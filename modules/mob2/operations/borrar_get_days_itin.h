
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_borrar_get_days_itin
#define _H_SAMSON_mob2_borrar_get_days_itin


#include <stdint.h>
#include <samson/module/samson.h>
#include <samson/modules/mob2/Itinerary.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Vector_UInt.h>
#include <samson/modules/system/Void.h>



namespace samson{
namespace mob2{


class borrar_get_days_itin : public samson::Map
{
	//Inputs
	Itinerary itinerary;
	//Outputs
	samson::system::Vector_UInt wdays;
	samson::system::Void mr_void;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob2.Itinerary
	output: system.Vector_UInt system.Void

	extendedHelp: 		Extract the week days when a movement is

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			itinerary.parse(inputs[0].kvs[i]->value);
			if(itinerary.num_moves.value > 1)
			{
				wdays.valuesSetLength(0);
				for(int j=0; j<itinerary.wdays_length; j++)
				{
					wdays.valuesAdd()->copyFrom(&itinerary.wdays[j]);
				}
				writer->emit(0, &wdays,&mr_void);
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
