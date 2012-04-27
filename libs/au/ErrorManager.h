
/* ****************************************************************************
 *
 * FILE            ErrorManager.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 * Simple but usefull error mesasage information
 * Whatever sistem that can report an error can activate this error providing a message
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_ERROR
#define _H_AU_ERROR

#include <set>
#include "au/containers/list.h"
#include "au/containers/vector.h"
#include <vector>
#include <sstream>		// std::ostringstream

namespace au 
{
    typedef enum 
    {
        item_message,
        item_warning,
        item_error
    } ErrorType;
    
    class ErrorMessage
    {
        std::vector<std::string> contexts; // List of context where this error happened
        std::string message;
        ErrorType type;
        
    public:

        ErrorMessage( ErrorType _type ,std::list<std::string> &_contexts , std::string _message )
        {
            std::list<std::string>::iterator it_contexts;
            for( it_contexts = _contexts.begin() ; it_contexts != _contexts.end() ; it_contexts ++ )
                contexts.push_back( *it_contexts );
            message = _message;
            type = _type;
        }
        
        // Get message in one line
        std::string getMessage()
        {
            std::ostringstream output;
            for ( size_t i = 0 ; i < contexts.size() ; i ++ )
                output << contexts[i] << " >> ";
            
            output << message;
            
            return output.str();
        }
        
        std::string getMultiLineMessage()
        {
            std::ostringstream output;
            for ( size_t i = 0 ; i < contexts.size() ; i ++ )
                output << ">> " << contexts[i] << "\n";
            output << "    Error: "  << message << "\n";
            return output.str();
        }
        
        ErrorType getType()
        {
            return type;
        }
        
    };
    
    
    class ErrorManager
    {
        // List of errors ocurred so far
        au::vector<ErrorMessage> errors; 
        
        // Current list of context to emit errors
        std::list<std::string> contexts;
        
    public:
        
        ErrorManager();
        
        // Old method to add an error
        void set( std::string _message );

        // Methods to add elements
        void add_error( std::string message );
        void add_warning( std::string message );
        void add_message( std::string message );
        
        
        // Check if there is at least one error
        bool isActivated();
        
        // Get the last error message
        std::string getMessage(); 

        // Get the all errors message in multiple lines
        std::string getCompleteMessage(); 
        
        // Access individual items
        size_t getNumItems()
        {
            return errors.size();
        }
        
        ErrorMessage* getItem( size_t i )
        {
            if( i < errors.size() )
                return errors[i];
            return NULL;
        }
        
    private:
        
        friend class ErrorContext;
        // Add a context to this error manager
        void push_context( std::string context );
        void pop_context( );
        
    };

    // Class to define context over error messages
    
    class ErrorContext
    {
        ErrorManager* error;
        
    public:
        
        ErrorContext( ErrorManager* error , std::string content );
        ~ErrorContext();
    };
    
    
}
#endif
