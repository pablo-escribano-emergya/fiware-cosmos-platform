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

#ifndef _H_SAMSON_passive_location_parse_cell_info
#define _H_SAMSON_passive_location_parse_cell_info


#include <samson/module/samson.h>
#include <samson/modules/mobility/Cell.h>
#include <samson/modules/system/UInt32.h>
#include <samson/modules/system/SimpleParser.h>    // SimpleParser

namespace samson{
namespace passive_location{


   class parse_cell_info : public samson::system::SimpleParser
	{

	   samson::passive_location::Cell cell;      // Information about this cell

	   std::vector<char*> words;    // Vector used to store words parsed at each line
	   std::vector<char*> lat_lon;  // Vector used to store lat and lon from a particular field


	public:


// INFO_MODULE 
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)
// output: system.UInt32 passive_location.Cell
// helpLine: Parse input txt file containing cell information( latitude & longitude ). key is cellID composed of ((LAC leftdesp 16) | cellID)
// END_INFO_MODULE

	   void init( samson::KVWriter *writer )
	   {
	   }
	   
	   void parseLine( char * line , samson::KVWriter *writer )
       {
          // Split line in words
          split_in_words( line, words , '|' );
		  
		  if( words.size() < 8 )
		  {
			 LM_W(("Wrong format in line:'%s', %d fields", line, words.size()));
			 return; // Wrong format
		  }
		  
		  if( !strcmp(words[0], "CSR") )
		  {
			 LM_W(("Ignoring CSR in line:'%s', %d fields", line, words.size()));
			 return; // Header
		  }


		  // Extract information

          // Cell id is in field "1"
		  uint32_t cellId = atol( words[1]);
		  // LAC is in field "2"
		  uint32_t LAC = atol(words[2]);

		  // We compose location id with ((LAC << 16) | cell_id), in a uint32_t field
		  cell.cellId.value = (LAC << 16) | cellId;
		  //LM_M(("Composed cellId: From LAC:%d(0x%0x) and cellId:%d(0x%0x) composed:%d(0x%0x)", LAC, LAC, cellId, cellId, cell.cellId.value, cell.cellId.value));
		  if ((cellId == 0) || (LAC == 0))
		  {
		      LM_W(("Parsing:'%s' LAC:%u, cellId:%u, compose:%u", line, LAC, cellId, cell.cellId.value));
		  }

		  if ((!strcmp(words[3], "900")) || (!strcmp(words[3], "2G")))
		  {
		      cell.type.value = 1;
		  }
		  else if ((!strcmp(words[3], "UMTS")) || (!strcmp(words[3], "3G")))
          {
              cell.type.value = 2;
          }
		  else
		  {
		      OLM_E(("Error, unknown network type(%s), not gsm-A nor UMTS\n", words[3]));
		      cell.type.value = 0;
		  }

		  // lat lon is in field "7" in format "latitude,longitude"
		  char* lat_lon_definition = words[7] + 1; // Skip '"'
		  lat_lon_definition[ strlen(lat_lon_definition) - 2 ] = '\0'; // Remove the last '"' 
		 
		  split_in_words( lat_lon_definition, lat_lon , ',' );

		  if( lat_lon.size() != 2 )
		  {
		  LM_W(("Wrong format for LATLONG field ($7): '%s'", lat_lon_definition));
			 return; // Wrong format;
		 }
		  
		  cell.position.latitude.value = atof( lat_lon[0] );
		  cell.position.longitude.value = atof( lat_lon[1] );

          // Emit the cell information
		  writer->emit( 0 , &cell.cellId , &cell );
		  
	   }
	   
	   void finish( samson::KVWriter *writer )
	   {
	   }
	   


	};


} // end of namespace passive_location
} // end of namespace samson

#endif
