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

#ifndef _H_SAMSON_sna_communities_unique_id_out
#define _H_SAMSON_sna_communities_unique_id_out


#include <samson/module/samson.h>
#include <set>
#include <iostream>
#include "sna_environment_parameters.h"




namespace samson{
namespace sna{


	class communities_unique_id_out : public samson::ParserOut
	{

	std::string sep;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:
		void init(samson::KVWriter *writer){
		  sep = environment->get(SNA_PARAMETER_COMMUNITIES_SEP, SNA_PARAMETER_COMMUNITIES_SEP_DEFAULT);
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  Clique id;
			  Community comm;

			  std::set<int> orphan_members;

			  int num_total;
			  int num_regular;
			  int num_orphan;
			  int orphans;

			  for (size_t i  =0 ;  i< inputs[0].num_kvs ; i++) {
			    num_total = 0;
			    num_regular = 0;
			    num_orphan = 0;

			    //parse inputs
			    id.parse( inputs[0].kvs[i]->key );
			    comm.parse( inputs[0].kvs[i]->value );

			    num_total = comm.nodes.nodes_length;
			    num_orphan = comm.orphan.nodes_length;
			    num_regular = num_total - num_orphan;

			    //create a set with orphan members of this community
			    orphan_members.clear();
			    for (int j = 0 ; j < comm.orphan.nodes_length ; j++) {
			      orphan_members.insert(comm.orphan.nodes[j].value);
			    }

			    for (int j=0; j< comm.nodes.nodes_length ; j++) {

			    	//total += fprintf(file, "%lu%s", comm.id_comm, sep);


			      //BUG solved by amd

			      unsigned long nodeIdInf = 0;
			      unsigned long nodeIdSup = 0;
			      nodeIdInf = comm.nodes.nodes[j].value % 1000000000;
			      nodeIdSup = (comm.nodes.nodes[j].value-nodeIdInf);
			      if(nodeIdSup > 0) {
			        nodeIdSup /= 1000000000;
			        //total += fprintf(file,"%lu",nodeIdSup);
			      }

			      //ACG: No funcionaba si el primer digito de la parte menos significativa es 0
			      //total += fprintf(file,"%09lu%s",nodeIdInf, sep);

			      if(orphan_members.find(comm.nodes.nodes[j].value) != orphan_members.end()) {
			    	  orphans = 1;
			        //total += fprintf(file, "%d%s", 1, sep);
			      } else {
			    	  orphans = 0;
			        //total += fprintf(file, "%d%s", 0, sep);
			      }


			      //total += fprintf(file, "%d%s", num_regular, sep);
			      //total += fprintf(file, "%d%s", num_orphan, sep);
			      //total += fprintf(file, "%d", num_total);

			      snprintf(output, MAX_STR_LEN, "%lu%s%lu%s%09lu%s%d%s%d%s%d%s%d\n", comm.id_comm.value, sep.c_str(), nodeIdSup, sep.c_str(), nodeIdInf, sep.c_str(), orphans, sep.c_str(), num_regular, sep.c_str(), num_orphan, sep.c_str(), num_total);



			      //total += fprintf(file, "\n");

			      writer->emit(output);
			    }
			  }
			  return;
			}

#ifdef RUNFINISH
		int runFinish(TXTWriter *writer ) {
			  output << "id_comm"<< sep;

			  output << "id_phone"<< sep;
			  output << "orphan"<< sep;

			  output << "num_regular"<< sep;
			  output << "num_orphan"<< sep;
			  output << "num_total";

			  output << "\n";
			}
#endif /* de RUNFINISH */



	};


} // end of namespace samson
} // end of namespace sna

#endif
