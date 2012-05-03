
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_borrar_client_clusters_out
#define _H_SAMSON_mob2_borrar_client_clusters_out


#include <stdint.h>
#include <samson/module/samson.h>
#include <samson/modules/mob2/Cluster.h>
#include <samson/modules/mob2/Node_Bts.h>



namespace samson{
namespace mob2{


class borrar_client_clusters_out : public samson::ParserOut
{
	//Inputs
	Node_Bts nodbts;
	Cluster cluster;

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif
#define MAX_STR_LEN 1024
	char output[MAX_STR_LEN];

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: mob2.Node_Bts mob2.Cluster

	extendedHelp: 		Parse out of clients clusterized

#endif // de INFO_COMMENT

	void init(TXTWriter *writer )
	{
	}

	void run(KVSetStruct* inputs , TXTWriter *writer )
	{

		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			size_t total = 0;
			nodbts.parse(inputs[0].kvs[i]->key);
			cluster.parse(inputs[0].kvs[i]->value);
			total += snprintf( output, MAX_STR_LEN,"%lu|",nodbts.phone.value);
			total += snprintf( output+total, MAX_STR_LEN-total,"%d|",cluster.label.value);
			total += snprintf( output+total, MAX_STR_LEN-total,"%d|",cluster.labelgroup.value);
			total += snprintf( output+total, MAX_STR_LEN-total,"%f\n", cluster.distance.value);
			writer->emit(output);

		}

	}

	void finish(TXTWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif