
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_parse_id_id
#define _H_SAMSON_sna_parse_id_id


#include <samson/module/samson.h>
#include <samson/modules/system/UInt2.h>

#include "sna_parsing_macros.h"



namespace samson{
namespace sna{


	class parse_id_id : public samson::Parser
	{

	public:


		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			samson::system::UInt2 numbers;
			samson::system::Void void_data;

			samson::system::UInt node_a;
			samson::system::UInt node_b;

			size_t offset = 0;
			size_t line_begin = 0;

			while( offset < length )
			{
				if( data[offset] == '\n' || data[offset] == '\0' )
				{
					data[offset] = '\0';



				    SNA_PARSER_INIT( data+line_begin )
				    SNA_PARSER_NEXT_FIELD
				    SNA_PARSER_GET_UINT( node_a )
				    SNA_PARSER_NEXT_FIELD
				    SNA_PARSER_GET_UINT( node_b )

				    numbers.value_1 = node_a;
				    numbers.value_2 = node_b;

					//rest of the fields not interesting right now..
					writer->emit(0, &numbers, &void_data);
					line_begin = offset+1;
				}
				offset++;
			}
		    SNA_PARSER_INIT( data+line_begin )
		    SNA_PARSER_NEXT_FIELD
		    SNA_PARSER_GET_UINT( node_a )
		    SNA_PARSER_NEXT_FIELD
		    SNA_PARSER_GET_UINT( node_b )

		    numbers.value_1 = node_a;
		    numbers.value_2 = node_b;

			//rest of the fields not interesting right now..
			writer->emit(0, &numbers, &void_data);


		}


	};


} // end of namespace samson
} // end of namespace sna

#endif
