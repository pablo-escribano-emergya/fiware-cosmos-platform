
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_sna_Community_BASE
#define _H_SAMSON_sna_Community_BASE


#include <samson/modules/sna/Clique.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace sna{


	class Community_base : public samson::DataInstance{

	public:
	::samson::system::UInt id_comm;
	::samson::sna::Clique nodes;
	::samson::sna::Clique orphan;

	Community_base() : samson::DataInstance(){
	}

	~Community_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing id_comm
			offset += id_comm.parse(data+offset);
		}

		{ //Parsing nodes
			offset += nodes.parse(data+offset);
		}

		{ //Parsing orphan
			offset += orphan.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing id_comm
			offset += id_comm.serialize(data+offset);
		}

		{ //Serializing nodes
			offset += nodes.serialize(data+offset);
		}

		{ //Serializing orphan
			offset += orphan.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing id_comm
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing nodes
			offset += ::samson::sna::Clique::size(data+offset);
		}

		{ //Sizing orphan
			offset += ::samson::sna::Clique::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning id_comm
			return id_comm.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing id_comm
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  id_comm compared 
		{ // comparing nodes
			int tmp = ::samson::sna::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  nodes compared 
		{ // comparing orphan
			int tmp = ::samson::sna::Clique::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  orphan compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}





	void copyFrom( Community_base *other ){
		{ //Copying id_comm
			id_comm.copyFrom(&other->id_comm);
		}

		{ //Copying nodes
			nodes.copyFrom(&other->nodes);
		}

		{ //Copying orphan
			orphan.copyFrom(&other->orphan);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting id_comm
			o << id_comm.str();
		}

		o<<" ";
				{ //Texting nodes
			o << nodes.str();
		}

		o<<" ";
				{ //Texting orphan
			o << orphan.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class Community_base

} // end of namespace samson
} // end of namespace sna

#endif
