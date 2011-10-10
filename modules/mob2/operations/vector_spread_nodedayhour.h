
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_vector_spread_nodedayhour
#define _H_SAMSON_mob2_vector_spread_nodedayhour


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/mob2/Node_Bts.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>
#include <samson/modules/system/DateComplete.h>
#include <samson/modules/system/Time.h>




namespace samson{
namespace mob2{


class vector_spread_nodedayhour : public samson::Map
{
	//Inputs
	samson::system::UInt node;
	samson::cdr::mobCdr cdr;
	//Outputs
	Node_Bts noddayhour;
	samson::system::Void mrvoid;
	// Local variables to handle weekday
	samson::system::DateComplete lDate;
	samson::system::Time lTime;


public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt cdr.mobCdr
	output: mob2.Node_Bts system.Void

	extendedHelp: 		Spread cdrs by node, day and hour

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			node.parse(inputs[0].kvs[i]->key);
			cdr.parse(inputs[0].kvs[i]->value);
			noddayhour.phone.value = node.value;
			noddayhour.bts.value = 0;
			cdr.timeUnix.getDateTimeFromTimeUTC(&lDate, &lTime);
			int date = lDate.week_day.value;
			// Group 0: MON-TUE, Group 1: FRI, Group 2: SAT, Group 3: SUN
#define SUNDAY_INDEX 0
#define SUNDAY_GROUP 3
#define SATURDAY_INDEX 6
#define SATURDAY_GROUP 2
#define FRIDAY_INDEX 5
#define FRIDAY_GROUP 1
#define WEEKDAY_GROUP 0
			if(date == SUNDAY_INDEX) 	   { noddayhour.wday.value = SUNDAY_GROUP;}
			else if(date == FRIDAY_INDEX) { noddayhour.wday.value = FRIDAY_GROUP;}
			else if(date == SATURDAY_INDEX) { noddayhour.wday.value = SATURDAY_GROUP;}
			else    	   { noddayhour.wday.value = WEEKDAY_GROUP;}
			noddayhour.range.value = lTime.hour.value;
			writer->emit(0, &noddayhour,&mrvoid);
		}
	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
