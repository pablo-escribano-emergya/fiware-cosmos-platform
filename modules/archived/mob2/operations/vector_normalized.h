
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_vector_normalized
#define _H_SAMSON_mob2_vector_normalized


#include <samson/module/samson.h>
#include <samson/modules/mob2/ClusterVector.h>
#include <samson/modules/mob2/Node_Bts.h>



namespace samson{
namespace mob2{


class vector_normalized : public samson::Map
{
	//Inputs
	Node_Bts nodbts;
	ClusterVector vector_in;
	//Outputs
	ClusterVector vector_norm;
	//Intermediate
	ClusterVector div;
	samson::system::Double elem;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: mob2.Node_Bts mob2.ClusterVector
	output: mob2.Node_Bts mob2.ClusterVector

	extendedHelp: 		Normalization of a vector

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			nodbts.parse(inputs[0].kvs[i]->key);
			vector_in.parse(inputs[0].kvs[i]->value);

			///// Initialization of values /////
			double sumvalues = 0;
			div.comsSetLength(0);
			vector_norm.comsSetLength(0);

			///// Div on total number of days of the group /////
			for(int j=0; j<vector_in.coms_length; j++)
			{
				if(j < 24) // Mondays, Tuesday, Wednesday and Thursday --> Total: 103 days
				{
					elem.value = vector_in.coms[j].value / 103;
					sumvalues += elem.value;
				}
				else
				{
					elem.value = vector_in.coms[j].value / 26;
					sumvalues += elem.value;
				}
				div.comsAdd()->copyFrom(&elem);
			}

			///// Normalization of vector /////
			for(int j=0; j<div.coms_length; j++)
			{
				elem.value = div.coms[j].value / sumvalues;
				vector_norm.comsAdd()->copyFrom(&elem);
			}
			writer->emit(0, &nodbts,&vector_norm);
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif