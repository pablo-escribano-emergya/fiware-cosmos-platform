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

#ifndef _H_SAMSON_sna_fake_cdrs
#define _H_SAMSON_sna_fake_cdrs


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class fake_cdrs : public samson::Generator
	{
		struct tm timeExpanded;


	public:


		void run( samson::KVWriter *writer )
		{
			  samson::system::UInt id;
			  CDR cdr;
			  int num_nodes = 1000;
			  int num_links_per_node = 30;

			  for (int i = 0 ; i < num_nodes ; i++)
			  {
			    for (int j = 0 ; j < num_links_per_node ; j++)
			    {
			      id.value = i+1;

			      //Get a different link

			      do{
			        cdr.node = (rand()%num_nodes)+1;
			      } while( cdr.node == id.value );  //Make sure we are not autolinked

			      for (int k = 0 ; k < rand()%10 ; k++)
			      {
			        timeExpanded.tm_mday  = rand()%30 + 1;
			        timeExpanded.tm_mon  = rand()%12;
			        timeExpanded.tm_year = 111;

			        timeExpanded.tm_hour = rand()%24;
			        timeExpanded.tm_min = rand()%60;
			        timeExpanded.tm_sec = rand()%60;

			        cdr.time.getTimeFromCalendar(&timeExpanded);

			        cdr.duration = rand()%100;

			        writer->emit(0, &id, &cdr);

			        if( (rand()%10) <= 2)
			          writer->emit(0, &id, &cdr);  //Duplicated


			      }
			    }
			  }
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif
