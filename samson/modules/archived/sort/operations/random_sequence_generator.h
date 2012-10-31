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

#ifndef _H_SAMSON_sort_random_sequence_generator
#define _H_SAMSON_sort_random_sequence_generator


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>
#include <samson/modules/system/Void.h>

/**************************************************************************
generator random_sequence_generator
{
	out system.UInt system.Void

	helpLine "Generates a random sequence of UInt-Void key-values, as many as selected with the environment variable 'test.number_of_samples'"
}
***************************************************************************/

namespace samson{
namespace sort{


	class random_sequence_generator : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
            size_t num_samples = environment->get( "test.number_of_samples", 1000000000);

            samson::system::UInt val1;
            samson::system::UInt val2;
            samson::system::Void dummy;

            for (size_t i = 0; (i < num_samples); i++)
            {
                    val1.value = rand();
                    val2.value = (val1.value << 31) + rand();

                    writer->emit(0, &val2, &dummy);
            }
		}


	};


} // end of namespace samson
} // end of namespace sort

#endif
