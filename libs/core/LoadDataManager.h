#ifndef _H_LOAD_DATA_MANAGER
#define _H_LOAD_DATA_MANAGER

#include <string>
#include "au_map.h"						// au::map
#include "Lock.h"						// au::Lock
#include "DiskManagerDelegate.h"		// ss::DiskManagerDelegate
#include "ObjectWithStatus.h"			// ss::getStatusFromArray(.)
#include "samson.pb.h"					// ss::network:...

namespace ss {

	class Buffer;
	class SamsonWorker;
	class LoadDataManager;

	
	class DataManagerItem
	{
		
	public:

		int fromIdentifier;						// Dalilah identifier to send responses
		
		Buffer *buffer;							// Buffer allocated for this task
		
		LoadDataManager *dataManager;			// Pointer to the data manager
		
		DataManagerItem( int _fromIdentifier , LoadDataManager *dataManager );
		
		virtual ~DataManagerItem(){}
		
	};
	
	
	class UploadItem : public DataManagerItem
	{

	public:
		
		network::UploadData uploadData;		// Information about the request
		
		std::string fileName;				// Filename fo the new upload
		size_t size;						// Size of the uploaded file
		
		UploadItem( int _fromIdentifier , LoadDataManager *dataManager, const network::UploadData &_uploadData ,  Buffer * buffer );
	
		size_t submitToFileManager();
		void sendResponse( bool error , std::string error_message );
		
		static std::string newFileName()
		{
			std::ostringstream fileName;
			fileName << "/tmp/file_" << rand()%1000 << rand()%1000;
			return fileName.str();
		}
		
		
	};	
	
	
	/**
	 Item requested to be downloaded
	 */
	
	class DownloadItem : public DataManagerItem
	{
		
	public:
		
		network::DownloadData downloadData;		// Information about the request
		
		
		DownloadItem( int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadData &_downloadData);		
		~DownloadItem();

		size_t submitToFileManager();
		void sendResponse( bool error , std::string error_message );
		
	};
	
	
	class LoadDataManager : public FileManagerDelegate
	{
		friend class UploadItem;
		friend class DownloadItem;
		
		
		SamsonWorker *worker;

		au::Lock lock;
		
		au::map<size_t,UploadItem> uploadItem;		// Items to be uploded from dalilahs
		
		au::map<size_t,DownloadItem> downloadItem;	// Items to be downloaded to dalilahs
		
	public:
		
		LoadDataManager( SamsonWorker *_worker )
		{
			worker = _worker;
		}
		
		// Add item to upload data
		void addUploadItem( int fromIdentifier, const network::UploadData &uploadData , Buffer * buffer  );
		
		// add item to download data
		void addDownloadItem( int fromIdentifier, const network::DownloadData &downloadData );
		
		// Disk Manager notifications
		virtual void fileManagerNotifyFinish(size_t id, bool success);

		std::string getStatus()
		{
			return "No status for Load Data Manager...";
		}

		
	};

}

#endif