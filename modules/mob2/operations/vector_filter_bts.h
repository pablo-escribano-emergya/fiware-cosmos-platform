
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_vector_filter_bts
#define _H_SAMSON_mob2_vector_filter_bts


#include <samson/module/samson.h>
#include <samson/modules/mob2/Bts_Counter.h>
#include <samson/modules/mob2/TwoInt.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>



namespace samson{
namespace mob2{


	class vector_filter_bts : public samson::Reduce
	{
        //Inputs
        samson::system::UInt bts;
        TwoInt group_hour;       //num1: group   Num2: hour
        // Outputs
        Bts_Counter counter;
        samson::system::Void mrvoid;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt mob2.TwoInt  
output: mob2.Bts_Counter system.Void

extendedHelp: 		Filter bts by num of comms total and max area

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

///////////////////////////
// VECTOR COMMS IN A BTS //
///////////////////////////

// Filter bts by ncomms and max area

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
        for(uint64_t i=0; i<inputs[0].num_kvs; i++)
        {
                bts.parse(inputs[0].kvs[i]->key);
                group_hour.parse(inputs[0].kvs[i]->value);
                counter.bts.value = bts.value;
                counter.wday.value = group_hour.num1.value;
                counter.range.value = group_hour.num2.value;
                counter.count.value = 0;
                writer->emit(0, &counter,&mrvoid);
        }

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
