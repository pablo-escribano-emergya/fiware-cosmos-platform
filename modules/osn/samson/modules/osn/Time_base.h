
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_osn_Time_BASE
#define _H_SAMSON_osn_Time_BASE


#include <samson/modules/system/UInt8.h>


namespace samson{
namespace osn{


	class Time_base : public samson::DataInstance{

	public:
	::samson::system::UInt8 hour;
	::samson::system::UInt8 minute;
	::samson::system::UInt8 seconds;

	Time_base() : samson::DataInstance(){
	}

	~Time_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing hour
			offset += hour.parse(data+offset);
		}

		{ //Parsing minute
			offset += minute.parse(data+offset);
		}

		{ //Parsing seconds
			offset += seconds.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing hour
			offset += hour.serialize(data+offset);
		}

		{ //Serializing minute
			offset += minute.serialize(data+offset);
		}

		{ //Serializing seconds
			offset += seconds.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing hour
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing minute
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing seconds
			offset += ::samson::system::UInt8::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning hour
			return hour.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing hour
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  hour compared 
		{ // comparing minute
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  minute compared 
		{ // comparing seconds
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  seconds compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}





	void copyFrom( Time_base *other ){
		{ //Copying hour
			hour.copyFrom(&other->hour);
		}

		{ //Copying minute
			minute.copyFrom(&other->minute);
		}

		{ //Copying seconds
			seconds.copyFrom(&other->seconds);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting hour
			o << hour.str();
		}

		o<<" ";
				{ //Texting minute
			o << minute.str();
		}

		o<<" ";
				{ //Texting seconds
			o << seconds.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class Time_base

} // end of namespace samson
} // end of namespace osn

#endif
