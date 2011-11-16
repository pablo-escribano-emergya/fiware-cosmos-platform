
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_convert_cdrs_gst2mob
#define _H_SAMSON_cdr_convert_cdrs_gst2mob


#include <samson/module/samson.h>
#include <samson/modules/system/UInt64.h>


namespace samson{
namespace cdr{


	class convert_cdrs_gst2mob : public samson::Map
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.gstCdr  
output: system.UInt cdr.mobCdr

helpLine: Convert CDRs from Gasset format to Mob format.
extendedHelp: 		Convert CDRs from Gasset format to Mob format.

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			//input
			gstCdr  cdr;
			samson::system::UInt phone;
			//output
			mobCdr  cdr_out;
			samson::system::UInt phone_out;

			for (size_t i=0  ; i< inputs[0].num_kvs ;i++)
			{
				phone.parse(inputs[0].kvs[i]->key);
				cdr.parse(inputs[0].kvs[i]->value);
				//phone_out.value = cdr.phone;
				//cdr_out.phone.value  = cdr.phone;
				phone_out.value = phone.value;
				cdr_out.phone.value = phone.value;
				cdr_out.cellId.value = cdr.cellId.value;

				//cdr_out.btsId = 0;
				//cdr_out.lacId = 0;
				//cdr_out.stateId = 0;

				cdr_out.timeUnix = cdr.timeUnix;

				//cdr_out.absDay = 0;

				cdr_out.mobScope.value = cdr.mobScope.value;

				writer->emit(0, &phone_out, &cdr_out);
			}
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace cdr

#endif
