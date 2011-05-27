
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_url_ServerPath_BASE
#define _H_SAMSON_url_ServerPath_BASE


#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace url{


	class ServerPath_base : public samson::DataInstance{

	public:
	::samson::system::UInt server;
	::samson::system::String path;

	ServerPath_base() : samson::DataInstance(){
	}

	~ServerPath_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing server
			offset += server.parse(data+offset);
		}

		{ //Parsing path
			offset += path.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing server
			offset += server.serialize(data+offset);
		}

		{ //Serializing path
			offset += path.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing server
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing path
			offset += ::samson::system::String::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning server
			return server.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing server
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  server compared 
		{ // comparing path
			int tmp = ::samson::system::String::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  path compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}




	void copyFrom( ServerPath_base *other ){
		{ //Copying server
			server.copyFrom(&other->server);
		}

		{ //Copying path
			path.copyFrom(&other->path);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting server
			o << server.str();
		}

		o<<" ";
				{ //Texting path
			o << path.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class ServerPath_base

} // end of namespace samson
} // end of namespace url

#endif
