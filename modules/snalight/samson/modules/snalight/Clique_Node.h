
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_snalight_Clique_Node
#define _H_SAMSON_snalight_Clique_Node


#include <samson/modules/snalight/Clique_Node_base.h>


namespace samson{
namespace snalight{


	class Clique_Node : public Clique_Node_base
	{
	public:

		void sortByWeight()
		{
			// Simple sort by weight
			for (int i = 0 ; i < links_length ; i++)
				for (int j = i+1 ; j < links_length ; j++)
				{
					if ( links[i].weight.value < links[j].weight.value )
					{

						Clique_Link tmp;
						tmp.copyFrom(&links[i]);
						links[i].copyFrom(&links[j]);
						links[j].copyFrom(&tmp);
					}

				}

		}
	};


} // end of namespace samson
} // end of namespace snalight

#endif
