
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_word_count2
#define _H_SAMSON_txt_word_count2


#include <samson/module/samson.h>

#include "samson/modules/system/String.h"
#include "samson/modules/system/UInt.h"

namespace samson{
namespace txt{


	class word_count2 : public samson::Reduce
	{

	public:
		
		samson::system::String word;
		samson::system::UInt count;
		samson::system::UInt total;


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
//		   printf("Running word_count2 with %lu and %lu\n" , inputs[0].num_kvs , inputs[1].num_kvs );
			
			if( inputs[0].num_kvs > 0 )
			  word.parse( inputs[0].kvs[0]->key );
			else
			  word.parse( inputs[1].kvs[0]->key );
			
//			printf("Word %s\n", word.value.c_str());

			total.value = 0;

			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				count.parse( inputs[0].kvs[i]->value );
				total.value += count.value;

//				printf("Count %lu -> %lu\n" , count.value , total.value );
			}

			for (size_t i = 0 ; i < inputs[1].num_kvs ; i++)
			{


				count.parse( inputs[1].kvs[i]->value );
				total.value += count.value;

//				printf("Count %lu -> %lu\n" , count.value , total.value );
			}



			// Emit the result			
//			printf("Emit %s %lu\n" , word.value.c_str() , total.value);
			writer->emit( 0, &word , &total );

//			printf("Finish Running word_count2 with %lu and %lu\n" , inputs[0].num_kvs , inputs[1].num_kvs );
		}

	};


} // end of namespace samson
} // end of namespace system

#endif
