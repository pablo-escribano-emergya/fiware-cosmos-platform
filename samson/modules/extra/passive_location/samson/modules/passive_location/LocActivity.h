/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_passive_location_LocActivity
#define _H_SAMSON_passive_location_LocActivity


#include <samson/modules/passive_location/LocActivity_base.h>


namespace samson{
namespace passive_location{


	class LocActivity : public LocActivity_base
	{
	public:

        void init()
        {
            dayActivity_length = 0;
            cellId.value = 0;
        }

        int findDayActivity(unsigned int _day)
        {
            for (int i = 0; (i < dayActivity_length); i++)
            {
                if (_day == dayActivity[i].day.value)
                {
                    return (i);
                }
            }
            return (-1);
        }
	};


} // end of namespace samson
} // end of namespace passive_location

#endif
