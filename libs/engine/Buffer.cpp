/* ****************************************************************************
 *
 * FILE                     Buffer.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include <sstream>          // std::stringstream
#include "au/string.h"		// au::Format
#include "engine/Buffer.h"	// Own interface
#include "au/xml.h"         // au::xml...

#include "logMsg/logMsg.h"                     // lmInit, LM_*
#include "logMsg/traceLevels.h"                // Trace Levels


namespace engine {

	Buffer::Buffer( std::string name ,   size_t max_size , int _tag )
	{
		_max_size = max_size;
		_offset = 0;
		
		tag = _tag;

		_name = name;
		
		if( max_size > 0)
		{
			_data = (char *) malloc(max_size);
			if( !_data )
			{
#ifdef __LP64__
			   LM_X(1,("Error (errno:%d) allocating memory for %lu bytes for tag:%d and name:'%s'", errno, max_size, tag, name.c_str()));
#else
			   LM_X(1,("Error (errno:%d) allocating memory for %d bytes for tag:%d and name:'%s'", errno, max_size, tag, name.c_str()));
#endif
			}
		}
		else
		{
			LM_W(("Buffer request of max_size(%lu) <= 0, for tag:%d and name:'%s'", max_size, tag, name.c_str()));
			_data = NULL;
		}
		
		_size = 0;
	}
	
	Buffer::~Buffer()
	{
		free();
	}
	
	std::string Buffer::str()
	{
        return au::str("[ Buffer (%s) MaxSize: %lu Size: %lu Offset %lu ]" , _name.c_str() , _max_size , _size , _offset );
	}

	
	void Buffer::free()
	{
		if( _data )
		{
			::free( _data );
			_data = NULL;
		}
		_max_size = 0;
		_size = 0;
		_offset = 0;
	}
	
	/**
	 Function to write content updating the size variable coherently
	 If it is not possible to write the entire block, it return false
	 So, it never try to write less than input_size bytes
	 */
	
	bool Buffer::write( char * input_buffer , size_t input_size )
	{
		if( _size + input_size > _max_size )
			return false;
		
		memcpy(_data + _size, input_buffer, input_size);
		_size += input_size;
		
		return true;
	}
	
	
	bool Buffer::skipWrite( size_t size )
	{
		if( _size + size <= _max_size)
		{
			_size += size;
			return true;
		}
		else
			return false;
	}
	
	
	size_t Buffer::skipRead( size_t size)
	{
		if( _offset + size > _max_size)
			size = (_max_size - _offset);	// Max offset
		
		_offset += size;
		return size;
		
	}
	
	/**
	 Write on the buffer the maximum possible ammount of data
	 */
	
	void Buffer::write( std::ifstream &inputStream )
	{
		inputStream.read( _data + _size , _max_size - _size ); 
		_size += inputStream.gcount();
	}
	
	
	/* 
	 Remove the last characters of an unfinished line and put them in buffer.
	 Remove the size of this set of characters
	 */
	
	int Buffer::removeLastUnfinishedLine( char ** buffer , size_t *buffer_size)
	{
        
		size_t last_line_size = 0;
		while( ( last_line_size < getSize() ) && (_data[_size - last_line_size - 1] != '\n' ) )
			last_line_size++;
		
        if( last_line_size == getSize() )
            return 1;   // Error... not final line found in the buffer

        *buffer = (char*) malloc( last_line_size );
		memcpy(*buffer, _data + _size - last_line_size , last_line_size);
		
        *buffer_size = last_line_size;

        // Reduce the size of this buffer
		_size -= last_line_size;
        
		return 0;
	}
	
	/**
	 Read content of the buffer in a continuous way
	 */
	
	size_t Buffer::read( char *output_buffer, size_t output_size)
	{
		size_t read_size = output_size;
		if( read_size > ( _size - _offset ) )
			read_size = (_size - _offset);
		
		memcpy(output_buffer, _data+_offset, read_size);
		_offset += read_size;
		return read_size;
	}
	
	/**
	 Auxilir functions to work directly with the content
	 */
	
	size_t Buffer::getSizePendingRead()
	{
		return _size - _offset;
	}
	
	size_t Buffer::getAvailableWrite()
	{
		return _max_size - _size;
	}
	
	char *Buffer::getData()
	{
		return _data;
	}
	
	size_t Buffer::getMaxSize()
	{
		return _max_size;
	}
	
	size_t Buffer::getSize()
	{
		return _size;
	}
	
	void Buffer::setSize( size_t size )
	{
		if ( size <= _max_size )
			_size = size;
	}
	
	
	SimpleBuffer Buffer::getSimpleBuffer()
	{
		return SimpleBuffer( _data , _max_size );
	}
	
	SimpleBuffer Buffer::getSimpleBufferAtOffset(size_t offset)
	{
		return SimpleBuffer( _data + offset , _max_size - offset );
	}
	
    void Buffer::getInfo( std::ostringstream& output)
    {
        au::xml_open(output , "buffer");
        au::xml_simple(output, "max_size", _max_size);
        au::xml_simple(output, "size", _size);
        au::xml_simple(output, "offset", _offset);
        au::xml_simple(output, "name", _name);
        au::xml_close(output , "buffer");
    }

	
}
