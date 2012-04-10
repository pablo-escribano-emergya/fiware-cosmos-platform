
#include "au/log/Log.h"
#include "au/log/LogFormatter.h"
#include "TableLogFormatter.h" // Own interface

namespace au 
{
    TableLogFormatter::TableLogFormatter( bool is_table , std::string format )
    {
        if( is_table )
        {
            // Table fields
            table_fields = split_using_multiple_separators( format , "|:;,-/\[] ()." );
            
            std::string table_definition;
            for ( size_t i = 0 ; i < table_fields.size() ; i++ )
            {
                if( table_fields[i].length() > 0 )
                    table_definition.append(  au::str("%s,left|", table_fields[i].c_str() ) );
            }
            
            table = new au::tables::Table( table_definition );
            log_formatter = NULL;
            
        }
        else
        {
            log_formatter = new LogFormatter( format );
            table = NULL;
        }
        
        
    }
    
    TableLogFormatter::~TableLogFormatter()
    {
        if( table )
            delete table;
        if( log_formatter )
            delete log_formatter;
    }
    
    void TableLogFormatter::add( Log* log )
    {
        if( table )
        {
            StringVector values;
            for ( size_t i = 0 ; i < table_fields.size() ; i++ )
                values.push_back( log->get(table_fields[i]) );
            table->addRow( values );
        }
        else
            output << log_formatter->get( log ) << "\n";
        
        
    }
    
    std::string TableLogFormatter::str()
    {
        if( table )
            return table->str();
        else
            return output.str();
        
    }

}