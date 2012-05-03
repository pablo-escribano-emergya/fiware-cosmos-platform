
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_urlbenchmark_export_top_hits_per_category
#define _H_SAMSON_urlbenchmark_export_top_hits_per_category


#include <samson/module/samson.h>

/************************************************
parserOut export_top_hits_per_category
{
	in system.UInt urlbenchmark.TopHits      # Category - Top URLS visited

	helpLine "Dumps to text the top hits visited per category"
}
************************************************/

namespace samson{
namespace urlbenchmark{


	class export_top_hits_per_category : public samson::ParserOut
	{
		samson::system::UInt categ;
		TopHits hits;

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif

#define MAX_STR_LEN 2048
			char output[MAX_STR_LEN];
			char *p_output;

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			int escritos;

			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				categ.parse(inputs[0].kvs[i]->key);
				hits.parse(inputs[0].kvs[i]->value);

				escritos = snprintf(output, MAX_STR_LEN, "cat_%lu: [", categ.value);
				p_output = output + escritos;

				for (int j = 0; (j < hits.hit_length); j++)
				{
#define MAX_CHUNK_LEN 128
					escritos += snprintf(p_output, MAX_CHUNK_LEN, "%s(%lu), ", hits.hit[j].URL.value.c_str(), hits.hit[j].count.value);
					if ((escritos + MAX_CHUNK_LEN) > MAX_STR_LEN)
					{
						break;
					}
					p_output = output + escritos;
				}
				strcat(p_output, " ]\n");

				writer->emit(output);
			}
		}


	};


} // end of namespace samson
} // end of namespace urlbenchmark

#endif