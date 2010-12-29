#ifndef SAMSON_FORMAT_H
#define SAMSON_FORMAT_H

#include <stdio.h>             /* sprintf */
#include <string>				// std::string

namespace au {


	/**
	 Class to help the format of strings
	 */
	
	class Format
	{
		
	public:

		static std::string percentage_string( double value , double total );
		static std::string percentage_string( double p );
		static  std::string string( double value, char  letter );
		static std::string string( size_t memory );
		static std::string string( size_t memory , std::string postfix );
		static std::string int_string(int value, int digits);
		static std::string time_string( int seconds );
		
		
		static size_t sizeOfFile( std::string fileName );
		
	};
}

#endif
