#ifndef _H_BLOCK
#define _H_BLOCK

/* ****************************************************************************
 *
 * FILE                      Block.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Basic unit of data for stream processing
 * A block is a unit of data possibly in memory with replication of disk
 *
 */
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "au/containers/simple_map.h"                         // au::simple_map

#include "engine/MemoryManager.h"

#include "samson/common/KVHeader.h"                   
#include "samson/common/KVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Object.h"                          // engien::EngineListener
#include "engine/Object.h"                          // engine::Object

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct

#include "BlockInfo.h"                              // struct BlockInfo

#include <set>

namespace samson {
    namespace stream
    {
        
        class BlockList;
        
        /**
         Main class to hold a block in memory
         */
        
        typedef struct BlockLookupRecord
        {
            char* keyP;
        } BlockLookupRecord;

        typedef struct BlockHashLookupRecord
        {
            size_t  startIx;
            size_t  endIx;
        } BlockHashLookupRecord;

        
        class Block;
        class BlockLookupList
        {
            
            BlockLookupRecord*      head;
            size_t                  size;
            BlockHashLookupRecord*  hashInfo;
            KVFormat                kvFormat;
            
        public:

            au::ErrorManager error;

            BlockLookupList( Block* _block );
            ~BlockLookupList();
            
            std::string lookup(const char* key, std::string outputFormat);
            
        };

        class Block :  public engine::Object
        {
            
            friend class BlockManager;  
            friend class BlockList;
            friend class PopQueueTask;
            friend class BlockBreakQueueTask;
            friend class BlockReader;
            friend class Queue;
            friend class StreamOutQueueTask;
            friend class BlockLookupList;
            
            // Identifiers of this block
            size_t worker_id;               // Identifier of the worker in the cluster
            size_t id;                      // Identifier of the block ( in this node )
            
            // Buffer ( if it is on memory )
            engine::Buffer *buffer;         // Buffer of data if content of this block is on memory
            
            KVHeader* header;               // Always on memory copy of the header
            
            size_t size;                    // Size of the buffer ( Not that buffer is NULL if content is not on memory )

            std::set< BlockList* > lists;   // List where this block is contained
            
            // Lock up table to quick access to key-values from REST interface
            au::Token token_lookupList;
            BlockLookupList* lookupList;

            typedef enum
            {
                on_memory,      // Initial state
                writing,        // A write order has been given
                ready,          // On disk and memory
                on_disk,        // Just on disk ( memory has been freed )
                reading         // Reading from disk to recover at memory
            } BlockState;

            BlockState state;              // State disk-memory
            
            au::Cronometer cronometer;       // Creation time of this block
            
        public:
            
            // Internal information used in the sort
            size_t min_task;
            int max_priority;
            size_t compare_time;   // Time since last update ( the lower the better )
            size_t order;             // Order inside the queue ( in case of same compare_time )
            
        private:
            
            // Constructor only visible in a BlockList
            Block( engine::Buffer *buffer  );
            Block( size_t _worker_id , size_t _id , size_t _size , KVHeader* _header );

        public:

            ~Block();

            // Update sort information
            void update_sort_information();
            
            // Lookup for a particular queue ( lookupList is created if necessary )
            std::string  lookup(const char* key, std::string outputFormat);

            // Set and Get priority ( manual ordering if blocks are not assigned to tasks )
            void setPriority( int _priority );
            int getPriority();
                        
            // Check if this block is not in any list anymore
            int getNumberOfLists();

            // Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
            int canBeRemoved();
                
            // Get file name for this block
            std::string getFileName();
            
        private:
            
            // Send notifications to write and read 
            void write();
            void read();
            
            // Release memory ( only if saved to disk previously )
            void freeBlock();

            // Get the operations
            ::engine::DiskOperation* getWriteOperation();
            ::engine::DiskOperation* getReadOperation();
            
            // Notifications
            void notify( engine::Notification* notification );
            
        public:

            // Checking each state
            bool isOnDisk();
            bool isWriting();
            bool isReading();
            bool isOnMemory();
            bool isReady();

            // Checking if content is on memory and/or on disk
            bool isContentOnMemory();
            bool isContentOnDisk();

            // Checking if content is locked ( contained in a lock - block list )
            bool isLockedInMemory();

            // Getting information
            size_t getSize();
            size_t getSizeOnMemory();
            size_t getSizeOnDisk();
            KVFormat getKVFormat();
            const char* getState();
            size_t getId();
 
            // Working with KVRanges
            KVRange getKVRange();
            bool isNecessaryForKVRange( KVRange range );
            
            // Get information about this block
            void update( BlockInfo &block_info );
            
            // Debug string    
            std::string str();

            // Xml version of the info
            void getInfo( std::ostringstream& output);

            // Collection-like monitorization information
            void fill( samson::network::CollectionRecord* record , Visualization* visualization );
            
            // Accessing information
            KVInfo* getKVInfo();
            char *getData();
            
            KVHeader getHeader()
            {
                return *header;
            }
            
        };
            
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif
