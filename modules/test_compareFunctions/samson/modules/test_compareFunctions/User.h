
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_test_compareFunctions_User
#define _H_SAMSON_test_compareFunctions_User


#include <samson/modules/test_compareFunctions/User_base.h>

//#define DEBUG_FILES
#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES



namespace samson{
namespace test_compareFunctions{


	class User : public User_base
	{

	public:
	inline static int compare_by_company(char *data1, char *data2)
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare_by_company( data1 , data2 , &offset_1 , &offset_2 );
	}

	inline static int compare_by_company (char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
	{
                ::samson::system::UInt8 local__filledOptFields__1;
                (*offset1) += local__filledOptFields__1.parse(data1+*offset1);
                ::samson::system::UInt8 local__filledOptFields__2;
                (*offset2) += local__filledOptFields__2.parse(data2+*offset2);
                { // comparing id
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  id compared
                { // comparing surname
			*offset1 += ::samson::system::String::size(data1+*offset1);
			*offset2 += ::samson::system::String::size(data2+*offset2);
                }   //  surname compared
                { // comparing companyId
                        int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
                        if( tmp != 0) return tmp;
                }   //  companyId compared
                return 0; //If everything is equal
	}

	inline static int compare_by_age(char *data1, char *data2)
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare_by_age( data1 , data2 , &offset_1 , &offset_2 );
	}

	inline static int compare_by_age (char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
	{
                ::samson::system::UInt8 local__filledOptFields__1;
                (*offset1) += local__filledOptFields__1.parse(data1+*offset1);
                ::samson::system::UInt8 local__filledOptFields__2;
                (*offset2) += local__filledOptFields__2.parse(data2+*offset2);
                { // comparing id
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  id compared
                { // comparing surname
			*offset1 += ::samson::system::String::size(data1+*offset1);
			*offset2 += ::samson::system::String::size(data2+*offset2);
                }   //  surname compared
                { // comparing companyId
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  companyId compared
                { // comparing age
                        if (((local__filledOptFields__1.value & 0x1) != 0) && ((local__filledOptFields__2.value & 0x1) == 0))
                        {
//#define DEBUG_FILES
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/User_compare.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "Compare: age1 set and age2 don't" << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
                                return 1;
                        }
                        else if (((local__filledOptFields__1.value & 0x1) == 0) && ((local__filledOptFields__2.value & 0x1) != 0))
                        {
//#define DEBUG_FILES
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/User_compare.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "Compare: age1 don't and age2 set" << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
                                return -1;
                        }
                        else if ((local__filledOptFields__1.value & 0x1) && (local__filledOptFields__2.value & 0x1))
                        {
                                int tmp = ::samson::system::UInt8::compare(data1, data2, offset1 , offset2);
//#define DEBUG_FILES
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/User_compare.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "Compare: age1 and age2 set and returns: " << tmp << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
                                if( tmp != 0) return tmp;
                        }
                }   //  age compared
                return 0; //If everything is equal
	}

	inline static int compare_by_surname(char *data1, char *data2)
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare_by_surname( data1 , data2 , &offset_1 , &offset_2 );
	}

	inline static int compare_by_surname (char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
	{
                ::samson::system::UInt8 local__filledOptFields__1;
                (*offset1) += local__filledOptFields__1.parse(data1+*offset1);
                ::samson::system::UInt8 local__filledOptFields__2;
                (*offset2) += local__filledOptFields__2.parse(data2+*offset2);
                { // comparing id
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  id compared
                { // comparing surname
                        int tmp = ::samson::system::String::compare(data1, data2, offset1 , offset2);
                        if( tmp != 0) return tmp;
                }   //  surname compared
                return 0; //If everything is equal
	}

	inline static int compare_by_expense(char *data1, char *data2)
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare_by_expense( data1 , data2 , &offset_1 , &offset_2 );
	}

	inline static int compare_by_expense (char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
	{
                ::samson::system::UInt8 local__filledOptFields__1;
                (*offset1) += local__filledOptFields__1.parse(data1+*offset1);
                ::samson::system::UInt8 local__filledOptFields__2;
                (*offset2) += local__filledOptFields__2.parse(data2+*offset2);
                { // comparing id
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  id compared
                { // comparing surname
			*offset1 += ::samson::system::String::size(data1+*offset1);
			*offset2 += ::samson::system::String::size(data2+*offset2);
                }   //  surname compared
                { // comparing companyId
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  companyId compared
		if (local__filledOptFields__1.value & 0x1)
                { // comparing age
			*offset1 += ::samson::system::UInt8::size(data1+*offset1);
			*offset2 += ::samson::system::UInt8::size(data2+*offset2);
                }   //  age compared
		if (local__filledOptFields__2.value & 0x1)
                { // comparing age
			*offset2 += ::samson::system::UInt8::size(data2+*offset2);
                }   //  age compared
                { // comparing expense
                        if (((local__filledOptFields__1.value & 0x2) != 0) && ((local__filledOptFields__2.value & 0x2) == 0))
                        {
                                return 1;
                        }
                        else if (((local__filledOptFields__1.value & 0x2) == 0) && ((local__filledOptFields__2.value & 0x2) != 0))
                        {
                                return -1;
                        }
                        else if ((local__filledOptFields__1.value & 0x2) && (local__filledOptFields__2.value & 0x2))
                        {
                                int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
                                if( tmp != 0) return tmp;
                        }
                }   //  expense compared
                return 0; //If everything is equal
	}

	inline static int compare_by_expense_rev(char *data1, char *data2)
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare_by_expense_rev( data1 , data2 , &offset_1 , &offset_2 );
	}

	inline static int compare_by_expense_rev (char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
	{
                ::samson::system::UInt8 local__filledOptFields__1;
                (*offset1) += local__filledOptFields__1.parse(data1+*offset1);
                ::samson::system::UInt8 local__filledOptFields__2;
                (*offset2) += local__filledOptFields__2.parse(data2+*offset2);
                { // comparing id
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  id compared
                { // comparing surname
			*offset1 += ::samson::system::String::size(data1+*offset1);
			*offset2 += ::samson::system::String::size(data2+*offset2);
                }   //  surname compared
                { // comparing companyId
			*offset1 += ::samson::system::UInt::size(data1+*offset1);
			*offset2 += ::samson::system::UInt::size(data2+*offset2);
                }   //  companyId compared
		if (local__filledOptFields__1.value & 0x1)
                { // comparing age
			*offset1 += ::samson::system::UInt8::size(data1+*offset1);
                }   //  age compared
		if (local__filledOptFields__2.value & 0x1)
                { // comparing age
			*offset2 += ::samson::system::UInt8::size(data2+*offset2);
                }   //  age compared
                { // comparing expense
                        if (((local__filledOptFields__1.value & 0x2) != 0) && ((local__filledOptFields__2.value & 0x2) == 0))
                        {
                                return -1;
                        }
                        else if (((local__filledOptFields__1.value & 0x2) == 0) && ((local__filledOptFields__2.value & 0x2) != 0))
                        {
                                return 1;
                        }
                        else if ((local__filledOptFields__1.value & 0x2) && (local__filledOptFields__2.value & 0x2))
                        {
                                int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
//#define DEBUG_FILES
#ifdef DEBUG_FILES
{
std::string filename = "/tmp/User_compare.log";
std::ofstream fs(filename.c_str(), std::ios::app);
fs << "Compare: expense1 and expense2 set and returns: " << tmp << std::endl;
fs.close();
}
#endif /* de DEBUG_FILES */
                                if( tmp != 0) return -tmp;
                        }
                }   //  expense compared
                return 0; //If everything is equal
	}

	};



} // end of namespace samson
} // end of namespace test_compareFunctions

#endif
