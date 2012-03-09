
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_system_parse_to_value
#define _H_SAMSON_system_parse_to_value


#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>

#include "au/StringComponents.h"

#include "Filter.h"


namespace samson{
namespace system{


	class parse_to_value : public samson::Parser
	{

	   samson::system::Value key;
	   samson::system::Value value;
        
        // Collection of filters to execute for every key-value
        FilterCollection filters_collection;

        // Internal buffer to store line
        char line[1024];

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  output: system.Value system.Value
//  
// helpLine: Parse lines with fields separated by a single char tab , space , coma , ...
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
            // By default, value is 1
            value = 1;
            
            // Setup the process chain...
            std::string command =  environment->get( "command" ,  "" );
            au::ErrorManager error;
            filters_collection.addFilters( command , writer , &error );
            
            // Error defined by user
            if( error.isActivated() )
                tracer->setUserError( error.getMessage() );

		}

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
            
            size_t offset = 0;
            while( offset < length )
            {
                // Parse a line
                size_t line_pos = 0;
                while( ( offset < length ) && data[offset] != '\n' )
                {
                    line[line_pos++] = data[offset];
                    offset++;
                }
                offset++; // Skip the '\n'
                line[line_pos] = '\0';

                // Set the key with the new string
                key.set_string( line );

                
                if( filters_collection.filters.size() > 0 )
                {
                    KeyValue kv( &key, &value );
                    for( size_t f = 0 ; f < filters_collection.filters.size() ; f++ )
                        filters_collection.filters[f]->run(kv);
                }
                else
                {                   
                    //Emit the parsed key value
                    writer->emit( 0 , &key , &value );
                }
            }
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace system
} // end of namespace samson

#endif
