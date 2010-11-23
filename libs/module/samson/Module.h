#ifndef SAMSON_MODULE_H
#define SAMSON_MODULE_H

#include <string>
#include <map>

#include <samson/Operation.h>           /* Operation                                */
#include <samson/Data.h>                /* Data                                     */



namespace ss
{	
	class Module 
	{
	public:
		
		std::string name;		// Name of this container
		std::string version;	// Version of this module
		std::string author;		// Author of this module (mail included to report bugs)

		std::map<std::string, Operation*>  operations;	// Vector containing operations (map, generate, reduce)
		std::map<std::string, Data*>      datas;		// Vector containing data			
		
		friend class ModulesManager;
			
		void *hndl;	// Pointer for the dlOpen

		Module()
		{
			hndl    = NULL;
		}
		
		Module(std::string _name, std::string _version, std::string _author)
		{
			name    = _name;
			version = _version;
			author  = _author;
			hndl    = NULL;
		}
		
	public:		
		
		Operation* getOperation( std::string name )
		{
			std::map<std::string , Operation*>::iterator i;
			i = operations.find( name );
			if( i == operations.end() )
				return NULL;
			else
				return i->second;
		}
		
		Operation::Type getOperationType( std::string name )
		{
			Operation * o =  getOperation(name);
			if( !o )
				return Operation::unknown;
			return o->getType();
		}

		/**
		 Function to get a new instance
		 */
		
		Data* getData( std::string name )
		{
			std::map<std::string , Data*>::iterator i = datas.find( name );
			if ( i == datas.end() )
				return NULL;
			else
				return i->second;
		}		
		
		bool checkData( std::string name )
		{
			if( name == "txt" )
				return true;	// Spetial case
			
			std::map<std::string , Data*>::iterator i = datas.find( name );
			if ( i == datas.end() )
				return false;
			else
				return true;
		}
		
	public:
		
		/** 
		 Add operation to this module
		 */
		
		void add( Operation *operation )
		{
			operations.insert( std::pair<std::string , Operation*>( operation->getName() , operation) );
		}
		
		void add( Data* data )
		{
			datas.insert( std::pair<std::string , Data*> ( data->getName() , data ) );
		}

		void copyFrom( Module *m)
		{
			datas.insert( m->datas.begin() ,  m->datas.end() );
			operations.insert( m->operations.begin() , m->operations.end());
			
		}
		
		
	};
}

#endif
