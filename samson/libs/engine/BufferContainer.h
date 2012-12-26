/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_BUFFER_CONTAINER
#define _H_BUFFER_CONTAINER

#include <string>
#include "au/containers/list.h"

namespace engine {
    
    class Buffer;
    
    // Class holding a buffer generated by memory manager
    
    class BufferContainer
    {
        Buffer* buffer;
        
    public:

        // Constructor and destructors
        BufferContainer();
        ~BufferContainer();

        // Get and set the internal buffer ( retaining and releasing acordingly )
        void setBuffer( Buffer * b );
        Buffer* getBuffer();
        
        // Release internal buffer ( if any )
        void release();
        
        // Handy operation to point to the same buffer
        void operator=( BufferContainer& other );

        // Create a buffer and retain internally
        Buffer* create( std::string name , std::string type , size_t size );
        
    };
 
    
    class BufferListContainer
    {
        au::list<Buffer> buffers;      
        
    public:
        
        ~BufferListContainer();
        
        void push_back( Buffer* buffer );

        void clear();

        // Get the next buffer
        Buffer* front();
        void pop();

        // Extract directly the next buffer and include them in a BufferContainer
        void pop( engine::BufferContainer * container );
        
        // Get total size included in the list
        size_t getTotalSize();
        
        // Get total number of buffers included here
        size_t getNumBuffers();

        // Extract from other list
        void extractFrom( BufferListContainer* other , size_t maximum_size );
        
        
    };
    
}

#endif

