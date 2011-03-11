#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <string>
#include <sstream>
#include "Error.h"			// au::Error

/**
 
 ProcessItem
 
	item inside the ProcessManager
 
	An item is a task to do without any IO operation in the middle.
	It only requires processing time
 */

namespace ss {

	
	class ProcessItem;
	class ProcessManager;
	
	/**
	 Delegate interface to receive notifications about finished ProcessItems
	 */
	
	class ProcessManagerDelegate
	{
		
	public:
		virtual void notifyFinishProcess( ProcessItem * item ) = 0;
		
	};
	
	
	class ProcessItem  
	{

	public:
		
		// Pointer to the process manager to notify that we are finished
		ProcessManager *processManager;

	private:
		
		// Delegate to notify when finishing
		ProcessManagerDelegate * delegate;

		std::string status_letter;
		std::string status;
		
	protected:
		
		double progress;		// Progress of the operation ( if internally reported somehow )
		
	public:
		
		au::Error error;		// Error management

		
		typedef enum	
		{
			data_generator,		// Any operation that generates data ( halt if output memory is not available )
			pure_process		// Any operation with no output that only process data ( necer halt )
			
		} ProcessManagerItemType;
		
		ProcessManagerItemType type;
		
	public:

		size_t component;		// Used by clients to indentify the component inside delegate to receive notiffication
		size_t tag;				// Used by clients to identify element inside delegate
		size_t sub_tag;			// Used by clients to identify sub-element inside delegate

		// Constructor with or without a delegate
		
		ProcessItem( ProcessManagerItemType type );
		
		// Status management
		
		void setStatus(std::string _status);
		void setStatusLetter( std::string _status_letter);
		std::string getStatus();
		
		// Assign the delegate begore constructor
		void setProcessManagerDelegate( ProcessManagerDelegate * _delegate );
		
		// Notify delegate if any
		void notifyFinishToDelegate();		
		
		// What to do when processor is available
		virtual void run()=0;

		// Function to create a new thread and run "run" in background
		void runInBackground();
		
		
	};
	

}

#endif
