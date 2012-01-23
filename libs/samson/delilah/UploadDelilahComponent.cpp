

#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/Buffer.h"							// samson::Buffer
#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...


#include "UploadDelilahComponent.h"			// Own interface

namespace samson
{

	// Function used by a parallel thread to process new uploads to be send to workers 
	// May use compression algorithms ( that is the reason to put in this task in a thread )
	void* processUploadPacketData( void *p );
	
	
#pragma mark -
	
	DelilahUploadComponent::DelilahUploadComponent( std::vector<std::string> &fileNames , std::string _queue , bool _compression, int _max_num_threads ) : DelilahComponent(DelilahComponent::load) , fileSet( fileNames ) 
	{
		
		// Queue name 
		queue = _queue;

		// Information about the compression	
		compression = _compression;
		
		uploadedSize = 0;
		processedSize = 0;
		totalSize = 0;	
		uploadedCompressedSize = 0;
		
		num_threads = 0;
		max_num_threads = _max_num_threads;
		
		// Compute the total size for all the files
		for ( size_t i =  0 ; i < fileNames.size() ; i++)
			totalSize += au::sizeOfFile( fileNames[i] );

		// Init counters of created and confirmed files
		num_files = 0;
		num_confirmed_files = 0;

		
		// Response from the controller initially NULL
		upload_data_finish = NULL;

		// Unninitialized status
		status = uninitialized;
        
		if( totalSize == 0)
		{
			status = finish_with_error;
            error.set("Not data to upload");
		}
        
        if( fileNames.size() == 1 )
            setConcept(au::str("Uploading %s (%s) to queue %s", fileNames[0].c_str() , au::str( fileSet.getTotalSize(),"bytes" ).c_str() ,  queue.c_str() ));
        else
            setConcept(au::str("Uploading %s to queue %s", au::str( fileSet.getTotalSize(),"bytes" ).c_str() ,  queue.c_str() ));
        
	}	
	
	DelilahUploadComponent::~DelilahUploadComponent()
	{
		if ( upload_data_finish ) 
			delete upload_data_finish;
	}
	
	void* runThreadDelilahLoadDataProcess(void *p)
	{
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());
        
		DelilahUploadComponent *d = ((DelilahUploadComponent*)p);
		d->_run();
		return NULL;
	}

	
	void DelilahUploadComponent::run()
	{
		
		if( status == finish_with_error )
		{
            setComponentFinished();
			return; // Nothing else to do
		}
		
		if( status != uninitialized )
			LM_X(1,("Unexpected status running an upload data process"));
		
		// Initial status ( We will send the first message to controller latter )
		status = waiting_controller_init_response;
		
		// Send the message to the controller
		Packet *p = new Packet(Message::UploadDataInit);
		
		p->message->set_delilah_id( id );
		samson::network::UploadDataInit *upload_data_init = p->message->mutable_upload_data_init();
		upload_data_init->set_queue( queue );
		delilah->network->sendToController( p );
		
		
		// Set the number of workers
		num_workers = delilah->network->getNumWorkers();
		
	}
	
	
	struct UploadPacketData {
		Packet *p;											// Packet prepared to upload
		network::UploadDataFile *loadDataFile;				// Message inside the packet to complete information about upload 
		int worker;											// Worker to send this packet
		Delilah* delilah;									// Delilah client pointer to use the network element
		DelilahUploadComponent* uploadDataProcess;		// Pointer to the global upload to get information about compression and report finish
	};
	
	
	void DelilahUploadComponent::_run()
	{
		
		while( !fileSet.isFinish() )
		{
			// No more things to do
			if( status == finish_with_error )
				return;
			
			// Wait if memory is not released
			while( ( engine::MemoryManager::shared()->getMemoryUsageByTag( MemoryOutputNetwork ) > 1.0 ) || ( num_threads >= max_num_threads ) )
            {
                // Bloques for memory of thread puposes
				sleep(1);
				LM_M(("sleeps with (engine::MemoryManager::shared()->getMemoryUsageByTag( MemoryOutputNetwork ):%f > 1.0) or (num_threads:%d >= max_num_threads:%d)", engine::MemoryManager::shared()->getMemoryUsageByTag( MemoryOutputNetwork ), num_threads, max_num_threads));
            }
			
			// Create a buffer
            size_t load_size_buffer = samson::SamsonSetup::shared()->getUInt64("load.buffer_size");
            engine::Buffer *b = engine::MemoryManager::shared()->newBuffer( "Loading buffer" , load_size_buffer , MemoryOutputNetwork );
			
            // Fill the header first
            b->skipWrite( sizeof(KVHeader) );
            
			// Fill the buffer with the contents from the file
			fileSet.fill( b );

            // Set the header
            KVHeader* _header = (KVHeader*) b->getData();
            _header->initForTxt( b->getSize() - sizeof( KVHeader ) );
			
            
            // Insert into the list of pending elements
			size_t file_id = num_files++;
			
			// Activate the finish flag if necessary before sending the packet ( so at the reception the flag is activated )
			if( fileSet.isFinish() )
			{
				LM_T(LmtDisk, ("Waiting last file confirmation"));
				lock.lock();
				status = waiting_file_upload_confirmations;	// Waiting at least the confirmation of this last file
				lock.unlock();
			}
			
			// Send to the rigth worker
			Packet *p = new Packet(Message::UploadDataFile);
			p->buffer = b;	// Add the buffer to the packet

			// Set message fields
			network::UploadDataFile *loadDataFile = p->message->mutable_upload_data_file();	

			p->message->set_delilah_id( id );				
			loadDataFile->set_load_id( load_id );		// load id operation at the controller
			loadDataFile->set_file_id( file_id );		// File id
			
			// Send the packet
			std::ostringstream message;
			message << "[ " << id << " ] < Buffer " << file_id << " > Created with size: " << au::str( b->getSize() , "B" );
			delilah->showTrace( message.str() );
			LM_T(LmtDisk, ("%s", message.str().c_str()));

			lock.lock();

			// Thread to process this upload. It may use compression algorithms before sending to the worker
			
			UploadPacketData *data  = (UploadPacketData*)malloc( sizeof(UploadPacketData) );
			
			data->p = p;
			data->loadDataFile = loadDataFile;
			data->delilah = delilah;
			data->worker = delilah->getNextWorker();
			data->uploadDataProcess = this;
			
			pthread_t t;
			pthread_create(&t, NULL, processUploadPacketData, (void*) data);
			num_threads++;
			
			lock.unlock();

			
			
		}
		
        // Show a message on screen
		{
			std::ostringstream message;
			message << "[ " << id << " ] All input data locally processed";
			delilah->showTrace( message.str() );
			LM_M(("%s", message.str().c_str()));
		}
		
		
	}
	
	void* processUploadPacketData( void *p )
	{
        // Free resources automatically when this thread finish
        pthread_detach(pthread_self());
        
		UploadPacketData * pd = (UploadPacketData*)p;
		
		size_t original_size = pd->p->buffer->getSize();
		size_t compress_size = pd->p->buffer->getSize();
		
		size_t file_id = pd->p->message->upload_data_file().file_id();
		size_t id = pd->uploadDataProcess->id;
		
		// Compress the buffer
		if ( pd->uploadDataProcess->compression )
		{
            LM_X(1,("Not implemented!!"));
            /*
             ??
             */
			
		}
		else
			pd->loadDataFile->set_file_ext("txt" );

		
		// Complete information in the upload message
		pd->loadDataFile->set_file_size( compress_size );
		
		// Notify that the thread finished
		pd->uploadDataProcess->finishCompressionThread(original_size);
		
		// Send the packet
		{
			std::ostringstream message;
			message << "[ " << id << " ] < Buffer " << file_id << " > Sending ( Compresed Size: " << au::str(compress_size) << " Original Size: " << au::str(original_size) << ")";
			pd->delilah->showTrace( message.str() );
			LM_T(LmtDisk, ("%s", message.str().c_str()));
		}
		
		pd->delilah->network->sendToWorker( pd->worker, pd->p);
		LM_T(LmtDisk, ("Packet sent to worker"));
		
		// Free allocated input parameter
		free( p );
		
		return NULL;
	}
	
	
	void DelilahUploadComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		if( status == finish_with_error )
			return;	// No more processign of incoming packets
		
		lock.lock();

		if (msgCode == Message::UploadDataInitResponse )
		{
			// Only valid in this mode
			if( status != waiting_controller_init_response)
				LM_X(1,("Unexpected status in an upload data process"));

			// Get the identifier at the controller for this operation
			load_id = packet->message->upload_data_init_response().load_id();

			// At this level, errors are not expected at the moment
			
			if( packet->message->upload_data_init_response().has_error() )
			{
				error.set(  packet->message->upload_data_init_response().error().message() );
				status = finish_with_error;
				
				lock.unlock();
				
                setComponentFinished();
				
				lock.lock();
				
			}
			else
			{
				
				// Chage the status so the background thread starts sending data
				status = sending_files_to_workers;

				upload_data_finish = new samson::network::UploadDataFinish();
				upload_data_finish->set_load_id( load_id );		// load id operation at the controller
				upload_data_finish->set_queue( queue );			// Set the queue to send data
				
				// Create the thread for this load process
				pthread_create(&t, NULL, runThreadDelilahLoadDataProcess, this);
			}
			
			
		}
		else if (msgCode == Message::UploadDataFileResponse )
		{
			//size_t file_id = packet->message->upload_data_response().upload_data().file_id();
			
			if( packet->message->upload_data_file_response().has_error() )
			{
				error.set(  packet->message->upload_data_finish_response().error().message() );
				status = finish_with_error;
			}
			
			// Add the generated file to the packet prepared for the final confirmation
			upload_data_finish->add_files()->CopyFrom( packet->message->upload_data_file_response().file() );
			
			// Update the uploaded size
			uploadedSize +=  packet->message->upload_data_file_response().query().file_size();

            if( totalSize > 0 )
                setProgress((double) uploadedSize / (double) totalSize );
            
			// Get the uploaded file_id 
			size_t file_id = packet->message->upload_data_file_response().query().file_id();
			
			std::ostringstream output;
			output << "[ " << id << " ] < Buffer " << file_id << " > Received upload confirmation";
			delilah->showTrace( output.str() );

			// Increate the number of confirmed files
			num_confirmed_files++;

			// Check the upload operation is complete to send the final message
			if( status == waiting_file_upload_confirmations )
				if ( num_files == num_confirmed_files)
				{
					// Change the status to waiting the final confirmation message
					status = waiting_controller_finish_response;
					
					// Send the final packet to the controller notifying about the loading process
					Packet *p = new Packet( Message::UploadDataFinish );
					network::UploadDataFinish *_upload_data_finish	= p->message->mutable_upload_data_finish();
					_upload_data_finish->CopyFrom( *upload_data_finish );

					// Set the general id for delilah
					p->message->set_delilah_id( id );

					// Set the final files used in this upload ( usign information from the initial message )
					
					delilah->network->sendToController( p );
				}

			
			
		} else if (msgCode == Message::UploadDataFinishResponse )
		{
			
			if( status != waiting_controller_finish_response )
				LM_X(1,("Unexpected status in an upload data process"));


			if( packet->message->upload_data_finish_response().has_error() )
			{
				error.set(  packet->message->upload_data_finish_response().error().message() );
				status = finish_with_error;
			}
			else
				status = finish;

            setComponentFinished();
			
			std::ostringstream output;
			output << "[ " << id << " ] Received upload data confirmation from controller\n";
			output << getStatus();
			delilah->showTrace( output.str() );
			
			
		}
		else
		{
			//??
		}

		lock.unlock();		
		
	}
	
	
	void DelilahUploadComponent::finishCompressionThread( size_t process_size )		
	{
		lock.lock();
		processedSize += process_size;
		num_threads--;
		lock.unlock();
	}
	

	
	std::string DelilahUploadComponent::showProgress( std::string title,  size_t size )
	{
		std::ostringstream output;

		int p;
		if( size > 0)
			p = ( (double) size / (double) totalSize ) * 100;
		else
			p = 0;
		
		size_t r;

		
        int seconds = cronometer.getSeconds();
        
		if ( seconds > 0 )
			r = ((double) size * 8.0 / (double) seconds);
		else
			r = 0;
		
		size_t r2 = r / num_workers;
		output << " [ "<< title << " " << au::str( size , "B" ) << " " << p << "% "; 
		output << au::str( r , "bps" ) << "  " << au::str( r2 , "bps/w" ) << "  ]";
		
		return output.str();
	}
	
    std::string DelilahUploadComponent::getShortStatus()
    {
		std::ostringstream output;
				
		output << "Uploading " << au::str( totalSize ,"B" ) << " to queue: " << queue << " ( Status ";
        
		switch (status) {
			case uninitialized:
				output << "Uninitialized";
				break;
			case waiting_controller_init_response:
				output << "Waiting controller response to our init message";
				break;
			case sending_files_to_workers:
				output << "Sending data";
				break;
			case waiting_file_upload_confirmations:
				output << "Waiting confirmation of received data";
				break;
			case waiting_controller_finish_response:
				output << "Waiting final message confirmation from controller";
				break;
			case finish_with_error:
				output << "ERROR: " << error.getMessage();
				break;
			case finish:
				output << "Finished correctly";
				break;
		}
		output << " )";
                
		return output.str();        
    }
    
	std::string DelilahUploadComponent::getStatus()
	{
		
		std::ostringstream output;
		
		
		output << "[ "<< id << " ] Uploading " << au::str( totalSize ,"B" ) << " to queue: " << queue << " ( Status ";

		switch (status) {
			case uninitialized:
				output << "Uninitialized";
				break;
			case waiting_controller_init_response:
				output << "Waiting controller response to our init message";
				break;
			case sending_files_to_workers:
				output << "Sending data";
				break;
			case waiting_file_upload_confirmations:
				output << "Waiting confirmation of received data";
				break;
			case waiting_controller_finish_response:
				output << "Waiting final message confirmation from controller";
				break;
			case finish_with_error:
				output << "ERROR: " << error.getMessage();
				break;
			case finish:
				output << "Finished correctly";
				break;
		}
		output << " )";

        
        // General status
        output << "\n\tConfirmed " << num_confirmed_files << " buffers of " << num_files;
        
        // Memory and process
        output << "\n\tParalel processes: " << num_threads << " / " << max_num_threads;
        output << "\n\tOutput memory usage " << au::percentage_string( engine::MemoryManager::shared()->getMemoryUsageByTag( MemoryOutputNetwork  ) );
        output << " of " << au::str( engine::MemoryManager::shared()->getMemory() , "B" );
        
        // Status of the file source
		output << "\n\tFile source: " << fileSet.getStatus();
        
		if( status == sending_files_to_workers )
		{
			output << " [ " << num_threads << " threads compressing data ]";

			if( uploadedSize > 0)
			{
                int seconds = cronometer.getSeconds();
				size_t pending_secs =  ( totalSize - uploadedSize ) * seconds / uploadedSize;
				output << "[ETA " << au::time_string( pending_secs ) << "] ";
			}
		
			output <<"\n\tProgress: ";

			output <<  showProgress( "Upload" , uploadedSize );
			output << " ";
			output <<  showProgress( "Processed" , processedSize );
			
		}


		return output.str();
	}
	
}