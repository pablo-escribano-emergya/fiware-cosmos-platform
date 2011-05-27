
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_tienda_Operation_BASE
#define _H_SAMSON_tienda_Operation_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace tienda{


	class Operation_base : public samson::DataInstance{

	public:
	::samson::system::UInt user;
	::samson::system::UInt product;
	::samson::system::UInt operation;

	Operation_base() : samson::DataInstance(){
	}

	~Operation_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing user
			offset += user.parse(data+offset);
		}

		{ //Parsing product
			offset += product.parse(data+offset);
		}

		{ //Parsing operation
			offset += operation.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing user
			offset += user.serialize(data+offset);
		}

		{ //Serializing product
			offset += product.serialize(data+offset);
		}

		{ //Serializing operation
			offset += operation.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing user
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing product
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing operation
			offset += ::samson::system::UInt::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning user
			return user.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing user
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  user compared 
		{ // comparing product
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  product compared 
		{ // comparing operation
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  operation compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}





	void copyFrom( Operation_base *other ){
		{ //Copying user
			user.copyFrom(&other->user);
		}

		{ //Copying product
			product.copyFrom(&other->product);
		}

		{ //Copying operation
			operation.copyFrom(&other->operation);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting user
			o << user.str();
		}

		o<<" ";
				{ //Texting product
			o << product.str();
		}

		o<<" ";
				{ //Texting operation
			o << operation.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class Operation_base

} // end of namespace samson
} // end of namespace tienda

#endif
