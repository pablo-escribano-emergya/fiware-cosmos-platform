
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_level_update_int32_levels
#define _H_SAMSON_level_update_int32_levels


#include <samson/module/samson.h>
#include <samson/modules/system/Int32.h>
#include <samson/modules/system/String.h>


namespace samson{
namespace level{


	class update_int32_levels : public samson::Reduce
	{

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.String system.Int32  
//  input: system.String system.Int32  
//  output: system.String system.Int32
//  output: system.String system.Int32
//  
// helpLine: Update Level with incomming values. Accumulated value is emitted at two outputs
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		   samson::system::String key;
		   samson::system::Int32 tmp;
		   samson::system::Int32 value;

		   if ( inputs[0].num_kvs > 0 )
		   {
		     LM_M(("Parsing key from hits"));
			  key.parse( inputs[0].kvs[0]->key );
		   }
		   else
		   {
		     LM_M(("Parsing key from state"));
			  key.parse( inputs[1].kvs[0]->key );
		   }

		   // Init value
		   value.value = 0;
		   
		   LM_M(("Parsing values from %lu hits", inputs[0].num_kvs));
		   for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		   {
			  tmp.parse( inputs[0].kvs[i]->value );
			  value.value += tmp.value;
		   }

		   LM_M(("Parsing values from %lu states", inputs[1].num_kvs));
		   for ( size_t i = 0 ; i < inputs[1].num_kvs ; i++ )
		   {
			  tmp.parse( inputs[1].kvs[i]->value );
			  value.value += tmp.value;
		   }


		   LM_M(("Emit key:%s, value:%d with %lu num_kvs", key.value.c_str(), value.value, inputs[0].num_kvs));
		   writer->emit( 0 , &key , &value );
		   writer->emit( 1 , &key , &value );


		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace level
} // end of namespace samson

#endif
