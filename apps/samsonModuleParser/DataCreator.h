/* ****************************************************************************
 *
 * FILE                     DataCreator.h
 *
 * DESCRIPTION				Creation of Data headeres
 *
 * ***************************************************************************/

#pragma mark once

#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <set>
#include <vector>
#include "DataContainer.h"		// DataContainer
#include "ModuleContainer.h"	// ModuleContainer
#include "OperationContainer.h"	// OperationContainer


namespace samson {

	class ModuleContainer;
	class OperationContainer;
	class DataContainer;
	
	class DataCreator
	{
        
	 public:
		
		// Name of the module
		std::string moduleFileName;

        // Output directory for .h files
        std::string outputDirectory;
        
		// Name of the output (.h / .cpp)
		std::string outputFileName;
		
		// Read the content of the file into a string
		std::string content;

		// Module object created from the parse
		ModuleContainer *module;
	  
		// List of parsed datas & operations
		vector <DataContainer> datas;
		vector <OperationContainer> operations;

        bool verbose;
        
		/* 
		 Constructor: name of the file to parse
		 */
		
		DataCreator( std::string moduleFileName , std::string outputDirectory , std::string outputFileName , bool _verbose );
		
		/**
		 Read the content of the file into a string
		 */
		
		void readFile( );
		
		/**
		 Parse content
		 Simple parser where all the commands are Command Name { definition  }
		 */
		
		void parse();
		
		/**
		 Print two two output files *.h and *.cpp
		 */
	  
		void print();
		
		// Print the header file
		void printMainHeaderFile();
		void printMainFile();
		
	};
}

