
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_fiware_pos_PosAndUsers
#define _H_SAMSON_fiware_pos_PosAndUsers


#include <samson/modules/fiware_pos/PosAndUsers_base.h>


namespace samson{
namespace fiware_pos{


	class PosAndUsers : public PosAndUsers_base
	{
	  public:
	    void Init() {
	      users.Init();
	      position.posX.value = 0;
	      position.posY.value = 0;
	    }
	};


} // end of namespace samson
} // end of namespace fiware_pos

#endif
