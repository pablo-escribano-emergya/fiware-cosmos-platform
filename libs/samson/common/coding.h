#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
*
* FILE                 coding.h - 
*
*/
#include <sys/time.h>                   // struct timeval
#include <string.h>                     // std::string

#include "au/string.h"                  // au::Format
#include "au/xml.h"                     // au::xml...

#include "samson/common/samson.pb.h"                  // network:...

#include <samson/module/Data.h>
#include "samson/module/KVFormat.h"	        // samson::KVFormat

#include "engine/SimpleBuffer.h"		// engine::SimpleBuffer
#include "logMsg/logMsg.h"                     // LM_X

#define MAX_UINT_32     4294967291 

#define KVFILE_MAX_KV_SIZE			   64*1024*1024				// Max size for an individual key-value
#define KVFILE_NUM_HASHGROUPS			64*1024					// Number of hash-groups

#define KVFILE_HASH_GROUP_VECTOR_SIZE		(sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS)					// Size of the vector containing information for each hash-group
#define KVFILE_TOTAL_HEADER_SIZE			(sizeof( KVHeader ) + KVFILE_HASH_GROUP_VECTOR_SIZE )		// Total size of the header + info vector

namespace samson {

	/*
	 
	 This file defines all the formats used to store key-values in files , network-buffers and shared-memory buffers

	 Generic samson format
	 --------------------------------------------------------------------------------
	 	 
	 SAMSON platforms stores all key-values in files with the following format:

	 Binarary files: ( containing key-values)

	 [KVHeader][KVInfo for each hash-group][data]
     
     Txt files containing only txt content   
     
     [KVHeader][txt-data]
	 
	 Where
	 
	 * KVHeader is a struct with some information about the format and content of the file ( it also indicated if it is a txt file )
	 * Info for each hash-group is a vector of "KV_NUM_HASH_GROUPS" structs of type KVInfo
	 * data is the buffer of real data with the key-values cofidied using the rigth serialization
     * txt-data is the buffer containing txt data

	 Note: In this case, all hash-groups are allways present
	 
	 SHARED MEMORY: (KVSharedFile)
	 --------------------------------------------------------------------------------

	 When a particular operation is executed (map,recuce of parseout), we need to store part or entire KVFile into a shared memory buffers
	 Indeed, if multiple files are needed, we store more than one in the same shared memory buffer
	 
	 [KVHeader][KVSharedFile1][KVSharedFile2][KVSharedFile3][KVSharedFile4] ....
	 
	 Where KVHeader informs about the number of files and some additional information
	 Each KVSharedFile has the following format
	 
	 [KVHeader][Info for each hash-group][data]
	 
	 where:
	 * KVHeader is a struct with some information about the process ( like total number of shared files and hash-group range)
	 * Info for each hash-group is a vector of "KV_NUM_HASH_GROUPS" structs of type KVInfo
	 * data is the buffer of real data with the key-values cofidied using the rigth serialization

	 Note that in this case, not all hash-group ranges have to be present.
	 
	 */
	

	// Unsigned types with different bits lengths
	
	typedef size_t uint64; 
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	
	/****************************************************************
	 KVInfo structure to keep information about size and # kvs
	 ****************************************************************/
	
	struct KVInfo
	{
		uint32 size;	// Total size
		uint32 kvs;		// Total number of kvs

		
		KVInfo(uint32 _size ,uint32 _kvs )
		{
			kvs = _kvs;
			size = _size;
		}
		
		KVInfo()
		{
			kvs = 0;
			size = 0;
		}
        
        void set(uint32 _size ,uint32 _kvs )
        {
            size = _size;
            kvs = _kvs;
        }

		void clear()
		{
			kvs = 0;
			size = 0;
		}
		
		void append( uint32 _size , uint32 _kvs )
		{
			size += _size;
			kvs += _kvs;
		}
		
		void append( KVInfo other )
		{
			size += other.size;
			kvs += other.kvs;
		}
        
        bool canAppend( KVInfo other )
        {
            if( ( (uint64) size + (uint64) other.size ) >= MAX_UINT_32 )
                return false;
            if( ( (uint64) kvs + (uint64) other.kvs ) >= MAX_UINT_32 )
                return false;
            
            return true;
            
        }

		void remove( uint32 _size , uint32 _kvs )
		{
			size -= _size;
			kvs -= _kvs;
		}
		
		void remove( KVInfo other )
		{
			size -= other.size;
			kvs -= other.kvs;
		}
		
		
		std::string str()
		{
			std::ostringstream o;
			o <<  "( " << au::str( kvs , "kvs" ) << " in " << au::str( size ,"bytes" ) << " )";  
			return o.str();
		}
		
		bool isEmpty()
		{
			return ((kvs==0)&&(size==0));
		}

        void getInfo( std::ostringstream& output)
        {
            au::xml_open(output,"kv_info");
            au::xml_simple( output , "kvs" , kvs );
            au::xml_simple( output , "size" , size );
            au::xml_close(output,"kv_info");
        }
        
	};	
	
	/***********************************************************************
	 FullKVInfo (64 bits) structure to keep information about size and # kvs
	 ***********************************************************************/
	
	struct FullKVInfo
	{
		uint64 size;	// Total size
		uint64 kvs;		// Total number of kvs
		
		
		FullKVInfo(uint32 _size ,uint32 _kvs )
		{
			kvs = _kvs;
			size = _size;
		}
		
		FullKVInfo()
		{
			kvs = 0;
			size = 0;
		}

        void set(uint32 _size ,uint32 _kvs )
        {
            size = _size;
            kvs = _kvs;
        }

        void set(uint64 _size ,uint64 _kvs )
        {
            size = _size;
            kvs = _kvs;
        }
		
		void clear()
		{
			kvs = 0;
			size = 0;
		}
		
#ifdef __LP64__
		void append( uint64 _size , uint64 _kvs )
		{
			size += _size;
			kvs += _kvs;
		}
#endif
		
		void append( FullKVInfo other )
		{
			size += other.size;
			kvs += other.kvs;
		}
		
#ifdef __LP64__
		void remove( uint64 _size , uint64 _kvs )
		{
			size -= _size;
			kvs -= _kvs;
		}
#endif
		
		void remove( FullKVInfo other )
		{
			size -= other.size;
			kvs -= other.kvs;
		}

#pragma mark ----
		
		void append( uint32 _size , uint32 _kvs )
		{
			size += _size;
			kvs += _kvs;
		}
		
		void append( KVInfo other )
		{
			size += other.size;
			kvs += other.kvs;
		}

		void set( KVInfo other )
		{
			size = other.size;
			kvs = other.kvs;
		}
		
		void remove( uint32 _size , uint32 _kvs )
		{
			size -= _size;
			kvs -= _kvs;
		}
		
		void remove( KVInfo other )
		{
			size -= other.size;
			kvs -= other.kvs;
		}
		
		bool fitsInKVInfo()
        {
            if( size >= MAX_UINT_32)
                return false;
            if( kvs >= MAX_UINT_32)
                return false;
            return true;
        }
        
        KVInfo getKVInfo()
        {
            return KVInfo( size , kvs );
        }
		
		std::string str()
		{
			std::ostringstream o;
			o <<  "( " << au::str( kvs , "kvs" ) << " in " << au::str( size ,"bytes" ) << " )";  
			return o.str();
		}
        
		std::string strDetailed()
        {
            return au::str("( %lu kvs [%s] in %lu bytes [%s] )"
                           ,kvs
                           ,au::str(kvs).c_str()
                           ,size
                           ,au::str(size).c_str()
                           );
        }
        
        void getInfo( std::ostringstream& output)
        {
            au::xml_open(output,"kv_info");
            au::xml_simple( output , "kvs" , kvs );
            au::xml_simple( output , "size" , size );
            au::xml_close(output,"kv_info");
        }
        
		bool isEmpty()
		{
			return ((kvs==0)&&(size==0));
		}
		
	};	

    
    bool operator==(const KVFormat & left, const KVFormat & right);
    bool operator!=(const KVFormat & left, const KVFormat & right);

    
	/**
        KVRange keeps information about a particular range of hash-groups
	 */
	
	struct KVRange
    {
        int hg_begin;
        int hg_end;
        
        KVRange();
        KVRange( int _hg_begin , int _hg_end );

        void set( int _hg_begin , int _hg_end );
        
        void setFrom( KVInfo *info );        
        bool isValid();
      
        void getInfo( std::ostringstream& output);
        
        std::string str();
        
        bool overlap( KVRange range );
        
        int getNumHashGroups();
        
        bool includes( KVRange range );
        
        bool contains( int hg );
       
        bool contains(  KVRange range );
        
        // Get the maximum division pattern for this range
        bool isValidForNumDivisions( int num_divisions );
        int getMaxNumDivisions();
        
        // Check if this range is valid for this KVInfo*
        bool check( KVInfo* info );
        
    };
    
    bool operator<(const KVRange & left, const KVRange & right);
    bool operator==(const KVRange & left, const KVRange & right);
    bool operator!=(const KVRange & left, const KVRange & right);

    int divisionForHG( int hg , int num_divisions );
    KVRange rangeForDivision( int pos , int num_divisions );
    
    void clear( KVInfo* info );
    
    // Get the agregation of
    KVInfo selectRange( KVInfo* info , KVRange range );
    
    
	/**
	 Header used in KV-Sets ( Files, Network messages, Operations, etc...)
	 */
	
	struct KVHeader 
	{
		
		// Information about the packet
		// ---------------------------------------------------------------
		
		int magic_number;			// Magic number to make sure reception is correct
		char keyFormat[100];		// Format for the key
		char valueFormat[100];		// Format for the value
        
		KVInfo info;				// Total information in this package ( in all hash-groups )
        KVRange range;              // Range of has-groups used in this file
		
        time_t time;                // Time stamp for this block

		// Specific fields only used in particular operations
		// ---------------------------------------------------------------
		
		int input;				// Input channel
		int num_inputs;			// Total number of inputs

		// Init header
		// ---------------------------------------------------------------
		
		void init( KVFormat format , KVInfo _info )		// Complete init function
		{
			magic_number =  4652783;

			setFormat( format );
			setInfo( _info );
			
			// Default initialization of the hash-group to full-files
			range.set( 0 , KVFILE_NUM_HASHGROUPS );

			// Default init for the input/num_inputs field ( only used in particular operations )
			input = 0 ;
			num_inputs = 0;
            
            // Current time-stamp
            time = ::time(NULL);
		}
        
        void initForTxt( size_t size )
        {
            magic_number =  4652783;
            setFormat( KVFormat("txt","txt") );
            range.set( 0 , KVFILE_NUM_HASHGROUPS );
            
            info.kvs = 0;// In txt data-sets there are not "key-values"
            info.size = size;
            
            input = 0;
            num_inputs=0;
            
            // Current time-stamp
            time = ::time(NULL);
            
        }
        
        bool isTxt()
        {
            return (getKVFormat() == KVFormat("txt","txt") );
        }
        
        bool check_size( size_t total_size )
        {
            if( isTxt() )
                return (total_size == ( sizeof(KVHeader) + info.size ) );
            else
                return (total_size == ( sizeof(KVHeader) + KVFILE_NUM_HASHGROUPS*sizeof(KVInfo) +  info.size ) );
                
        }
        
		void setHashGroups( uint32 _hg_begin , uint32 _hg_end )
		{
            range.set( _hg_begin , _hg_end );
		}
        
		void setFormat( KVFormat format )
		{
			snprintf(keyFormat, 100, "%s", format.keyFormat.c_str());
			snprintf(valueFormat, 100, "%s", format.valueFormat.c_str());
		}
		
		
		// Functions to set of get information from the header
		// ---------------------------------------------------------------
		
		void setInfo( KVInfo _info )
		{
			info = _info;
		}
		
		uint32 getTotalSize()
		{
			// Get the total size of the message including header, hash-group info and data
			return  sizeof(KVHeader) + sizeof(KVInfo)*getNumHashGroups() + info.size;	
		}

		uint32 getNumHashGroups()
		{
            return range.getNumHashGroups();
		}
		
		// Format operations
		// ---------------------------------------------------------------
		
		KVFormat getKVFormat()
		{
			return KVFormat( keyFormat , valueFormat );
		}
		
		
		// Check operations ( magic number and other conditions )
		// ---------------------------------------------------------------
		
		bool check()
		{
			return ( magic_number == 4652783);
		}
		
		bool checkInput()
		{
			if( input >= num_inputs)
				return false;
			
			return true;
		}
		
        std::string str()
        {
            std::ostringstream output;
            output << "KVHeader: " << info.str() << " (" << getKVFormat().str() << ")";
            return output.str();
        }
        
        void getInfo( std::ostringstream &output )
        {
            au::xml_open(output , "kv_header" );
            
            getKVFormat().getInfo( output );
            info.getInfo( output );
            range.getInfo(output);
            
            au::xml_close(output , "kv_header" );
        }
        
	};

    /**
     Structure used to work with contents of KVFiles
     **/
    
    struct KVFile
    {
        KVHeader *header;
        KVInfo *info;
        char *data;
        
        KVFile( char *data)
        {
            init(data);
        }
        
        void init( char * _data )
        {
            header = (KVHeader*) _data;
            info   = (KVInfo*)  (_data + sizeof( KVHeader ) );
            data   = _data + sizeof( KVHeader ) + KVFILE_NUM_HASHGROUPS*sizeof(KVInfo);
        }
        
        size_t offset( int hg )
        {
            size_t offset = 0;
            for ( int i = 0 ; i < hg ; i++ )
                offset += info[i].size;
            return offset;
        }
        
    };    
    

    
	/**
	 A SharedFile from the Process side
	 */
	
	class ProcessSharedFile
	{
	public:
		
		// Pointer to the header
		KVHeader *header;
		
		// Pointer to the info for each hash-group
		KVInfo *info;

		// Pointers to each hash-groups and size
		char ** hg_data;
		
		// Old mechanism to access data of the hash-groups
		char *data;
		int hg;
		size_t offset;
		
		ProcessSharedFile()
		{
			hg = 0;
			hg_data = NULL;
		}
		
		~ProcessSharedFile()
		{
			if( hg_data )
				free(hg_data);
		}
		
		// Set the real pointer to the current data and returns the toal size of this file in the shared memory area
		size_t set( char *_data )
		{
			offset = 0;	// Init the offset of this file
						
			header = (KVHeader*) _data;
			if( !header->check() )
			  LM_X(1,("Error checking the magic number of header in ProcessSharedFile"));

			// Get the number of hash group from header
			uint32 num_hash_groups = header->getNumHashGroups();
			
			info = (KVInfo*) ( _data + sizeof(KVHeader) );
			data = (_data + sizeof(KVHeader) + sizeof(KVInfo)*num_hash_groups);
			
			size_t total_size = sizeof(KVHeader) + sizeof(KVInfo)*num_hash_groups + header->info.size;
			if( total_size != header->getTotalSize() )
			  LM_X(1,("Error checking size of a message inside ProcessSharedFile"));
			
			
			// Pointer and size of every hash-group
			// ------------------------------------------------
			if( hg_data )
				free(hg_data);

			hg_data = (char **) malloc( sizeof(char*) * num_hash_groups );
			
			char * current_data = _data + sizeof(KVHeader) + num_hash_groups * sizeof(KVInfo); // Point to the begining of data
			
			for (uint hg = 0 ; hg < num_hash_groups ; hg++)
			{
				hg_data[hg] = current_data;
				current_data += info[hg].size;
			}
			
			return total_size;
			
		}
		
		size_t getNumKVsInThisHashGroup()
		{
			return info[hg].kvs;
		}
		
	};	

	
	class FileManagerReadItem;
	
	/**
	 A SharedFile from the ProcessAssitant side
	 */
	
	class ProcessAssistantSharedFile
	{
	public:
		
		KVInfo *info;					// Information about key-values ( read directly from file )
		KVInfo *cumulative_info;		// Cumulative information
		
		int input;						// Input of this file ( input channel )
		int num_inputs;					// total number of inputs
		
		std::string fileName;			// fileName of this file

		KVFormat format;
		KVInfo total_info;
		
		ProcessAssistantSharedFile( KVFormat _format , KVInfo _info , int _input  , int _num_inputs ,  std::string _fileName)
		{
			input = _input;
			fileName = _fileName;
			num_inputs = _num_inputs;
			
			format = _format;
			total_info = _info;
			
			info				=  (KVInfo*) malloc( sizeof( KVInfo) * KVFILE_NUM_HASHGROUPS );
			cumulative_info		=  (KVInfo*) malloc( sizeof( KVInfo) * KVFILE_NUM_HASHGROUPS );
		}
		
		~ProcessAssistantSharedFile()
		{
			free( info );
			free( cumulative_info );
		}
		
        engine::SimpleBuffer getSimpleBufferForInfo()
		{
			return engine::SimpleBuffer( (char*) info , sizeof( KVInfo) * KVFILE_NUM_HASHGROUPS );
		}
		
		void setup()
		{
			KVInfo cumulative;
			cumulative.clear();
			for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
			{
				cumulative.append( info[i] );
				cumulative_info[i] = cumulative;
			}
		}
		
		
		// Get the header of this file for a particular range of hash-groups
		
		KVHeader getKVHeader( int hg_begin , int hg_end )
		{
			// Header to be written on shared memory
			KVHeader header;
			header.init( format , total_info );
			
			// Input channel information
			header.input = input;				
			header.num_inputs = num_inputs;
			
			// Information about the number of kvs and size
			header.info = getKVInfo( hg_begin , hg_end);	
			
			// Hash group range we are processing
            header.range.set( hg_begin , hg_end );
			
			if( !header.check() )
			  LM_X(1,("Error cheking magic number of KVHeader"));
			return header;
		}
		
		
		// Operation to get information when processing a subset of hash-groups ( reduce item operation )
		
		KVInfo getKVInfo( int hg_begin , int hg_end )
		{
			KVInfo _info;
			if (hg_begin == 0)
			{
				// Spetial case with the first operation
				_info = cumulative_info[hg_end-1];
			}
			else
			{
				_info.size = cumulative_info[hg_end-1].size - cumulative_info[hg_begin-1].size;
				_info.kvs = cumulative_info[hg_end-1].kvs - cumulative_info[hg_begin-1].kvs;
			}
			
			return _info;
		}
		
		size_t getFileOffset( int hg_begin )
		{
			size_t file_offset = KVFILE_TOTAL_HEADER_SIZE;
			if( hg_begin > 0)
				file_offset += cumulative_info[hg_begin-1].size;
			
			return file_offset;
		}
		
		
	};
	
	
	class ProcessAssistantSharedFileCollection
	{
		
	public:
		
		std::vector<ProcessAssistantSharedFile*> file;
		
		int num_inputs;				// Number of input channels
		int *num_input_files;		// Number of input files per input channels
		int total_num_input_files;	// Total number of files

		FullKVInfo *info_hg;            // Vector with the total info per hash_group ( necessary when organizing reduce );
		
		size_t total_size;
		
		ProcessAssistantSharedFileCollection( const network::WorkerTask & workerTask  )
		{
			// Get the number of inputs & files per input
			int num_inputs = workerTask.input_queue_size();
			total_num_input_files = 0;
			num_input_files = (int*) malloc( sizeof(int) * num_inputs );
			for( int i = 0 ; i < num_inputs ; i++)
			{
				KVFormat format( workerTask.input_queue(i).queue().format().keyformat() , workerTask.input_queue(i).queue().format().valueformat() );
				
				num_input_files[i] = workerTask.input_queue(i).file_size();
				for (int j = 0 ; j < num_input_files[i] ; j++)
				{
					KVInfo info( workerTask.input_queue(i).file(j).info().size() , workerTask.input_queue(i).file(j).info().kvs() );
					file.push_back( new ProcessAssistantSharedFile( format , info , i , num_inputs ,workerTask.input_queue(i).file(j).name() ) );
				}
				total_num_input_files += num_input_files[i];
			}
			
			// Size of each hash-group
			info_hg = (FullKVInfo*) malloc( KVFILE_NUM_HASHGROUPS * sizeof(FullKVInfo) );
			
		}
		
		void setup()
		{
			for (int i = 0 ; i < total_num_input_files ; i++)
				file[i]->setup();
			
			// Compute the total size per hash group	
			total_size = 0;
			for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
			{
				info_hg[hg].clear();
				for (int f = 0 ; f < total_num_input_files ; f++)
					info_hg[hg].append( file[f]->info[hg] );
				
				total_size += info_hg[hg].size;
			}
		}
		
		~ProcessAssistantSharedFileCollection()
		{
			free( num_input_files);
			free( info_hg );
			
			for (size_t i = 0 ; i < file.size() ;i++)
				delete file[i];
			
		}
	};


#pragma mark Monitorization of queue information
	
	typedef struct
	{
		std::string queue;
		std::string format;
		FullKVInfo info;
		int num_files;
	} QueueMonitorInfo;
	
	
#define KV_NODE_SIZE	255
#define KV_NODE_UNASIGNED 4294967295u	// 2^32-1

	
	struct HashGroupOutput {
		
		KVInfo info;		// Basic info of this hg
		uint32 first_node;		// First block with output for this hash gorup
		uint32 last_node;		// Last block with output for this hash gorup
		
		void init()
		{
			info.clear();
			first_node = KV_NODE_UNASIGNED;
			last_node = KV_NODE_UNASIGNED;
		}
		
	};	
	
	/**
	 A channel is the unit of data for a particular server and output
	 */
	
	struct OutputChannel {
		
		KVInfo info;
		HashGroupOutput hg[ KVFILE_NUM_HASHGROUPS ];
		
		void init()
		{
			info.clear();
			for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
				hg[i].init();
		}
	};
	
	/* 
	 Basic unit of storage for output key-values
	 It is used by a particular output channel in a linked-chain way
	 */
	
	struct NodeBuffer {
		uint8 size;					// Size used inside this minibuffer
		char data[KV_NODE_SIZE];	// Real buffer with data
		uint32 next;				// Identifier of the next buffer
		
		void init()
		{
			size = 0;
			next = KV_NODE_UNASIGNED;
		}
		
		inline size_t write( char *_data , size_t _size )
		{
			size_t available = KV_NODE_SIZE - size;
			
			if( _size > available )
			{
				memcpy(data+size, _data, available);
				size = KV_NODE_SIZE;
				return available;
			}
			else
			{
				memcpy(data+size, _data, _size);
				size += _size;
				return _size;
			}
		}
		
		inline bool isFull()
		{
			return ( size == KV_NODE_SIZE );
		}
		
		inline void setNext( uint32 _next )
		{
			next = _next;
		}
		
		inline size_t availableSpace()
		{
			return KV_NODE_SIZE - size;
		}
		
	};
    
    /**
     Structure to save the hash-code of the datainstance
     */
    
    struct KeyValueHash
    {
        size_t key_hash;
        size_t value_hash;
        
        KeyValueHash()
        {
            key_hash = 0;
            value_hash = 0;
        }
    };
	
}

#endif