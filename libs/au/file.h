
/* ****************************************************************************
 *
 * FILE            file.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull functions to work with files
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_FILE
#define _H_AU_FILE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map

namespace au {

    size_t sizeOfFile( std::string fileName );

    void removeDirectory( std::string fileName , au::ErrorManager & error ); 

    bool isDirectory(char path[]);
    bool isRegularFile(char path[]);

    
    bool check_string_ends_with( std::string txt , std::string end_txt );

    // Working with paths
    std::string path_remove_last_component( std::string path );
    std::string get_directory_from_path( std::string path );

   
    
    
}

#endif
