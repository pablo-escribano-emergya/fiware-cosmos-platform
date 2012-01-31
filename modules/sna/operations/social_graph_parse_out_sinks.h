
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_social_graph_parse_out_sinks
#define _H_SAMSON_sna_social_graph_parse_out_sinks


#include <samson/module/samson.h>
#include "sna_environment_parameters.h"
#include <iostream>
#include <sstream>


/****************************************************************
parserOut social_graph_out
{
  in  system.UInt sna.Node

  setup

  help
  {
    " Convert Social Graph to txt phoneA|phoneB|weight|dir."
    " When used with -duplicated every link will be printed, if not, only links where phoneA < phoneB are printed"
    " When used with -extern_mark a fifth column will be included indicating if phoneA is extern or subscriber"
  }
}
********************************************************************/
namespace samson{
namespace sna{


	class social_graph_parse_out_sinks : public samson::ParserOut
	{
		int max_connections ;
		int max_strong_connections ;
		char* sep ;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];


	public:

		void init(samson::KVWriter *writer ){

		  const char * def = "|";

		  OLM_M(("Starts social_graph_parse_out_sinks::init()"));

		  sep = strdup(environment->get("sna.separator", def).c_str());

		  // Here we filter elements with more than X connections
		  max_connections = environment->getInt(SNA_PARAMETER_MAX_CONNECTIONS , SNA_PARAMETER_MAX_CONNECTIONS_DEFAULT);

		  // Here we filter elements with more than X strong connections
		  max_strong_connections = environment->getInt(SNA_PARAMETER_MAX_STRONG_CONNECTIONS, SNA_PARAMETER_MAX_STRONG_CONNECTIONS_DEFAULT);
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  Node node_data;

			  //OLM_T(LMT_User01, ("Processing %d kv", inputs[0].num_kvs));

			  // Already in init()
			  /****************************************************************
			  const char * def = "|";

			  sep = strdup(environment->get("sna.separator", def).c_str());
			  // Here we filter elements with more than X connections
			  max_connections = environment->getInt(SNA_PARAMETER_MAX_CONNECTIONS , SNA_PARAMETER_MAX_CONNECTIONS_DEFAULT);

			  // Here we filter elements with more than X strong connections
			  max_strong_connections = environment->getInt(SNA_PARAMETER_MAX_STRONG_CONNECTIONS, SNA_PARAMETER_MAX_STRONG_CONNECTIONS_DEFAULT);
			  /*************************************************************************/

			  if (inputs[0].num_kvs > 0)
			  {
				  samson::system::UInt key;
				  key.parse(inputs[0].kvs[0]->value);

				  //OLM_T(LMT_User01, ("phone:%ld, Starts social_graph_parse_out_sinks::run() with inputs[0].num_kvs:%d", key.value, inputs[0].num_kvs));
			  }
			  else
			  {
				  //OLM_T(LMT_User01, ("Starts social_graph_parse_out_sinks::run() with inputs[0].num_kvs == 0"));
			  }

			  for (size_t i  =0 ;  i< inputs[0].num_kvs ; ++i) {
			    node_data.parse( inputs[0].kvs[i]->value );
		    	//OLM_T(LMT_User01, ("node_id:%ld, numberOfLinksWithWeightEqualOrHigher(1.0):%d, numberOfLinksWithWeightEqualOrHigher(0.0):%d, max_strong_connections:%d , max_connections:%d ", node_data.id.value, node_data.numberOfLinksWithWeightEqualOrHigher( 1.0 ), node_data.numberOfLinksWithWeightEqualOrHigher( 0.0 ), max_strong_connections, max_connections));



			    if (
			      (  node_data.numberOfLinksWithWeightEqualOrHigher( 1.0 ) >= max_strong_connections ) ||
			      (  (node_data.numberOfLinksWithWeightEqualOrHigher( 0.0 ) - node_data.numberOfLinksWithWeightEqualOrHigher( 1.0 ) ) >=  max_connections )
			      )
			    {
			    	snprintf(output, MAX_STR_LEN, "%lu%s%lu%s%lu\n", node_data.id.value, sep, (long unsigned int) node_data.numberOfLinksWithWeightEqualOrHigher( 1.0 ), sep, (long unsigned int) node_data.numberOfLinksWithWeightEqualOrHigher( 0.0 ));
				    writer->emit(output);
			    }
			  }
			  return;
		}

#ifdef RUNFINISH
		int MACRO_sna_social_graph_parse_out_sinks::parseOutFinish( FILE *file) {
		  int total = 0;

		  total += fprintf(file, "phone%s", sep);
		  total += fprintf(file, "strong_contacts%s", sep);
		  total += fprintf(file, "contacts");

		  total += fprintf(file, "\n");

		  free(sep);

		  return total;
		}


#endif

	};


} // end of namespace samson
} // end of namespace sna

#endif