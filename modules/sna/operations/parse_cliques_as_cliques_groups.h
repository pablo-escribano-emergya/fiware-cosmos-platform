
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_parse_cliques_as_cliques_groups
#define _H_SAMSON_sna_parse_cliques_as_cliques_groups


#include <samson/module/samson.h>

#include "sna_parsing_macros.h"




namespace samson{
namespace sna{


	class parse_cliques_as_cliques_groups : public samson::Parser
	{

	public:


		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			samson::system::UInt cliqueId;
			samson::system::UInt nodeId;

			size_t offset = 0;
			size_t line_begin = 0;

			while( offset < length )
			{
				if( data[offset] == '\n' || data[offset] == '\0' )
				{
					data[offset] = '\0';

					SNA_PARSER_INIT(  data+line_begin )
					SNA_PARSER_NEXT_FIELD
					SNA_PARSER_GET_UINT( cliqueId.value )

					SNA_PARSER_NEXT_FIELD
					SNA_PARSER_GET_UINT( nodeId.value );

					//rest of the fields not interesting rigth now..
					writer->emit(0, &cliqueId, &nodeId);
					line_begin = offset+1;
				}
				offset++;
			}
			SNA_PARSER_INIT(  data+line_begin )
			SNA_PARSER_NEXT_FIELD
			SNA_PARSER_GET_UINT( cliqueId.value )

			SNA_PARSER_NEXT_FIELD
			SNA_PARSER_GET_UINT( nodeId.value );

			//rest of the fields not interesting rigth now..
			writer->emit(0, &cliqueId, &nodeId);
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif