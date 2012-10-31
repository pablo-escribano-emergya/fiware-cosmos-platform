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

#ifndef _H_SAMSON_example_generator
#define _H_SAMSON_example_generator


#include <samson/module/samson.h>

#include <samson/modules/system/UInt.h>
#include <iostream>
#include <sstream>


/* ****************************************************************************
 *
 * TRACES Definitions
 */


namespace samson{
namespace example{

	class generator : public samson::Generator
	{

	  bool active;

	public:

	        void setup( int worker , int num_workers, int process , int num_processes )
		{
		  if( ( worker == 0 ) && ( process == 0 ) )
		    active = true;
		  else
		    active = false;
		}

		void run( samson::KVWriter *writer )
		{

		  if(!active)
		    return;

		  // Recover the number of samples from the environment variables
		  size_t num_samples = environment->get( "example.samples" ,  10 );

		  // Trace to monitor the operation
		  // tracer->trace_print( 0 , "example.generator init with %d samples" , num_samples );

		  OLM_T( LMT_User01 , (" example.generator init with %d samples", num_samples));


		  OLM_M(("Example of simple message..."));
		  OLM_D(("Example of a debug message..."));		  
		  OLM_E(("Example of an error message..."));
		  OLM_W(("Example of a Warning message..."));


		  samson::system::UInt a;
		  samson::system::UInt b;
				

			for (size_t i = 0 ; i < num_samples ; i++)
			{
			  if( ( i%10000 ) == 0)
			    operationController->reportProgress( (double) i / (double) num_samples );


				a = i;
				b = i;

				writer->emit( 0 , &a , &b );
			}
		
		  OLM_T( LMT_User01 , ("example.generator finish with %d samples", num_samples));

		}



	};


} // end of namespace samson
} // end of namespace example

#endif
