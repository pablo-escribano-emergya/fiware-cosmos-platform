
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_cdr_parse_mob_cdrs
#define _H_SAMSON_cdr_parse_mob_cdrs


#include <samson/module/samson.h>

#include <samson/modules/system/UInt64.h>
#include "cdr_parse_fields.h"


namespace samson{
namespace cdr{


class parse_mob_cdrs : public samson::Parser
{

public:
	bool (parse_mob_cdrs ::*_mobGetCdrInfo)( char *line, mobCdr *cdr );

#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.UInt64 cdr.mobCdr

	helpLine: Parse CDR files to obtain the mobility imformation associated to each phone.
	extendedHelp: 		Parse CDR files to obtain the mobility imformation associated to each phone.

#endif // de INFO_COMMENT


	bool mobGetCdrInfo_MX( char *line, mobCdr *cdr )
	{
		size_t _phone = 0L;
		unsigned int _cellId = 0;
		size_t _mobScope;
		unsigned int _pos = 0;
		unsigned int _pos_field = 0;

		struct tm timeExpanded;

		try
		{
			// Origin cell id
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrStrCellToInt( line+_pos_field, &_cellId );

			// TEF phone number
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrPhoneToNumber( line+_pos_field, &_phone, CONF_MOB_PHONE_LENGTH );
			if( _phone != 0L )
			{
				cdr->phone.value = _phone;
			}
			else
			{
				throw false;
			}

			// Detination cell (only if origin one is missing)
			cdrGetNextField( line, &_pos, &_pos_field );
			if( _cellId == 0 )
			{
				cdrStrCellToInt( line+_pos_field, &_cellId );
			}
			cdr->cellId.value = _cellId;
			cdr->btsId.value = 0;
			cdr->lacId.value = 0;
			cdr->stateId.value = 0;

			// The other phone number
			cdrGetNextField( line, &_pos, &_pos_field );

			// Direction
			cdrGetNextField( line, &_pos, &_pos_field );

			// Other field
			//cdrGetNextField( line, &_pos, &_pos_field );

			// Date
			cdrGetNextField( line, &_pos, &_pos_field );
			int old_date = 0;
			cdrStrDateToStructure( line+_pos_field, &timeExpanded, old_date );



			// Time
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrStrTimeToStructure( line+_pos_field, &timeExpanded );

			cdr->timeUnix.getTimeUTCFromCalendar(&timeExpanded);

			// Absolute day
			cdr->absDay = 0;

			// Duration
			cdrGetNextField( line, &_pos, &_pos_field );

			// To reach the desired field in original CDR's
			/*
      cdrGetNextField( line, &_pos, &_pos_field );
      cdrGetNextField( line, &_pos, &_pos_field );
      cdrGetNextField( line, &_pos, &_pos_field );
      cdrGetNextField( line, &_pos, &_pos_field );
      cdrGetNextField( line, &_pos, &_pos_field );
      cdrGetNextField( line, &_pos, &_pos_field );
			 */

			// Mobility scope
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrStrScopeToInt_MX( line+_pos_field, &_mobScope );
			cdr->mobScope.value = _mobScope;
		}
		catch( ... )
		{
			return false;
		}

		return true;
	}



	void init( samson::KVWriter *writer )
	{
		// To date, only data from Mexico are processed
		_mobGetCdrInfo = &parse_mob_cdrs::mobGetCdrInfo_MX;
	}

	void parseLines( char *line, samson::KVWriter *writer )
	{
		//Datas to emit
		samson::system::UInt64 phone;
		mobCdr cdr;

		if((this->*_mobGetCdrInfo)(line , &cdr) == true)
		{

#define MAX_STR_LEN 1024
			//char output[MAX_STR_LEN];
			//ctime_r(&(cdr.time.value), output);
			//output[strlen(output)-1] = '\0';
			//OLM_T(LMT_User06, ("cdr: time: '%s', week_day: %d\n", output, int(cdr.week_day.value)));

			phone.value = cdr.phone.value;

			writer->emit(0, &phone, &cdr);

		}
		else
		{
			//OLM_E(("Error parsing line:'%s' for CDR", line));
			;
		}
	}

	void run( char *data , size_t length , samson::KVWriter *writer )
	{
		size_t offset = 0;
		size_t line_begin = 0;

		while( offset < length )
		{

			if( data[offset] == '\n')
			{
				data[offset] = '\0';

				parseLines(data+line_begin , writer);

				line_begin = offset+1;
			}
			++offset;
		}
	}

	void finish( samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace cdr

#endif
