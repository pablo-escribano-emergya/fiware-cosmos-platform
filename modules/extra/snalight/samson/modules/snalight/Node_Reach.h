
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_snalight_Node_Reach
#define _H_SAMSON_snalight_Node_Reach


#include <samson/modules/snalight/Node_Reach_base.h>


namespace samson{
namespace snalight{


	class Node_Reach : public Node_Reach_base
	{
	public:
		bool isExtern(){
		  return (flags.value & samson::snalight::Node::NODE_FLAG_EXTERN);
		}

	};


} // end of namespace samson
} // end of namespace snalight

#endif