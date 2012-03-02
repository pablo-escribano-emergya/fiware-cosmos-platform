/* ****************************************************************************
*
* FILE                 ModulesManager.cpp - 
*
*/
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include <ios>
#include <string>
#include <fnmatch.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"			// Lmt...

#include "au/TokenTaker.h"                          // au::TokenTake

#include "samson/common/MessagesOperations.h"
#include "samson/common/samsonDirectories.h"		/* SAMSON_MODULES_DIRECTORY                 */
#include "samson/common/MessagesOperations.h"		// evalHelpFilter(.)
#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup

#include <samson/module/samsonVersion.h>   /* SAMSON_VERSION                           */
#include <samson/module/Module.h>
#include "samson/module/ModulesManager.h"			/* Own interface                            */
#include <samson/module/Data.h>			/* samson::system::UInt ... */

#include "select.h"

namespace samson
{
	
	static ModulesManager *modulesManager=NULL;
	
	ModulesManager::ModulesManager() : token("ModulesManager")
	{
		reloadModules();
	}
	
	ModulesManager::~ModulesManager()
	{
        // Remove the main instances of the modules created while loading from disk
        modules.clearMap();
        LM_T(LmtModuleManager,("Destroying ModulesManager"));
	}
    
    
    void ModulesManager::destroy()
    {
		if(!modulesManager)
			LM_X(1,("Destroting a non initializedModules Manager"));
        
        LM_V(("Destroying ModulesManager"));
        delete modulesManager;
        
        modulesManager = NULL;
    }
	
	void ModulesManager::init()
	{
		if(modulesManager)
			LM_X(1,("Error initializing Modules Manager twice"));
        
        LM_V(("Init ModulesManager"));
		modulesManager = new ModulesManager();
	}
    
	ModulesManager* ModulesManager::shared()
	{
		if( !modulesManager )
			LM_X(1,("Modules Manager not initialized"));

		return modulesManager;
	}
	
	void ModulesManager::addModules()
	{
		// Load modules from "~/.samson/modules" && "/etc/samson/modules"
		addModulesFromDirectory( SamsonSetup::shared()->modulesDirectory() );
	}

	void ModulesManager::addModulesFromDirectory( std::string dir_name )
	{	
        LM_T(LmtModuleManager,("Adding modules from directory %s", dir_name.c_str() ));
        
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir_name.c_str())) == NULL) {
			//logError( "Error opening directory for modules " + dir_name );
			return;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			
			std::string path = dir_name + "/" + dirp->d_name;
			
			struct ::stat info;
			stat(path.c_str(), &info);
			
			if( S_ISREG(info.st_mode) )
				addModule( path );
			
		}
		closedir(dp);
		
	}

    std::string ModulesManager::getModuleFileName( std::string module_name )
    {
        au::TokenTaker tt( &token , "ModulesManager::reloadModules");
        Module* module = modules.findInMap(module_name);
        if( !module )
            return "";
        
        return module->file_name;
    }

	
	void ModulesManager::reloadModules()
	{
        au::TokenTaker tt( &token , "ModulesManager::reloadModules");
        
		LM_T(LmtModuleManager,("Reloading modules"));
		
		LM_T(LmtModuleManager,("Clear previous operations and data-types"));
		clearModule();  		// Clear this module itself (operations and datas)
        
        // Close handlers for all open modules
		LM_T(LmtModuleManager,("Closing previous modules ( if any )"));
        au::map< std::string  , Module >::iterator m; 
        for ( m =  modules.begin(); m != modules.end() ; m++)
        {
            Module *module = m->second;
            int res = dlclose( module->hndl);
            if( res != 0)
                LM_W(("Error while closing module %s", module->name.c_str() )); 
        }

		LM_T(LmtModuleManager,("Remove list of previous modules"));
        modules.clearMap();     // Remove all modules stored here
		
		// Add modules again
		addModules();
		
        // Add the select operation
        add( new SelectOperation() );
        
		/*
		 Spetial operation to be moved to a proper file
		 */
		
		Operation *compact = new Operation( "samson.compact" , Operation::system );
		compact->inputFormats.push_back( samson::KVFormat::format("*" ,"*") );
		add( compact );
        
		
	}
	
	typedef Module*(*moduleFactory)();
	typedef std::string(*getVersionFunction)();

    
    Status ModulesManager::loadModule( std::string path , Module** module , std::string* version_string )
	{
        LM_T(LmtModuleManager,("Adding module at path %s", path.c_str() ));
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL)
        {
            LM_W(("Not possible to dlopen for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			return Error;
		}
        
		void *mkr = dlsym(hndl, "moduleCreator");		
		if(mkr == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return Error;
		}
        
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		if(getVersionPointer == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return Error;
		}
		
        moduleFactory f = (moduleFactory)mkr;
        getVersionFunction fv = (getVersionFunction)getVersionPointer;

        // Get module and get version
        *module = f();
        *version_string = fv();

        return OK;
	}
    
    
    
	void ModulesManager::addModule( std::string path )
	{
        LM_T(LmtModuleManager,("Adding module at path %s", path.c_str() ));
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL)
        {
            LM_W(("Not possible to dlopen for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			return;
		}
        
		void *mkr = dlsym(hndl, "moduleCreator");		
		if(mkr == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return;
		}
        
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		if(getVersionPointer == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return;
		}
		
        moduleFactory f = (moduleFactory)mkr;
        getVersionFunction fv = (getVersionFunction)getVersionPointer;
        
        Module *module = f();
        module->file_name = path; // Save the full path of this module
        
        std::string platform_version  = fv();
        
        if ( !module )
        {
            LM_E(( "Not possible to load module at path %s (not container found)" , path.c_str()));
            dlclose(hndl);
            return;
        }
        
        if( platform_version == SAMSON_VERSION )
        {
            LM_T(LmtModuleManager,("Module %s compiled for version %s ... OK!" , module->name.c_str() , platform_version.c_str()  ));
            LM_T(LmtModuleManager,("Adding module %s (%s) %d ops & %d data-types" , 
                        module->name.c_str() , 
                        path.c_str() , 
                        (int)module->operations.size(),
                        (int)module->datas.size()
                                   ));
            
            module->hndl = hndl;
            addModule( module );
            
            // Insert in the map of modules for listing
            //LM_M(("Adding module %s version %s from %s with %d operations and %d data-types" , module->name.c_str() , module->version.c_str(), path.c_str() , module->operations.size() ,  module->datas.size() ));
            modules.insertInMap(path, module);
        }
        else
        {
            LM_W(("Not loading file %s since its using a different API version %s vs %s" , path.c_str() , platform_version.c_str() , SAMSON_VERSION ));
            delete module;
            
            // Close dynamic link
            dlclose(hndl);
        }
		
	}

	void ModulesManager::addModule(  Module *container )
	{
		// Copy all the opertion to this top-level module
		copyFrom( container	);
	}
	
    
    void ModulesManager::getInfo( std::ostringstream& output)
    {
        au::TokenTaker tt(&token);			//!< General lock for modules access
        
        au::xml_open(output , "modules_manager" );
        au::xml_iterate_map( output , "modules" , modules );
        au::xml_close(output , "modules_manager" );
    }
    
    samson::network::Collection* ModulesManager::getModulesCollection(VisualitzationOptions options ,  std::string pattern )
    {
        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("modules");
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            std::string name = it->second->name;
            if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
            {
                Module * module = it->second;
                
                network::CollectionRecord* record = collection->add_record();
                
                ::samson::add( record , "name"        , module->name              , "left,different" );
                ::samson::add( record , "version"     , module->version           , "different" );
                ::samson::add( record , "#operations" , module->operations.size() , "f=uint64,sum" );
                ::samson::add( record , "#datas"      , module->datas.size()      , "f=uint64,sum" );
                ::samson::add( record , "author"      , module->author            , "left,different" );
                
            }
        }
        return collection;
        
    }
    samson::network::Collection* ModulesManager::getDatasCollection(VisualitzationOptions options ,  std::string pattern )
    {
        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("datas");
        
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            Module * module = it->second;
            
            std::map<std::string, Data*>::iterator it_datas;
            for( 
                it_datas = module->datas.begin() ; 
                it_datas != module->datas.end()  ; 
                it_datas++)
            {
                
                std::string name = it_datas->first;
                Data * data = it_datas->second;
                
                if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
                {
                    
                    network::CollectionRecord* record = collection->add_record();
                    
                    ::samson::add( record , "name" , data->_name    , "" );
                    ::samson::add( record , "help" , data->help()   , "left" );
                }
                
            }
        }
        return collection;
        
    }

    samson::network::Collection* ModulesManager::getOperationsCollection(VisualitzationOptions options ,  std::string pattern )
    {
        
        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("operations");
        
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            Module * module = it->second;
            
            std::map<std::string, Operation*>::iterator it_operation;
            for( 
                it_operation = module->operations.begin() ; 
                it_operation != module->operations.end()  ; 
                it_operation++)
            {
                
                std::string name = it_operation->first;
                Operation * operation = it_operation->second;
                
                if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
                {
                    
                    network::CollectionRecord* record = collection->add_record();
                    
                    ::samson::add( record , "name" , operation->_name         , "left" );
                    ::samson::add( record , "type" , operation->getTypeName() , "" );
                    
                    //::samson::add( record , "help" , operation->helpLine()    , "" );
                    if( options == verbose )
                    {
                        ::samson::add( record , "inputs"  , operation->inputFormatsString()  , "" );
                        ::samson::add( record , "outputs" , operation->outputFormatsString()  , "" );
                    }
                }
                
            }
        }
        return collection;        
    }

	
}
