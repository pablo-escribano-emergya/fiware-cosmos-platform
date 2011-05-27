
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_cdr_User_BASE
#define _H_SAMSON_cdr_User_BASE


#include <samson/modules/cdr/Date.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>


namespace samson{
namespace cdr{


	class User_base : public samson::DataInstance{

	public:
	::samson::system::UInt id;
	::samson::system::UInt titularId;
	::samson::system::UInt8 type;
	::samson::cdr::Date activationDate;
	::samson::system::UInt8 activationCode;
	::samson::system::UInt8 age;
	::samson::system::UInt8 province;
	::samson::system::UInt8 sex;
	::samson::cdr::Date churnDate;
	::samson::system::UInt8 churnCode;

	User_base() : samson::DataInstance(){
	}

	~User_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing id
			offset += id.parse(data+offset);
		}

		{ //Parsing titularId
			offset += titularId.parse(data+offset);
		}

		{ //Parsing type
			offset += type.parse(data+offset);
		}

		{ //Parsing activationDate
			offset += activationDate.parse(data+offset);
		}

		{ //Parsing activationCode
			offset += activationCode.parse(data+offset);
		}

		{ //Parsing age
			offset += age.parse(data+offset);
		}

		{ //Parsing province
			offset += province.parse(data+offset);
		}

		{ //Parsing sex
			offset += sex.parse(data+offset);
		}

		{ //Parsing churnDate
			offset += churnDate.parse(data+offset);
		}

		{ //Parsing churnCode
			offset += churnCode.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing id
			offset += id.serialize(data+offset);
		}

		{ //Serializing titularId
			offset += titularId.serialize(data+offset);
		}

		{ //Serializing type
			offset += type.serialize(data+offset);
		}

		{ //Serializing activationDate
			offset += activationDate.serialize(data+offset);
		}

		{ //Serializing activationCode
			offset += activationCode.serialize(data+offset);
		}

		{ //Serializing age
			offset += age.serialize(data+offset);
		}

		{ //Serializing province
			offset += province.serialize(data+offset);
		}

		{ //Serializing sex
			offset += sex.serialize(data+offset);
		}

		{ //Serializing churnDate
			offset += churnDate.serialize(data+offset);
		}

		{ //Serializing churnCode
			offset += churnCode.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing id
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing titularId
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing type
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing activationDate
			offset += ::samson::cdr::Date::size(data+offset);
		}

		{ //Sizing activationCode
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing age
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing province
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing sex
			offset += ::samson::system::UInt8::size(data+offset);
		}

		{ //Sizing churnDate
			offset += ::samson::cdr::Date::size(data+offset);
		}

		{ //Sizing churnCode
			offset += ::samson::system::UInt8::size(data+offset);
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
		{ // comparing titularId
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  titularId compared 
		{ // comparing type
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  type compared 
		{ // comparing activationDate
			int tmp = ::samson::cdr::Date::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  activationDate compared 
		{ // comparing activationCode
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  activationCode compared 
		{ // comparing age
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  age compared 
		{ // comparing province
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  province compared 
		{ // comparing sex
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  sex compared 
		{ // comparing churnDate
			int tmp = ::samson::cdr::Date::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  churnDate compared 
		{ // comparing churnCode
			int tmp = ::samson::system::UInt8::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  churnCode compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}












	void copyFrom( User_base *other ){
		{ //Copying id
			id.copyFrom(&other->id);
		}

		{ //Copying titularId
			titularId.copyFrom(&other->titularId);
		}

		{ //Copying type
			type.copyFrom(&other->type);
		}

		{ //Copying activationDate
			activationDate.copyFrom(&other->activationDate);
		}

		{ //Copying activationCode
			activationCode.copyFrom(&other->activationCode);
		}

		{ //Copying age
			age.copyFrom(&other->age);
		}

		{ //Copying province
			province.copyFrom(&other->province);
		}

		{ //Copying sex
			sex.copyFrom(&other->sex);
		}

		{ //Copying churnDate
			churnDate.copyFrom(&other->churnDate);
		}

		{ //Copying churnCode
			churnCode.copyFrom(&other->churnCode);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting id
			o << id.str();
		}

		o<<" ";
				{ //Texting titularId
			o << titularId.str();
		}

		o<<" ";
				{ //Texting type
			o << type.str();
		}

		o<<" ";
				{ //Texting activationDate
			o << activationDate.str();
		}

		o<<" ";
				{ //Texting activationCode
			o << activationCode.str();
		}

		o<<" ";
				{ //Texting age
			o << age.str();
		}

		o<<" ";
				{ //Texting province
			o << province.str();
		}

		o<<" ";
				{ //Texting sex
			o << sex.str();
		}

		o<<" ";
				{ //Texting churnDate
			o << churnDate.str();
		}

		o<<" ";
				{ //Texting churnCode
			o << churnCode.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class User_base

} // end of namespace samson
} // end of namespace cdr

#endif
