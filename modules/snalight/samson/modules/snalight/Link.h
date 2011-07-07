
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_snalight_Link
#define _H_SAMSON_snalight_Link


#include <samson/modules/snalight/Link_base.h>


namespace samson{
namespace snalight{


	class Link : public Link_base
	{

	public:
                double getWeight()
                {
                        return (double)weight.value / 1000.0;
                }

                void setWeight( double value )
                {
                        weight.value =  (int)(value*1000.0);

                }

		static int double_2_intScaled( double value)
		{
			return  (int) (value*1000.0);
		}

		static double intScaled_2_double (int value)
		{
			return (double)value / 1000.0;
		}

	};


} // end of namespace samson
} // end of namespace snalight

#endif
