#ifndef MODULE_CONTAINER_H
#define MODULE_CONTAINER_H

/* ****************************************************************************
*
* FILE                     ModuleContainer.h
*
* DESCRIPTION				Information about a module (module definition)
*
*/
#include <unistd.h>             /* _exit, ...                              */
#include <string.h>             /* strtok, strcpy, ...                     */
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>

#include <samson/KVFormat.h>
#include "DataType.h"



namespace ss
{
	class DataCreator;
	class AUTockenizer;
	
	class ModuleContainer
	{
		
	public:
		
		std::string title;
		std::string name;
		std::string author;
		std::string version;
		
		std::vector <std::string> includes;
		std::vector <std::string> help;
		
		std::map<std::string,std::string> sets;
		
		ModuleContainer( std::string _name )
		{
			// Nick name could be sna or sna.cdrs or sna.cdrs.spain.... reverse domain
			name = _name;
		}
		
#pragma mark DEFINE NAME
		
		std::string getDefineUniqueName()
		{
			std::ostringstream o;
			o << "_H_SS_";
			
			std::vector<std::string> tockens = tockenizeWithDots( name.c_str() );	
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << tockens[i] << "_";
			
			o << "Module";
			return o.str();
			
		}
		
		
		
#pragma mark Begin and End namepsace definitions
		
		std::string getClassName()
		{
			return "Module";
		}
		
		std::string getFullClassName()
		{
			
			std::ostringstream o;
			o << "ss::";
			
			std::vector<std::string> tockens = tockenizeWithDots( name.c_str() );	
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << tockens[i] << "::";
			
			o << "Module";
			return o.str();
		}
		
		
#pragma mark Begin and End namepsace definitions
		
		
		std::vector<std::string> tockenizeWithDots( std::string myString )
		{
			assert( myString.length() < 1000 );
			char tmp[1000];
			strcpy(tmp, myString.c_str() );
			std::vector<std::string> tockens;
			char *p = strtok(tmp, ".");
			while (p) {
				tockens.push_back( std::string(p) ); 
				p = strtok(NULL, " ");
			}			
			return tockens;
		}
		
		void parse( AUTockenizer *module_creator ,int begin ,int end );
		
	};

}

#endif
