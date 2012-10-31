/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_sna_parse_links_gasset
#define _H_SAMSON_sna_parse_links_gasset


#include <samson/module/samson.h>

#include "sna_parsing_macros.h"


namespace samson{
namespace sna{


class parse_links_gasset : public samson::Parser
{

public:


	void run( char *data , size_t length , samson::KVWriter *writer )
	{

		//Key Value to emit at the output

		//MRData_Link_Edges key_link ;
		Link_Edges       key_link;
		Link       value_link ;

		samson::system::UInt node_a;
		samson::system::UInt node_b;
		double link_strength;
		double link_dir;

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
				SNA_PARSER_NEXT_FIELD
				SNA_PARSER_GET_DOUBLE( link_strength )
				SNA_PARSER_NEXT_FIELD
				SNA_PARSER_GET_DOUBLE( link_dir )

				//Weigth in an integuer format
				int weight = (int) (link_strength * 1000) ;
				int dir = (int) ( link_dir * 1000 );

				if (node_a != node_b)
				{
					// Emit one way
					key_link.phoneA = node_a ;
					key_link.phoneB = node_b ;

					value_link.id = node_b ;
					value_link.weight = weight ;
					value_link.dir = dir ;

					writer->emit(0, &key_link, &value_link);

					// Emit the other way
					key_link.phoneA = node_b ;
					key_link.phoneB = node_a ;

					value_link.id = node_a ;
					value_link.weight = weight ;
					value_link.dir = 1000-dir ;

					writer->emit(0, &key_link, &value_link);
				}
				else
				{
					OLM_E(("Error in parser: Node: %d links itself", node_a.value));
				}
				line_begin = offset+1;
			}
			offset++;
		}
		SNA_PARSER_INIT( data+line_begin )
		SNA_PARSER_NEXT_FIELD
		SNA_PARSER_GET_UINT( node_a )
		SNA_PARSER_NEXT_FIELD
		SNA_PARSER_GET_UINT( node_b )
		SNA_PARSER_NEXT_FIELD
		SNA_PARSER_GET_DOUBLE( link_strength )
		SNA_PARSER_NEXT_FIELD
		SNA_PARSER_GET_DOUBLE( link_dir )

		//Weigth in an integuer format
		int weight = (int) (link_strength * 1000) ;
		int dir = (int) ( link_dir * 1000 );

		if (node_a != node_b)
		{
			// Emit one way
			key_link.phoneA = node_a ;
			key_link.phoneB = node_b ;

			value_link.id = node_b ;
			value_link.weight = weight ;
			value_link.dir = dir ;

			writer->emit(0, &key_link, &value_link);

			// Emit the other way
			key_link.phoneA = node_b ;
			key_link.phoneB = node_a ;

			value_link.id = node_a ;
			value_link.weight = weight ;
			value_link.dir = 1000-dir ;

			writer->emit(0, &key_link, &value_link);
		}
		else
		{
			OLM_E(("Error in parser: Node: %d links itself", node_a.value));
		}
	}


};


} // end of namespace samson
} // end of namespace sna

#endif
