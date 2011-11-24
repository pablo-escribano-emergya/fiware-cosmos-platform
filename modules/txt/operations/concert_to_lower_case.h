
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_concert_to_lower_case
#define _H_SAMSON_txt_concert_to_lower_case


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace txt{


	class concert_to_lower_case : public samson::Map
	{


	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.String system.UInt  
//  output: system.String system.UInt
//  
// helpLine: Convert to lower case. Also filtere words with non-printable chars
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		   samson::system::String word;
		   samson::system::UInt count;

           for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
           {
			  
              word.parse( inputs[0].kvs[i]->key );
              count.parse( inputs[0].kvs[i]->value );
			  
              bool emit_word = true;
			  
              for ( size_t c = 0 ; c < word.value.length() ; c++ )
              {
                 char a = word.value[c];

				 if( a < 32 )
				 {
					emit_word= false;
					break;
				 }

				 if( a == 127 )
				 {
					emit_word= false;
					break;
				 }

				 // Transform
				 if( (a>=65) && (a<=90))
					word.value[c] = a +32;


              }
		   
			  if( emit_word )
				 writer->emit( 0 , &word , &count );
		   }
		}
		
		void finish( samson::KVWriter *writer )
		{
		}
		
		
		
	};
   

} // end of namespace txt
} // end of namespace samson

#endif
