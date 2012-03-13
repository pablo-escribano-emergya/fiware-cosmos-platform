
#ifndef _H_SAMSON_system_common
#define _H_SAMSON_system_common


namespace samson{
    namespace system{

        /*
         
         ========================================================================================================================================================
         INTRO
         ========================================================================================================================================================
         
         system.Value is a generic data type able to contain different data-types like: strings, numbers, vector of Values, map of Values, ...

         
         There are three basic types of operations to work with system.Value datas.
         
         system.parse_to_value           // Parse ascii file into a system.Value system.Value dataset.
                                         // Additional commands specified with "command" environment variable 
         
         system.map_value_to_value       // Map a system.Value-system.Value data-set into another system.Value system.Value data-set with command specified in "command" environment variable
         system.map_value_to_2_value     // The same with 2 output channels
         system.map_value_to_3_value     // The same with 3 output channels
         
         system.reduce_value_to_value    // Reduce a data set ( system.Value system.Value ) to create a nother one system.Value system.Value data-set

         ========================================================================================================================================================
         Commands for system.parse_to_value & sytem.map_value_to_X_value
         ========================================================================================================================================================
         
         With command environment variable we can specifiy the process to be perform over input key-values.
         Each key-value is process individually with the chain/s of operations provided in the command varialbe.
         
         The general format is as follows:
         
         run_stream_operation system.parser_to_value       <input> <output> env:command " ...commands... "
         run_stream_operation system.map_value_to_value    <input> <output> env:command " ...commands... "
         run_stream_operation system.map_value_to_2_value  <input> <output> env:command " ...commands... "
         run_stream_operation system.map_value_to_3_value  <input> <output> env:command " ...commands... "

         Where command is following this pattern
         
         " command_1 | command_2 | command_3 | emit 0 ; command_1 | emit 0 ; command_1 | command_2 | emit 1 ; ....... "
         
         So, multiple chains of commands (separated by ";" token) can be executed.
         A chain of commands is a set of different commands separated by the "|" token indicating that key-values travel across pipes
         
         ========================================================================================================================================================
         Individual Commands for system.parse_to_value & sytem.map_value_to_X_value
         ========================================================================================================================================================
         
         parse [-separator X ] [field_1 filed_2 field_3]
         
         Parse input key ( assuming they are strings ). Value is keep unchanged
         If no fields are specified, all of them ( varialbe number of fields per line ) are considered strings
         If fields are speficied, a different format is applyied to each field ( fixed number of fields per line )
         
         -separator X : Default separator is space. A different char can be used to separate columns
                      : tab is used to specify tabular
         
         field_x : string or number
                   More options will be added
         
         
         ------------------------------------------------------------------------------------------------------------------------------------------------

         parse_words
         
         Extract words ( only letters ) from key assuming it is a string. Value is keep unchanged
         
         ------------------------------------------------------------------------------------------------------------------------------------------------

         parse_chars
         
         Extract individual chars from key assuming it is a string. Value is keep unchanged
         
         ------------------------------------------------------------------------------------------------------------------------------------------------
         
         select key_selector value_selector : Construct a new key value form the original one
         
         Examples:
         
              select value key                       : Invert key and value
              select key[0] key[1]                   : Use first vector component of input key as output-key and second vector component of input key as output value. input value is ignored
              select [ key value ]  1                : Construct a vector for the output-key. This vector is formed using input key and input value. output-value is a constant 1
              select { "key":key "value":value } 1   : Construct a map for the output-key. This map is formed using input key and values. output-value is a constant 1
              
              select key strlen(key)                 : Emit the input key as output key. output-value is the length if input key assuming it is a string. strlen is a buildin function
         
         
         ------------------------------------------------------------------------------------------------------------------------------------------------

         filter selector comparisson selector : Filter key-values matching a particular criteria
         
         Example
          
                filter key == 'andreu'             : key has to be "andreu"
                filter strlen(key) >= 10           : Filter string keys with a length higher or equal 10
                filter key == value                : Filter key-values with the same value in both places
                filter strlen(key[0])<value        : Filter key-values where the first component of key ( assumed vector ) is lower than the value. Both assumed numbers

                filter isAlpha( key ) == 1         : filter key-values with an alphanumeric value as key
         
         ------------------------------------------------------------------------------------------------------------------------------------------------
         
         ========================================================================================================================================================
         Build-in functions to use when selecting or filtering key-values
         ========================================================================================================================================================
         
         
         srtlen( X )    Length of the input key assuming it is a string. If a vector is provided, it generated an output vector with components lengths.
         str( X )       Transform input value to a string. Tipically convert a number into a string
         num( X )       Transform input value to a number. Tipically convert a string into a number
         isAlpha( X )   Return "1" if all the letters in input are alphanumerical values. "0" is returned othersise
         
         */
        
    }
}