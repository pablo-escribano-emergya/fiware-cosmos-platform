#ifndef _H_DELILAH_TRACE_COLLECTION
#define _H_DELILAH_TRACE_COLLECTION

#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/TokenTaker.h"                  // au::TokenTake
#include "au/Console.h"                     // au::Console
#include "au/ConsoleAutoComplete.h"

#include "tables/Select.h"
#include "tables/Table.h"

#include "DelilahClient.h"                  // ss:DelilahClient

#include "samson/delilah/Delilah.h"			// samson::Delilah

namespace samson {
	
    
    // Class used to store traces received from SAMSON
    
    class Trace
    {
        
    public:
        
        NodeIdentifier node;
        
        std::string type;
        std::string context;
        std::string text;
        
        Trace(  NodeIdentifier _node , std::string _type , std::string _context , std::string _text )
        {
            node = _node;
            type = _type;
            context = _context;
            text = _text;
        }
        
    };
    
    // class used to store some traces received from SAMSON
    
    class TraceCollection
    {
        
        au::list<Trace> traces;
        size_t max_num_elements;
        
    public:
        
        // Constructor
        TraceCollection();
        
        // Add traces to this collection ( limited number will be stored in memory )
        void add( NodeIdentifier node , std::string type , std::string context , std::string text );
        
        // Table with traces to be displayed on screen with show_traces command
        std::string str();
         
    };
    
}

#endif
