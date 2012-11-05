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
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_getUsersFromServer
#define _H_SAMSON_url_getUsersFromServer


#include <samson/module/samson.h>


/************************************************************
reduce getUsersFromServer
{
	in system.UInt system.UInt           # Server(id) - User(id)
	out system.UInt url.UserCountVector  # Server(id) - Users(Vector, with count)

	helpLine       "Transform Server-User pairs into Server-UsersCountVector pairs (count may be important to spread categories)"
}
**************************************************************/

namespace samson{
namespace url{


	class getUsersFromServer : public samson::Reduce
	{

	public:
		samson::system::UInt userId;
		samson::system::UInt serverId;

		UserCount userCount;


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			bool first = true;
			size_t userIdPrev = 0;
			int count = 0;
			UserCountVector users;

			serverId.parse(inputs[0].kvs[0]->key);

			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				userId.parse(inputs[0].kvs[i]->value);
				//OLM_T(LMT_User06,("For serverId:%d  user:%d", serverId.value, userId.value));
				if (first || (userId == userIdPrev))
				{
					count++;
					//OLM_T(LMT_User06,("\t(%d) Increment count to %d", serverId.value, count));
					first = false;
				}
				else
				{
					//OLM_T(LMT_User06,("(%d) It's a new user, store %d with count:%d", serverId.value, userIdPrev, count));
					userCount.user = userIdPrev;
					userCount.count = count;
					users.usersCountAdd()->copyFrom(&userCount);
					count = 1;
				}
				userIdPrev = userId.value;
			}
			//OLM_T(LMT_User06,("(%d) It's the last user, store %d with count:%d", serverId.value, userIdPrev, count));
			userCount.user = userIdPrev;
			userCount.count = count;
			users.usersCountAdd()->copyFrom(&userCount);
			users.countRef = inputs[0].num_kvs;

			writer->emit(0, &serverId, &users);
		}


	};


} // end of namespace samson
} // end of namespace url

#endif
