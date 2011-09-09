
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_cdr_parse_cell_catalogue_position
#define _H_SAMSON_cdr_parse_cell_catalogue_position


#include <samson/module/samson.h>
#include "cdr_parse_fields.h"
#include <locale.h>



namespace samson{
namespace cdr{


class parse_cell_catalogue_position : public samson::Parser
{
	char *oldlocale;

public:
	bool (parse_cell_catalogue_position ::*_gstGetCellInfo)( char *line, CellPos *cell );


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.UInt32 cdr.CellPos

	helpLine: Parse the cell catalogue from Mexico with position info (Mobility 2.0).
	extendedHelp: 		Parse the cell catalogue from Mexico with position info (Mobility 2.0).

#endif // de INFO_COMMENT

	///(TEMM specific)
	///Function to parse a line and convert it into
	///a cell structure; used to
	///parse the cell catalogue.
	///
	///@param line Pointer to the line to be parsed.
	///@param cell Pointer to the cell structure.
	///
	///@return TRUE if successful parsing, FALSE in case
	///   of error.
	bool gstGetCellInfo_MX( char *line, CellPos *cell ){
		uint32_t _cellId;
		size_t _btsId;
		size_t _lacId;
		size_t _stateId;
		float _posX;
		float _posY;
		char *_endptr;
		unsigned int _pos = 0;
		unsigned int _pos_field = 0;

		try{
			// cell id
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrStrCellToInt( line+_pos_field, &_cellId );
			if( _cellId != 0 ){
				cell->cellId.value = _cellId;
			}else{
				OLM_E(("Error parsing cellId at %s", line+_pos_field));
				throw false;
			}

			// BTS id
			cdrGetNextField( line, &_pos, &_pos_field );
			_btsId = (unsigned long)strtoul( line+_pos_field, &_endptr, 10 );
			if( *_endptr == '\0' ){
				cell->bts.value = _btsId;
			}else{
				OLM_E(("Error parsing btsId at %s", line+_pos_field));
				throw false;
			}

			// council id
			cdrGetNextField( line, &_pos, &_pos_field );
			_lacId = (unsigned long)strtoul( line+_pos_field, &_endptr, 10 );
			if( *_endptr == '\0' ){
				cell->mun.value = _lacId;
			}else{
				OLM_E(("Error parsing lacId at %s", line+_pos_field));
				throw false;
			}

			// state id
			cdrGetNextField( line, &_pos, &_pos_field );
			_stateId = (unsigned long)strtoul( line+_pos_field, &_endptr, 10 );
			if( *_endptr == '\0' ){
				cell->sta.value = _stateId;
			}else{
				OLM_E(("Error parsing stateId at %s", line+_pos_field));
				throw false;
			}

			// Position info
			cdrGetNextField( line, &_pos, &_pos_field );
			_posX = (unsigned long)strtof( line+_pos_field, &_endptr );
			if( *_endptr == '\0' ){
				cell->posx.value = _posX;
			}else{
				OLM_E(("Error parsing posX at %s", line+_pos_field));
				throw false;
			}

			cdrGetNextField( line, &_pos, &_pos_field );
			_posY = (unsigned long)strtof( line+_pos_field, &_endptr );
			if( *_endptr == '\0' ){
				cell->posy.value = _posY;
			}else{
				OLM_E(("Error parsing posY at %s", line+_pos_field));
				throw false;
			}
		}catch( ... ){
			OLM_E(("Error parsing line:'%s' for cell", line));
			return false;
		}

		return true;
	}


	void init( samson::KVWriter *writer )
	{
		// To date, only data from Mexico are processed
		_gstGetCellInfo = &parse_cell_catalogue_position::gstGetCellInfo_MX;

		// Cell table from Mexico seems to be in "es" locale ("," as decimal point)
		char *oldlocaletmp;
		if ((oldlocaletmp = setlocale(LC_NUMERIC, "es_ES.UTF-8")) == NULL)
		{
			LM_E(("Error in setlocale"));
			oldlocale = strdup("C");
		}
		else
		{
			oldlocale = strdup(oldlocaletmp);
			setlocale(LC_NUMERIC, "es_ES.UTF-8");
		}

	}

	void parseLines( char *line, samson::KVWriter *writer )
	{
		//Datas to emit
		samson::system::UInt32 cellId;
		CellPos cell;

		//OLM_T(LMT_User06,("Parsing: '%s'\n", line));
		if( (this->*_gstGetCellInfo)( line, &cell ) == true ){
#define MAX_STR_LEN 1024
			//char output[MAX_STR_LEN];
			//ctime_r(&(cdr.time.value), output);
			//output[strlen(output)-1] = '\0';
			//OLM_T(LMT_User06, ("cdr: time: '%s', week_day: %d\n", output, int(cdr.week_day.value)));

			cellId.value = cell.cellId.value;
			writer->emit(0, &cellId, &cell );

		}
		else
		{
			OLM_E(("Error parsing line:'%s' for cell", line));
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
		setlocale(LC_NUMERIC, oldlocale);
		free (oldlocale);
	}



};


} // end of namespace cdr
} // end of namespace samson

#endif
