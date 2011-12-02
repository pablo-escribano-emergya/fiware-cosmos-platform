#ifndef _AU_CONSOLE_AUTO_COMPLETE
#define _AU_CONSOLE_AUTO_COMPLETE

#include <string>
#include <vector>

#include "au/au_namespace.h"


NAMESPACE_BEGIN(au)


class ConsoleAutoCompleteAlternative
{
    
public:
    
    std::string label;  // Label to show on screen
    std::string command;   // Text to be compared with the current last word
    
    bool add_space_if_unique;
    
    
    ConsoleAutoCompleteAlternative( std::string txt )
    {
        label = txt;
        command = txt;
        add_space_if_unique = true;
    }
    
    ConsoleAutoCompleteAlternative( std::string _label, std::string _command ,  bool _add_space_if_unique  )
    {
        command = _command;
        label = _label;
        add_space_if_unique = _add_space_if_unique;
    }
    
};

class ConsoleAutoComplete
{
    
    // Alternatives for the last word        
    std::vector<ConsoleAutoCompleteAlternative> last_word_alternatives;
    
    std::string help_message;
    
protected:
    
    // Current message introduced so far
    std::string current_command;
    
    // Preivous words ( before the last one )
    std::vector<std::string> previous_words;
    std::string last_word;
    
    bool add_space_if_unique_alternative;
    
public:
    
    ConsoleAutoComplete( std::string command );
    
    // Check what is the current situation in auto-completion
    bool completingFirstWord();
    bool completingSecondWord();
    bool completingThirdWord();
    bool completingSecondWord( std::string first_word );
    bool completingThirdWord( std::string first_word , std::string second_word );
    
    // Add an alternative for the last word
    void add( std::string command );
    void add( std::string label , std::string command , bool add_space );
    void add( ConsoleAutoCompleteAlternative alternative );
    
    // Set some help message
    void setHelpMessage( std::string _help_message );
    
    // Add files in the current directory
    void auto_complete_files( std::string file_selector );
    
    // Get the string to add to the current comman
    std::string stringToAppend();
    std::string getHelpMessage();
    
    // Auxiliar functions
    int common_chars_in_last_word_alternative();
    bool necessary_print_last_words_alternatives();
    void print_last_words_alternatives();
    
};

NAMESPACE_END

#endif