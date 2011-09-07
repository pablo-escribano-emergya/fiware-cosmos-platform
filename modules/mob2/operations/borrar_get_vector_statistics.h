
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_borrar_get_vector_statistics
#define _H_SAMSON_mob2_borrar_get_vector_statistics


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Vector_UInt.h>
#include <samson/modules/system/Void.h>



namespace samson{
namespace mob2{


	class borrar_get_vector_statistics : public samson::Reduce
	{
        //Inputs
        samson::system::Vector_UInt vector_uint;
        //Outputs
        samson::system::UInt statistic;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.Vector_samson::system::UInt system.Void  
output: system.Vector_samson::system::UInt system.UInt

extendedHelp: 		Get statistics of a samson::system::UInt vector

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
        vector_uint.parse(inputs[0].kvs[0]->key);
        statistic.value = inputs[0].num_kvs;

        writer->emit(0, &vector_uint,&statistic);

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
