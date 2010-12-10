#ifndef _H_DELILAH_CLIENT
#define _H_DELILAH_CLIENT

#include "Message.h"		// ss::Message

namespace ss
{
	
	class Packet;
	class DelilahUploadDataProcess;
	
	class DelilahClient
	{
	public:
		
		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

		// Quit 
		virtual void quit()=0;

		// Notifications for a loadData process
		virtual void loadDataConfirmation( DelilahUploadDataProcess *process )=0;
		
		// Write something on screen
		virtual void showMessage(std::string message)
		{
		}
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t ){
		}
		
		
		
		
	};
}

#endif
