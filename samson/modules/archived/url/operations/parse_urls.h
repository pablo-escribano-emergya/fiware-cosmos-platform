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

#ifndef _H_SAMSON_url_parse_urls
#define _H_SAMSON_url_parse_urls


#include <samson/module/samson.h>
#include <samson/modules/system/SimpleParser.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/Void.h>

namespace samson{
namespace url{
  

    class parse_urls : public samson::system::SimpleParser
   {

	  typedef enum
	  {
	    server,           // Default
	    url               // Full url
	  } OutputParameter;

        // Output server name or full url depending on parameter url.output="server" (default) url.output="url"

	samson::system::String key;            // Output key
	samson::system::Void void_value;      // Empty output for value

	OutputParameter outputParameter;  // Type of output

	public:


	void init(samson::KVWriter *writer)
	{
	  std::string output  = environment->get( "url.output" , "server" );

	  if( output == "url")
	    outputParameter = url;
	  else
	    outputParameter = server;

	}

    void parseLine( char * _line , samson::KVWriter *writer )
	{
	  std::string line = _line;

	  // Main division of the line
	  // --------------------------------------------------------------------

	  size_t pos_division   = line.find(" ");
	  size_t pos_division_2 = line.find(" ", pos_division );

          if( pos_division == std::string::npos )
            return; // Not valid division ( url and user identifier )
          if( pos_division_2 == std::string::npos )
            return; // Not valid division ( url and user identifier )

	  std::string str_url         = line.substr(0,pos_division);
	  // std::string str_time_stamp  = line.substr(pos_division,pos_division_2 - pos_division );
	  //std::string str_user        = line.substr(pos_division_2, line.length() - pos_division_2 );


	  if (outputParameter == server )
	  {
	    // Strip the url to get the server
	    // --------------------------------------------------------------------

	    size_t pos1 = str_url.find("://");

	    if( pos1 == std::string::npos )
	      return; // Not valid URL

	    pos1 +=3; // To point at the begin of the server

	    size_t pos2 = str_url.find("/", pos1+3 );

	    if( pos2 == std::string::npos )
	      key.value = str_url.substr( pos1 , str_url.length() - pos1 );
	    else
	      key.value = str_url.substr( pos1 , pos2 - pos1 );
	  }
	  else if ( outputParameter == url)
	  {
	    key.value = str_url;
	  }

	  // Parse time-stamp
	  // --------------------------------------------------------------------

	  //time_stamp.value = strtoull( str_time_stamp.c_str() , NULL , 10 );

	  // Emit the key-value at the output
	  // --------------------------------------------------------------------

	  if( key.value.length() > 0 )
	  {
	    //OLM_M(("Server: '%s time: %u'",server.value.c_str() , time_stamp.value));
	    writer->emit( 0 , &key , &void_value );
	  }

	}

	};


} // end of namespace samson
} // end of namespace url

#endif
