
#ifndef _AU_CONSOLE
#define _AU_CONSOLE

#include <string>
#include <list>
#include <termios.h>                // termios

#include "au/ConsoleCode.h"
#include "au/Token.h"
#include "au/namespace.h"

#include "ConsoleEscapeSequence.h"

NAMESPACE_BEGIN(au)

class Console;
class ConsoleAutoComplete;
class ConsoleCommandHistory;

class Console
{
    // History information ( all commands introduced before )
    ConsoleCommandHistory *command_history;
        
    // Pending messages to be displayed
    pthread_t t_running;
    std::list< std::string > pending_messages;
    au::Token token_pending_messages;
        
    // Flag to quit internal loop
    bool quit_console;

    // Counter used only for testing
    int counter; 

    // Detector of scape sequences
    ConsoleEscapeSequence escape_sequence;
    
public:
    
    Console();
    ~Console();
    
    void runConsole();
    void quitConsole();
    
    /* Methods to write things on screen */
    void writeWarningOnConsole( std::string message );
    void writeErrorOnConsole( std::string message );
    void writeOnConsole( std::string message );
    
    // Customize console
    virtual std::string getPrompt();
    virtual void evalCommand( std::string command );
    virtual void autoComplete( ConsoleAutoComplete* info );
    
    void addEspaceSequence( std::string sequence );
    virtual void process_escape_sequence( std::string sequence ){ sequence = "SEQ"; };
    
    void refresh();
    
    // Make sure all messages are shown
    void flush();
    
    /* ask about Console quit status */
    bool isQuitting();

private:
    
    void print_command();
    bool isImputReady();
    
    void process_auto_complete( ConsoleAutoComplete * info );
    void process_char( char c );

    void internal_process_escape_sequence( std::string sequence );
    void internal_command( std::string sequence );
    
    void process_background();
    bool isNormalChar( char c );
    
    
    void write( std::string message );
    
};

NAMESPACE_END

#endif

