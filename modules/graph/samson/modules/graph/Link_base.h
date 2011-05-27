
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_graph_Link_BASE
#define _H_SAMSON_graph_Link_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace graph{


	class Link_base : public samson::DataInstance{

	public:
	::samson::system::UInt id;
	::samson::system::UInt weight;

	Link_base() : samson::DataInstance(){
	}

	~Link_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing id
			offset += id.parse(data+offset);
		}

		{ //Parsing weight
			offset += weight.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing id
			offset += id.serialize(data+offset);
		}

		{ //Serializing weight
			offset += weight.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing id
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing weight
			offset += ::samson::system::UInt::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning id
			return id.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing id
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  id compared 
		{ // comparing weight
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  weight compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}




	void copyFrom( Link_base *other ){
		{ //Copying id
			id.copyFrom(&other->id);
		}

		{ //Copying weight
			weight.copyFrom(&other->weight);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting id
			o << id.str();
		}

		o<<" ";
				{ //Texting weight
			o << weight.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class Link_base

} // end of namespace samson
} // end of namespace graph

#endif
