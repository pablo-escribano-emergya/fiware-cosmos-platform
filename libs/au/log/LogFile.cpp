

#include "Log.h"
#include "LogFile.h" // Own interface


namespace au 
{
    au::Status LogFile::read( std::string file_name , LogFile** logFile )
    {
        int fd = open( file_name.c_str() , O_RDONLY );
        if( fd < 0 )
            return OpenError;
        
        // File descriptor to read logs
        FileDescriptor file_descriptor( "reading log file" , fd );
        
        // Create log file
        *logFile = new LogFile();
        
        while (true) 
        {
            Log* log = new Log();
            if( log->read(&file_descriptor) )
                (*logFile)->logs.push_back( log );
            else
                break;
        }
        
        
        if( (*logFile)->logs.size() == 0 )
        {
            delete *logFile;
            *logFile = NULL;
            return Error;
        }
        
        return OK;
    }}