
#include <string>

#include "logMsg/logMsg.h"

#include "SamsonConnector.h"
#include "BufferProcessor.h" // Own interface

#include "samson/module/ModulesManager.h"

#include "common.h"

extern char input_splitter_name[1024];
extern size_t buffer_size;
extern size_t input_buffer_size;

namespace samson {
    
    BufferProcessor::BufferProcessor( SamsonConnectorItem* _item , SamsonConnector * _stream_connector )
    {
        item = _item;
        stream_connector = _stream_connector;
        splitter_name = input_splitter_name;
        
        // Internal buffer ( to be used as input by the splitter )
        buffer = (char*) malloc( buffer_size );
        max_size = buffer_size;
        size = 0;
        
        if( splitter_name == "" )
        {
            splitter = NULL;
        }
        else
        {
        
            Operation* operation = ModulesManager::shared()->getOperation( splitter_name );
            
            if( !operation || ( operation->getType() != Operation::splitter ) )
                LM_X(1, ("Wrong splitter %s" , input_splitter_name ));
            
            // Get instace of the operation
            splitter = (Splitter*) operation->getInstance();
            if( !splitter )
                LM_X(1, ("Error getting instance of the splitter"));
        }
    
    }
    
    BufferProcessor::~BufferProcessor()
    {
        // Free allocted buffer
        if ( buffer )
            free( buffer );
        
    }
    

    // SplitterEmitter interface
    void BufferProcessor::emit( char* data , size_t length )
    {
        // If not possible to write in the current buffer, just flush content
        {
            engine::Buffer * output_buffer = output_buffer_container.getBuffer();
            if( output_buffer )
                if( output_buffer->getSize() + length > output_buffer->getMaxSize() )
                    flushOutputBuffer();
        }
        
        // Recover buffer to write output
        engine::Buffer * output_buffer = output_buffer_container.getBuffer();
        if( !output_buffer )
        {
            size_t output_buffer_size = std::max( length , (size_t) 64000000 - sizeof(KVHeader) ); // Minimum 64Mbytes buffer
            output_buffer = output_buffer_container.create("output_splitter", "connector", output_buffer_size );
        }
        
        // Write in the buffer
        if( !output_buffer->write(data, length) )
            LM_X(1, ("Internal error"));
        
        if( output_buffer->getSize() == output_buffer->getMaxSize() )
            flushOutputBuffer();
        
    }
    
    void BufferProcessor::flushOutputBuffer()
    {
        engine::Buffer* output_buffer = output_buffer_container.getBuffer();
        
        if( output_buffer )
        {
            stream_connector->push( output_buffer , item );
            output_buffer_container.release();
        }
        
    }
    
    void BufferProcessor::process_intenal_buffer( bool finish )
    {        
        // If no splitter, no process
        if( !splitter )
            return; // If no splitter, we never create an outputBuffer

        if( size == 0 )
            return;
        
        // Pointer to data that has not been used in splitter
        char * nextData;
        
        // Split buffer to know what to push
        int c = splitter->split(buffer, size, finish, &nextData, this );
        
        if( c == 0 )
        {
            // Flush pending data generated here
            flushOutputBuffer();
            
            // Data to be skip after process
            if( nextData )
            {
                
                if( ( nextData < buffer ) || (nextData > ( buffer + size) ) )
                {
                    LM_W(("Splitter %s has returned a wrong nextData pointer when processing a buffer of %s. Skipping this buffer" , input_splitter_name , au::str( max_size ).c_str()  ));
                    size = 0;
                }
                else
                {
                    size_t skip_size = nextData - buffer;
                    
                    // Move data at the begining of the buffer
                    memmove( buffer + skip_size , buffer, size - skip_size );
                    size -= skip_size;
                }
            }
            else
                size = 0; // if nextData is NULL Ignore all input data
        }
        else
        {
            if( size >= max_size ) 
            {
                LM_W(("Splitter %s is not able to split a full buffer %s. Skipping this buffer" , input_splitter_name , au::str( max_size ).c_str()  ));
                size = 0;
            }
            else
                return; // Not enoutght data for splitter... this is OK if the buffer is not full
        }
        
        
    }

    
    void BufferProcessor::push( engine::Buffer * input_buffer )
    {
        
        if( !splitter )
        {
            stream_connector->push( input_buffer, item );
            return;
        }
            
        size_t pos_in_input_buffer = 0; // Readed so far
        while( pos_in_input_buffer < input_buffer->getSize() )
        {
            size_t copy_size = std::min( max_size - size , input_buffer->getSize() - pos_in_input_buffer );
            
            memcpy(buffer+size, input_buffer->getData() + pos_in_input_buffer , copy_size);
            size += copy_size;
            pos_in_input_buffer += copy_size;

            
            // Process internal buffer every 2 seconds or when internal buffer is full
            if( ( size == max_size ) || ( cronometer.diffTimeInSeconds() > 2 ) )
            {
                // Reset cronometer
                cronometer.reset();
            
                // Process internal buffer to push blocks at the output
                process_intenal_buffer( false );
            }
        }
        
    }
    
    void BufferProcessor::flush()
    {
        process_intenal_buffer( true ); // Process internal buffer with the "finish" flag activated
    }
    
}