
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_system_Vector_UInt_BASE
#define _H_SAMSON_system_Vector_UInt_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace system{


	class Vector_UInt_base : public samson::DataInstance{

	public:
	::samson::system::UInt *values;
	int values_length;
	int values_max_length;

	Vector_UInt_base() : samson::DataInstance(){
		values_length=0;
		values_max_length=0;
		values = NULL;
	}

	~Vector_UInt_base() {
		if( values )
			delete[] values ;
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing vector values
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
		 	valuesSetLength( _length );
			for (int i = 0 ; i < (int)values_length ; i++)
			{ //Parsing values
				offset += values[i].parse(data+offset);
			}

		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serialization vector values
			offset += samson::staticVarIntSerialize( data+offset , values_length );
			for (int i = 0 ; i < (int)values_length ; i++)
			{ //Serializing values
				offset += values[i].serialize(data+offset);
			}

		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Getting size of vector values
			size_t _length;
			offset += samson::staticVarIntParse( data+offset , &_length );
			::samson::system::UInt _tmp;
			for (int i = 0 ; i < (int)_length ; i++)
			{ //Sizing values
				offset += ::samson::system::UInt::size(data+offset);
			}

		}

		return offset;
	}

	int hash(int max_num_partitions){
		if( values_length > 0 )
			{ //Partitioning values
				return values[0].hash(max_num_partitions);
			}

		else return 0;

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // Comparing vector values
			size_t _length1,_length2;
			*offset1 += samson::staticVarIntParse( data1+(*offset1) , &_length1 );
			*offset2 += samson::staticVarIntParse( data2+(*offset2) , &_length2 );
			if( _length1 < _length2 ) return -1;
			if( _length1 > _length2 ) return 1;
			for (int i = 0 ; i < (int)_length1 ; i++)
			{ // comparing values[i]
				int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
				if( tmp != 0) return tmp;
			}   //  values[i] compared 
		}   // vector values compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}



	void valuesSetLength(int _length){
		if( _length > values_max_length){ 
			::samson::system::UInt *_previous = values;
			int previous_length = values_length;
			if(values_max_length == 0) values_max_length = _length;
			while(values_max_length < _length) values_max_length *= 2;
			values = new ::samson::system::UInt[values_max_length ];
			if( _previous ){
				for (int i = 0 ; i < previous_length ; i++)
					values[i].copyFrom( &_previous[i] );
				delete[] _previous;
			}
		}
		values_length=_length;
	}

	::samson::system::UInt* valuesAdd(){
		valuesSetLength( values_length + 1 );
		return &values[values_length-1];
	}

	void copyFrom( Vector_UInt_base *other ){
		{ // CopyFrom field values
			valuesSetLength( other->values_length);
			for (int i = 0 ; i < values_length ; i++)
			{ //Copying values
				values[i].copyFrom(&other->values[i]);
			}
		}

	};

	std::string str(){
		std::ostringstream o;
		{// toString of vector values
			for(int i = 0 ; i < values_length ; i++)
			{ //Texting values
				o << values[i].str();
			}
				 o << " ";
		}

		o<<" ";
		return o.str();
	}

	}; //class Vector_UInt_base

} // end of namespace samson
} // end of namespace system

#endif
