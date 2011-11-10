
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob_map_get_month
#define _H_SAMSON_mob_map_get_month


#include <samson/module/samson.h>
#include <samson/modules/system/Date.h>


namespace samson{
namespace mob{


class map_get_month : public samson::Map
{

	// Input[0k]
	samson::system::Date date;
	// // Output [0kv]
	samson::system::Void void_data;

	// In MACRO, global variable. Used only to compute the coveredDays by the TimeSlot,
	// but this information is not used (now) in mobility module
	// Could be put used from the dataset, but we will have some problems with
	// the ComplexTimeSlot setting (now it is done at init(), so no access to datasets.
	samson::system::Date mob_month;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.Date system.Void
	output: system.Void system.Void

	helpLine: Gets month being processed.
	extendedHelp: 		Gets month being processed.

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
		mob_month.month.value = 0;
	}


	// Goyo. Parece que no se usa, lo cual no sorprende
	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{


		for (size_t i = 0 ; i < inputs[0].num_kvs ; ++i)
		{
			//Parsing key and value
			date.parse( inputs[0].kvs[i]->key );

			if (mob_month.month.value == 0)
			{
				mob_month.copyFrom (&date);
			}

			writer->emit(0, &void_data, &void_data);
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace mob

#endif