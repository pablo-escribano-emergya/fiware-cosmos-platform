
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_OTTstream_parse_user_cluster
#define _H_SAMSON_OTTstream_parse_user_cluster


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/SimpleParser.h>


namespace samson{
namespace OTTstream{


	class parse_user_cluster : public samson::system::SimpleParser
	{

	    std::vector<char*> fields;
	    char sep;

	    samson::system::UInt userId;
	    samson::system::UInt clusterId;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  output: system.UInt system.UInt
//  
// helpLine: Parse user classification by OTT clusters
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		    sep = '|';
		}

		void parseLine( char * line, samson::KVWriter *writer )
		{
		    char *endptr = NULL;

		    split_in_words(line, fields, sep);
		    if (fields.size() != 2)
		    {
		        return;
		    }

		    userId.value = strtol(fields[0], &endptr, 10);
		    clusterId.value = strtol(fields[1], &endptr, 10);

		    writer->emit(0, &userId, &clusterId);
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace OTTstream
} // end of namespace samson

#endif