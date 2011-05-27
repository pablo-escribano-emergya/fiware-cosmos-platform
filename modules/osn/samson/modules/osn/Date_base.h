
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_osn_Date_BASE
#define _H_SAMSON_osn_Date_BASE


#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>


namespace samson{
namespace osn{


	class Date_base : public samson::DataInstance{

	public:
	::samson::system::UInt8 year;
	::samson::system::UInt8 month;
	::samson::system::UInt8 day;
	::samson::system::UInt days_2000;
	::samson::system::UInt8 week_day;

	Date_base() : samson::DataInstance(){
	}

	~Date_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing year
			offset += year.parse(data+offset);
		}

		{ //Parsing month
			offset += month.parse(data+offset);
		}

		{ //Parsing day
			offset += day.parse(data+offset);
		}

		{ //Parsing days_2000
			offset += days_2000.parse(data+offset);
		}

		{ //Parsing week_day
			offset += week_day.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing year
			offset += year.serialize(data+offset);
		}

		{ //Serializing month
			offset += month.serialize(data+offset);
		}

		{ //Serializing day
			offset += day.serialize(data+offset);
		}

		{ //Serializing days_2000
			offset += days_2000.serialize(data+offset);
		}

		{ //Serializing week_day
			offset += week_day.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing year
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing month
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing day
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing days_2000
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing week_day
			offset += ::samson::system::UInt8::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning year
			return year.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing year
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  year compared 
		{ // comparing month
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  month compared 
		{ // comparing day
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  day compared 
		{ // comparing days_2000
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  days_2000 compared 
		{ // comparing week_day
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  week_day compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}







	void copyFrom( Date_base *other ){
		{ //Copying year
			year.copyFrom(&other->year);
		}

		{ //Copying month
			month.copyFrom(&other->month);
		}

		{ //Copying day
			day.copyFrom(&other->day);
		}

		{ //Copying days_2000
			days_2000.copyFrom(&other->days_2000);
		}

		{ //Copying week_day
			week_day.copyFrom(&other->week_day);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting year
			o << year.str();
		}

		o<<" ";
				{ //Texting month
			o << month.str();
		}

		o<<" ";
				{ //Texting day
			o << day.str();
		}

		o<<" ";
				{ //Texting days_2000
			o << days_2000.str();
		}

		o<<" ";
				{ //Texting week_day
			o << week_day.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class Date_base

} // end of namespace samson
} // end of namespace osn

#endif
