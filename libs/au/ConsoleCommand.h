

#ifndef _AU_CONSOLE_COMMAND
#define _AU_CONSOLE_COMMAND

#include <string>

#include "au/au_namespace.h"

NAMESPACE_BEGIN(au)


class ConsoleCommand
{
    std::string command;    // Command we are currently reading from the user
    int pos;                // Current position in the command
    
public:
    
    ConsoleCommand();
    ConsoleCommand( std::string _command );
    
    // Add chars and string to current command
    void add( char c );
    void add( std::string txt );
    
    void delete_char();
    void delete_word();
    void delete_rest_line();
    
    void move_home();
    void move_end();
    void move_cursor( int offset );
    
    std::string getCommand();
    std::string getCommandUntilPointer();
    int getPos();
    
    bool isCursorAtEnd();
    
    void setCommand( std::string _command );
    
};

NAMESPACE_END

#endif