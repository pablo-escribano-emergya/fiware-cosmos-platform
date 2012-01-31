/* ****************************************************************************
 *
 * FILE                     DataContainer.h
 *
 * DESCRIPTION				Information about a data type (module definition)
 *
 * ***************************************************************************/

#pragma once

#include <unistd.h>             /* _exit, ...                              */
#include <string.h>             /* strtok, strcpy, ...                     */
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#include <samson/module/KVFormat.h>
#include "DataType.h"
#include <sys/stat.h>			// mkdir(.)
#include <fstream>

#include "au/ErrorManager.h"

namespace samson
{
	class DataCreator;
	class AUTockenizer;

	class DataContainer
	{
		
	 public:
	  
		std::string module;					// Module where this data is included ( example sna )
		std::string name;					// Name of the element inside the module (example CDR)
		std::vector <DataType> items;		// Data items it includes
		bool any_optional;					// To signal when at least there is one optional field
        
		std::set<std::string> includes;		// List of includes necessary for the system

		size_t hashTypeItems;
        
        // Error management during parsing....
        au::ErrorManager error;
        
		DataContainer( std::string _module, std::string _name )
		{
			module = _module;
			name = _name; 
			any_optional = false;
			hashTypeItems = 0;
		}
		
		void addItem( DataType item)
		{
			items.push_back( item );

			if (item.optional)
			{
				any_optional = true;
			}
			
			includes.insert( item.getInclude() );
			
		}
		
		bool parse( AUTockenizer *module_creator , int begin , int end );
		
		std::string mainClassName()
		{
			return name;
		}
		
		void printBaseFile(std::string directory)
		{
			size_t valMask;
			
			if( items.size() == 0)
			{
                /*
                if( verbose ) 
                    std::cout << "File " << name << "_base.h not created since there are no internal fields.\n";
                 */
				return ;
			}

            /*
			if( verbose ) 
                std::cout << "Creating file " << name << "_base.h\n";
             */
			
			// Prepare the files and directories
			std::ostringstream fileName;
			fileName << directory;	// Base directory
			fileName << "samson";
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/modules";
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/" << module;
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/" << name << "_base.h";

			std::ofstream file( fileName.str().c_str() );
			
			// Computing hashTypeItems
			std::string acumItems = "";
			for (vector <DataType>::iterator field =items.begin() ; field != items.end() ; field++)
			{
				if ((*field).optional)
				{
					acumItems += "opt";
				}
				if ((*field).isVector())
				{
					acumItems += "vect";
				}
				acumItems += (*field).fullType;
			}
			hashTypeItems = functHashTypeItems(acumItems);


			//Fields to control optional serializing
			if (any_optional)
			{
				int nOptFields = 0;
				for (vector <DataType>::iterator field =items.begin() ; field != items.end() ; field++)
				{
					if ((*field).optional)
					{
						nOptFields++;
					}
				}

				valMask = 0;

				if (nOptFields <= 8)
				{
					DataType optFilledField(UINT8, NAME_FILLEDOPTIONALFIELDS, DataType::container_none, false, valMask, 0);
					items.insert(items.begin(), optFilledField);
					includes.insert( optFilledField.getInclude() );
				}
				else if (nOptFields <= 16)
				{
					DataType optFilledField(UINT16, NAME_FILLEDOPTIONALFIELDS, DataType::container_none, false, valMask, 0);
					items.insert(items.begin(), optFilledField);
					includes.insert( optFilledField.getInclude() );
				}
				else if (nOptFields <= 32)
				{
					DataType optFilledField(UINT32, NAME_FILLEDOPTIONALFIELDS, DataType::container_none, false, valMask, 0);
					items.insert(items.begin(), optFilledField);
					includes.insert( optFilledField.getInclude() );
				}
				else if (nOptFields <= 64)
				{
					DataType optFilledField(UINT64, NAME_FILLEDOPTIONALFIELDS, DataType::container_none, false, valMask, 0);
					items.insert(items.begin(), optFilledField);
					includes.insert( optFilledField.getInclude() );
				}
				else
				{
#ifdef __LP64__
					fprintf(stderr, "Optional fields (nOptFields:%d) for more than %lu fields, not yet supported\n", nOptFields, 8*sizeof(size_t));
#else
					fprintf(stderr, "Optional fields (nOptFields:%d) for more than %d fields, not yet supported\n", nOptFields, 8*sizeof(size_t));
#endif
					exit(1);
				}

			}


			file << "\n";
			file << "/**\n";
			file << " File autogenerated with samsonModuleParser, please do not edit\n";
			file << "*/\n";
			file << "\n";

			file << "#ifndef _H_SAMSON_" << module << "_" << name << "_BASE\n";
			file << "#define _H_SAMSON_" << module << "_" << name << "_BASE\n";
			
			file << "\n";
			file << "\n";
			
            
            // Basic include files
            file << "#include <samson/module/List.h>   // Used in list elements\n"; 
				
			std::set<std::string>::iterator include;
			for ( include = includes.begin() ; include != includes.end() ; include++)
				file << "#include " << *include << "\n";
			
			file << "\n";
			file << "\n";
			
			file << "namespace samson{\n";
			file << "namespace " << module << "{\n";
			
			file << "\n";
			file << "\n";

			file << "\tclass " << name << "_base : public samson::DataInstance{\n\n";
			
			file << "\tpublic:\n";
			


			//Field definition
			for (vector <DataType>::iterator field =items.begin() ; field != items.end() ; field++)
				file << (*field).getDeclaration("\t");
			file << "\n";

			// Constructor
			file << "\t"<<name<<"_base() : samson::DataInstance(){\n";

			for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
				file << (*field).getInitialization("\t\t");
			file << "\t}\n\n";
			
			//Destructor
			file << "\t~" << name << "_base() {\n";
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				file << (*field).getDestruction("\t\t");
			file << "\t}\n\n";
			
			if (any_optional)
			{
				file << "\tvoid unassignAllOptionals(){\n";
				for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
					file << (*field).getUnassignedOptionals("\t\t");
				file << "\t}\n\n";

			}

            // get name
            file << "\tstd::string getName(){\n";
            file << "\t\treturn \"" << module << "." << name << "\";\n";
            file << "\t}\n\n";
            
			//Parse
			file << "\tint parse(char *data){\n" ;
			file << "\t\tint offset=0;\n";
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				file << (*field).getParseCommand("\t\t");
			}
			file << "\t\treturn offset;\n";
			file << "\t}\n\n";
			
			
			//Serialization
			file << "\tint serialize(char *data){\n";
			file << "\t\tint offset=0;\n";
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				file << (*field).getSerializeCommand("\t\t");
			}
			file << "\t\treturn offset;\n";
			file << "\t}\n\n";
			
			//Size
			file << "\tstatic inline int size(char *data){\n";
			file << "\t\tint offset=0;\n";
			for ( vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				if ((any_optional) && ((*field).valMask == 0))
				{
					file << "\t\t" << (*field).classNameForType() << " local" << NAME_FILLEDOPTIONALFIELDS << ";\n";
					file << (*field).getParseCommandIndividual("\t\t", string("local")+string(NAME_FILLEDOPTIONALFIELDS)) << "\n";
				}
				else
				{
					file << (*field).getSizeCommand("\t\t");
				}
			}
			file << "\t\treturn offset;\n";
			file << "\t}\n\n";
			
			//Get partition function
			if( items.size() > 0 ) 
			{
				file << "\tint hash(int max_num_partitions){\n";
				//Only get partition with the first field
				vector <DataType>::iterator field = items.begin();
				if (any_optional)
				{
					field++;
				}
				file << (*field).getPartitionCommand("\t\t");
				file << "\t}\n\n";
			}
			
			//Comparison
			file << "\tinline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){\n";

			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				if ((any_optional) && ((*field).valMask == 0))
				{
					file << "\t\t" << (*field).classNameForType() << " local" << NAME_FILLEDOPTIONALFIELDS << "1;\n";
					file << "\t\t" << (*field).getParseCommandForCompare(string("local")+string(NAME_FILLEDOPTIONALFIELDS)+string("1"), "1") << "\n";
					file << "\t\t" << (*field).classNameForType() << " local" << NAME_FILLEDOPTIONALFIELDS << "2;\n";
					file << "\t\t" << (*field).getParseCommandForCompare(string("local")+string(NAME_FILLEDOPTIONALFIELDS)+string("2"), "2") << "\n";
				}
				else
				{
					file << (*field).getCompareCommand("\t\t");
				}
			}
			file << "\t\treturn 0; //If everything is equal\n";
			file << "\t}\n\n";
			
			// Global compare function
			file <<"\tinline static int compare( char* data1 , char* data2 )\n";
			file <<"\t{\n";
			file <<"\t\tsize_t offset_1=0;\n";
			file <<"\t\tsize_t offset_2=0;\n";
			file <<"\t\treturn compare( data1 , data2 , &offset_1 , &offset_2 );\n";
			file <<"\t}\n\n";
			
			//Optional functions
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				file << (*field).getSetAssignedFunction("\t");
				file << (*field).getGetAssignedFunction("\t");
				file << "\n";
			}
			file << "\n";

			
			//Vector functions
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				if( (*field).isVector() )
				{
					file << (*field).getSetLengthFunction("\t");
					file << (*field).getAddFunction("\t");
				}
			}
			
			//Functions to translate and access data instances from name string.
			// External API from string to int*
			// This piece of code is the same for every data type

			file << "\tint *getDataPath(const std::string &dataPathString){\n";
			file << "\t\treturn(getDataPathStatic(dataPathString));\n\t}\n";


			file << "\tstatic int *getDataPathStatic(const std::string &dataPathString){\n";
			file << "\t\tconst char *dataPathCharP = dataPathString.c_str();\n";
			file << "\t\tint nlevels = 1;\n";
			file << "\t\tint *dataPathIntP;\n\n";
			file << "\t\tconst char *p_sep = dataPathCharP;\n";
			file << "\t\twhile ((p_sep = strchr(p_sep, '.')) != NULL)\n\t\t{\n";
			file << "\t\t\tnlevels++;\n";
			file << "\t\t\tp_sep++;\n\t\t}\n\n";
			file << "\t\tif ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)\n\t\t{\n";
			file << "\t\t\treturn ((int *)NULL);\n\t\t}\n\n";
			file << "\t\tint retError = getDataPath(dataPathCharP, dataPathIntP);\n\n";
			file << "\t\tif (retError)\n\t\t{\n";
			file << "\t\t\tfree(dataPathIntP);\n";
			file << "\t\t\tdataPathIntP = NULL;\n\t\t}\n\n";
			file << "\t\treturn  (dataPathIntP);\n";
			file << "\t}\n\n";

			// Internal API from char* to int*
			file << "\tstatic int getDataPath(const char * dataPathCharP, int *dataPathIntP){\n";
			file << "\t\tif (*dataPathCharP == 0)\n\t\t{\n";
			file << "\t\t\t*dataPathIntP = -1;\n";
			file << "\t\t\treturn (0);\n\t\t}\n\n";
			file << "\t\tif (strcmp(dataPathCharP, \"" << name << "\") == 0)\n\t\t{\n";
			file << "\t\t\t*dataPathIntP = -1;\n";
			file << "\t\t\treturn (0);\n\t\t}\n\n";
			file << "\t\tif (strncmp(dataPathCharP, \"" << name << ".\", strlen(\"" << name << ".\")) == 0)\n\t\t{\n";
			file << "\t\t\treturn (getDataPath(dataPathCharP+strlen(\"" << name << ".\"), dataPathIntP));\n\t\t}\n";
			int index = 0;
			for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
			{
				if ((*field).name != NAME_FILLEDOPTIONALFIELDS)
				{
					file << (*field).getGetDataPath("\t\t", index);
					index++;
				}
			}
			file << "\t\treturn -1;\n";
			file << "\t}\n\n";

			//Functions to translate and access data instances from name string.
			// External API from string to int*
			// This piece of code is the same for every data type
			file << "\tstd::string getTypeFromPath(const std::string &dataPathString){\n";
			file << "\t\tconst char *dataPathCharP = dataPathString.c_str();\n";
			file << "\t\treturn(getTypeFromPathStatic(dataPathCharP));\n\n";
			file << "\t}\n\n";

			// Internal API from char* to int*
			file << "\tstatic std::string getTypeFromPathStatic(const char * dataPathCharP){\n";
			file << "\t\tif (*dataPathCharP == 0)\n\t\t{\n";
			file << "\t\t\treturn (\"" << module << "." << name << "\");\n\t\t}\n\n";
			file << "\t\tif (strcmp(dataPathCharP, \".\") == 0)\n\t\t{\n";
			file << "\t\t\treturn (\"" << module << "." << name << "\");\n\t\t}\n\n";
			file << "\t\tif (strcmp(dataPathCharP, \"" << name << "\") == 0)\n\t\t{\n";
			file << "\t\t\treturn (\"" << module << "." << name << "\");\n\t\t}\n\n";
			file << "\t\tif (strncmp(dataPathCharP, \"" << name << ".\", strlen(\"" << name << ".\")) == 0)\n\t\t{\n";
			file << "\t\t\treturn (getTypeFromPathStatic(dataPathCharP+strlen(\"" << name << ".\")));\n\t\t}\n";
			index = 0;
			for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
			{
				if ((*field).name != NAME_FILLEDOPTIONALFIELDS)
				{
					file << (*field).getGetTypeFromStr("\t\t", index);
					index++;
				}
			}
			file << "\t\treturn(" << "\"_ERROR_\"" << ");\n";
			file << "\t}\n\n";




			file << "\tstd::string getTypeFromPath(const int *dataPathIntP){\n";
			file << "\t\treturn(getTypeFromPathStatic(dataPathIntP));\n\t}\n";

			// API to get the data type from the int* path
			file << "\tstatic std::string getTypeFromPathStatic(const int *dataPathIntP){\n";
			file << "\t\tswitch(*dataPathIntP)\n\t\t{\n";
			file << "\t\t\tcase -1:\n";
			file << "\t\t\t\treturn (\"" << module << "." << name << "\");\n";
			file << "\t\t\t\tbreak;\n";
			index = 0;
			for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
			{
				if ((*field).name != NAME_FILLEDOPTIONALFIELDS)
				{
					file << (*field).getGetType("\t\t\t", index);
					index++;
				}
			}
			file << "\t\t\tdefault:\n";
			file << "\t\t\t\treturn (" << "\"_ERROR_\"" << ");\n";
			file << "\t\t\t\tbreak;\n";
			file << "\t\t};\n";
			file << "\t}\n\n";

			// API to get the Type from static
			file << "\tstatic const char *getTypeStatic(){\n";
			file << "\t\treturn(\"" << module << "." << name << "\");\n";
			file << "\t}\n\n";

			// API to get the Type from instance
			file << "\tconst char *getType(){\n";
			file << "\t\treturn(\"" << module << "." << name << "\");\n";
			file << "\t}\n\n";

			// API to check the Type from static
			file << "\tstatic bool checkTypeStatic(const char *type){\n";
			file << "\t\tif (strcmp(type, \"" << module << "." << name << "\") == 0)\n\t\t{\n";
			file << "\t\t\treturn true;\n\t\t}";
			file << "\t\treturn false;\n";
			file << "\t}\n\n";

			// API to check the Type from instance
			file << "\tbool checkType(const char *type){\n";
			file << "\t\tif (strcmp(type, \"" << module << "." << name << "\") == 0)\n\t\t{\n";
			file << "\t\t\treturn true;\n\t\t}";
			file << "\t\treturn false;\n";
			file << "\t}\n\n";

			// API to get the hashType from static
			file << "\tstatic size_t getHashTypeStatic(){\n";
			file << "\t\treturn(" << hashTypeItems << "ULL);\n";
			file << "\t}\n\n";

			// API to get the hashType from instance
			file << "\tsize_t getHashType(){\n";
			file << "\t\treturn(" << hashTypeItems << "ULL);\n";
			file << "\t}\n\n";

			// API to check the hashType from static
			file << "\tstatic bool checkHashTypeStatic(size_t valType){\n";
			file << "\t\tif (valType == " << hashTypeItems << "ULL)\n\t\t{\n";
			file << "\t\t\treturn true;\n\t\t}";
			file << "\t\treturn false;\n";
			file << "\t}\n\n";

			// API to check the hashType from instance
			file << "\t bool checkHashType(size_t valType){\n";
			file << "\t\tif (valType == " << hashTypeItems << "ULL)\n\t\t{\n";
			file << "\t\t\treturn true;\n\t\t}";
			file << "\t\treturn false;\n";
			file << "\t}\n\n";


			// API to get an instance from the int* path
			file << "\tDataInstance * getDataInstanceFromPath(const int *dataPathIntP){\n";
			file << "\t\tswitch(*dataPathIntP)\n\t\t{\n";
			file << "\t\t\tcase -1:\n";
			file << "\t\t\t\treturn(this);\n";
			file << "\t\t\t\tbreak;\n";
			index = 0;
			for (vector <DataType>::iterator field = items.begin(); field != items.end() ; field++)
			{
				if ((*field).name != NAME_FILLEDOPTIONALFIELDS)
				{
					file << (*field).getGetInstance("\t\t\t", index);
					index++;
				}
			}
			file << "\t\t\tdefault:\n";
			file << "\t\t\t\treturn (NULL);\n";
			file << "\t\t\t\tbreak;\n";
			file << "\t\t};\n";
			file << "\t}\n\n";

			//Copy from
			file << "\tvoid copyFrom( "<< name << "_base *other ){\n";
			
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				file << (*field).getCopyFromCommand("\t\t");
			
			file << "\t};\n";
			
			// str
			file << "\n\tstd::string str(){\n";
			file << "\t\tstd::ostringstream o;\n";
			
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				file << (*field).getToStringCommand("\t\t") ;
				file << "\t\to<<\" \";\n";
			}
			
			file <<"\t\treturn o.str();\n";
			file <<"\t}\n\n";
			
			
			file << "\t}; //class "<< name << "_base\n\n";	

			// End of namspace
			

			file << "} // end of namespace " << module << "\n";
			file << "} // end of namespace samson\n";

			file << "\n";
			
			file << "#endif\n";

			
			file.close();
			
		}
		
		std::string getIncludeFile()
		{
			std::ostringstream output;
			output << "<samson/modules/" << module << "/" << name << ".h>";
			return output.str();
		}
		
		void printNominalFile(std::string directory)
		{
			// Prepare the files and directories
			
			std::ostringstream fileName;
			fileName << directory;	// Base directory
			fileName << "samson";
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/modules";
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/" << module;
			mkdir( fileName.str().c_str() , 0755 );
			fileName << "/" << name << ".h";

			
			std::ifstream _file( fileName.str().c_str() );
			if( _file.is_open() )
			{
				_file.close();
                
                /*
                if( verbose ) 
                    std::cout << "File " << name << ".h is not generated because it already exist\n";
                 */
				return;
			}
			else
            {
                /*
                if( verbose ) 
                    std::cout << "Creating file " << name << ".h\n";
                 */
            }

			
			std::ofstream file( fileName.str().c_str() );
	
			file << "\n";
			file << "/**\n";
			file << " File autogenerated with samsonModuleParser. Please, edit to complete this data\n";
			file << "*/\n";
			file << "\n";
			
			file << "#ifndef _H_SAMSON_" << module << "_" << name << "\n";
			file << "#define _H_SAMSON_" << module << "_" << name << "\n";
			
			file << "\n";
			file << "\n";
			
			file << "#include <samson/modules/" << module << "/" << name << "_base.h>\n";
			
			file << "\n";
			file << "\n";

			// Name space
			file << "namespace samson{\n";
			file << "namespace " << module << "{\n";

			file << "\n";
			file << "\n";
			
			file << "\tclass " << name << " : public " << name << "_base\n";
			file << "\t{\n";
			file << "\t};\n";
			
			file << "\n";
			file << "\n";
			
			// End of namspace
			file << "} // end of namespace samson\n";
			file << "} // end of namespace " << module << "\n";
			
			file << "\n";
			
			file << "#endif\n";
			
			file.close();			
			
		}
		
		void printFiles( std::string directory)
		{
			// Print the base file
			printBaseFile(directory);
			
			// Print the nominal file ( only if it does not exist )
			printNominalFile(directory);

		}
		
		size_t functHashTypeItems(std::string acumItems)
		{
			static const size_t InitialFNV = 2166136261U;
			static const size_t FNVMultiple = 16777619;

			size_t hash = InitialFNV;
			for(size_t i = 0; i < acumItems.length(); i++)
			{
				hash = hash ^ (acumItems[i]);       /* xor  the low 8 bits */
				hash = hash * FNVMultiple;      /* multiply by the magic number */
			}
			return hash;
		}

	};

}



