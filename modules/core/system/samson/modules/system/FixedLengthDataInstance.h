#ifndef SAMSON_FIXED_LENGTH_DATA_INSTANCE_H
#define SAMSON_FIXED_LENGTH_DATA_INSTANCE_H
#include <string>                /* std::string                              */
#include <sstream>               /* std::ostringstream   */
#include <samson/module/var_int.h>
#include <samson/module/DataInstance.h>

namespace samson {

namespace system {

template <typename T>
class FixedLengthDataInstance : public samson::DataInstance
{
public:
    T value;

public:
    FixedLengthDataInstance() : samson::DataInstance(){
    }

    ~FixedLengthDataInstance() {
    }

    int parse(char *data){
        value = *((T*)data);
        return sizeof(T);
    }

    int serialize(char *data){
        *((T*)data) = value;
        return sizeof(T);
    }

    static int size(char *data){
        return sizeof(T);
    }

    inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){

        T _value1,_value2;

        _value1 = *((T*)(data1+*offset1));
        _value2 = *((T*)(data2+*offset2));

        *offset1 += sizeof(T);
        *offset2 += sizeof(T);

        if( _value1 < _value2 ) return -1;
        if( _value1 > _value2 ) return  1;
        return 0;
    }

    inline static int compare( char* data1 , char* data2 )
    {
        size_t offset_1=0;
        size_t offset_2=0;
        return compare( data1 , data2 , &offset_1 , &offset_2 );
    }

    int serial_compare( char* data1 , char* data2 )
    {
        return compare( data1, data2 );
    }

    void copyFrom( FixedLengthDataInstance<T> *other ){
        value = other->value;
    };

    std::string str(){
        std::ostringstream o;
        o << value;
        return o.str();
    }

    std::string strJSON(std::string _varNameInternal){
        std::ostringstream o;
        o << "{" << "\"" << _varNameInternal << "\":" << value << "}";
        return o.str();
    }

    std::string strJSONInternal(std::string _varNameInternal, bool vectorMember){
        std::ostringstream o;
        if (vectorMember)
        {
            o << value;
        }
        else
        {
            o << "\"" << _varNameInternal << "\":" << value;
        }
        return o.str();
    }

    std::string strXML(std::string _varNameInternal){
        std::ostringstream o;
        o << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        o << strXMLInternal(_varNameInternal);
        return o.str();
    }

    std::string strXMLInternal(std::string _varNameInternal){
        std::ostringstream o;
        o << "<" << _varNameInternal << ">" << value << "</" << _varNameInternal << ">\n";
        return o.str();
    }

};


}// system namespace
} // ss namespace

#endif