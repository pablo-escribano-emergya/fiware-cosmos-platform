
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_node_bts_counter
#define _H_SAMSON_mob2_node_bts_counter


#include <samson/module/samson.h>
#include <samson/modules/mob2/Bts_Counter.h>
#include <samson/modules/mob2/Node_Bts.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>



namespace samson{
namespace mob2{


	class node_bts_counter : public samson::Reduce
	{
        //Input
        Node_Bts nodeBts;
        //Outputs
        samson::system::UInt nodeId;
        Bts_Counter count;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: mob2.Node_Bts system.Void  
output: system.UInt mob2.Bts_Counter

extendedHelp: 		Count number of comms by node, bts, day and hour

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

////////
// Count number of comms by node, bts, hour and day
////////

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
        nodeBts.parse(inputs[0].kvs[0]->key);
        nodeId.value = nodeBts.phone.value;
        count.bts.value = nodeBts.bts.value;
        count.wday.value = nodeBts.wday.value;
        count.range.value = nodeBts.range.value;
        count.count.value = inputs[0].num_kvs;
        writer->emit(0, &nodeId,&count);

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
