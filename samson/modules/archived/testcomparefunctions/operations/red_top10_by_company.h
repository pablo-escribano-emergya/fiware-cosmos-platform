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

#ifndef _H_SAMSON_testcomparefunctions_red_top10_by_company
#define _H_SAMSON_testcomparefunctions_red_top10_by_company


#include <samson/module/samson.h>


namespace samson{
namespace testcomparefunctions{


	class red_top10_by_company : public samson::Reduce
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt testcomparefunctions.User  compareValueFunction=compare_by_expense_rev
output: system.UInt testcomparefunctions.User

helpLine: For every company, selects top10 expesense consumers
#endif // de INFO_COMMENT

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif
// Max line to be output
#define MAX_STR_LEN 2048
    char line[MAX_STR_LEN];

    		void init( TXTWriter *writer)
		{
			OLM_T(LMT_User06, ("class red_top10_by_company : public samson::Reduce: init()\n"));
			writer->emit("Hello, world!\n");
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			system::UInt companyId;
			User user;
			OLM_T(LMT_User06, ("Starts with inputs[0].num_kvs:%d\n", inputs[0].num_kvs));
			if (inputs[0].num_kvs < 1)
			{
				OLM_E(("No users for this companies group\n"));
				return;
			}

			for (int i = 0; (i < 10); i++)
			{
				companyId.parse(inputs[0].kvs[i]->key);
				user.parse(inputs[0].kvs[i]->value);

				OLM_T(LMT_User06,("For companyId:%lu, emit userID:%d, expense:%lu\n", companyId.value, user.id.value, user.expense.value));

				writer->emit(0, &companyId, &user);
			}
		}

    		void finish(samson::KVWriter *writer)
		{
			OLM_T(LMT_User06, ("class red_top10_by_company : public samson::Reduce: finish()\n"));
		}
	};


} // end of namespace samson
} // end of namespace testcomparefunctions

#endif
