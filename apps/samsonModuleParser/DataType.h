#ifndef DATATYPE_H
#define DATATYPE_H

/* ****************************************************************************
*
* FILE                     DataType.h
*
* DESCRIPTION				Definition of a basic data-type
*/
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>                   // exit


// Defines for the optional stuff
#define NAME_FILLEDOPTIONALFIELDS  "__filledOptFields__"
#define UINT8	"system.UInt8"
#define UINT16	"system.UInt16"
#define UINT32	"system.UInt32"
#define UINT64	"system.UInt64"

using namespace std;

namespace samson {

	// Basic operations with "." separated names
	std::vector<std::string> tockenizeWithDots( std::string myString );
	std::string getModuleFromFullName( std::string fullName );
	std::string getNameFromFullName( std::string fullName );
	
	class DataType
	{
	public:
		
		string fullType;
		string module;
		string type;
		string name;
		bool vector;
		bool optional;
		size_t valMask;
		
		DataType( std::string _full_type , std::string _name , bool _vector, bool _optional, size_t _valMask, int nline);
		
		/* Get the include file that is necessary to use this data type */
		std::string getInclude();
		
		/* Function to give us the name of a particular class */
		string classNameForType(  );

		/* Function to show the declaration of the field */
		string getDeclaration(string pre_line);
		
		/* Set length function... only in vectors */
		string	getSetLengthFunction( string pre_line );
		
		/* Set assigned function... only in optional variables */
		string	getSetAssignedFunction( string pre_line );

		/* Get assigned function... only in optional variables */
		string	getGetAssignedFunction( string pre_line );

		/* Add element function... only in vectors */
		string	getAddFunction( string pre_line );
		
		/* Initialization inside the constructor */
		string getInitialization(string pre_line);
		string getInitialization(string pre_line, string initialValue);
		
		string getDestruction(string pre_line);
		
		string getParseCommandIndividual( string pre_line, string _name );
		string getParseCommandForCompare( string _name, string indice );
		string getParseCommandVector( string pre_line, string _name );
		
		string getParseCommand(string pre_line);
		
		string getSerializationCommandIndividual( string pre_line, string _name);
		string getSerializationCommandVector( string pre_line, string _name);
		
		string getSerializeCommand( string pre_line );

		string getSizeCommandIndividual( string pre_line, string _name);
		string getSizeCommandVector( string pre_line, string _name);
		
		string getSizeCommand(string pre_line);

		string getPartitionCommandIndividual(string pre_line, string _name);
		string getPartitionCommandVector(string pre_line, string _name);
		
		string getPartitionCommand( string pre_line );
		
		string getCompareCommandIndividual( string pre_line ,  string _name );
		string getCompareCommandVector( string pre_line ,  string _name );
		
		string getCompareCommand( string pre_line );

		string getToStringCommandIndividual(string pre_line, string _name);
		string getToStringCommandVector(string pre_line, string _name);
		
		string getToStringCommand( string pre_line );

		string getCopyFromCommandIndividual(string pre_line, string _name);
		string getCopyFromCommandVector(string pre_line, string _name);
		
		string getCopyFromCommand(string pre_line);

	};
}

#endif
