
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_FuseCategoryHits
#define _H_SAMSON_url_FuseCategoryHits


#include <samson/module/samson.h>


/******************************************************************
reduce FuseCategoryHits
{
	in system.UInt url.ServerPathVector  # Category(id) - ServerPathVector (historical)
	in system.UInt url.ServerPathVector  # Category(id) - ServerPathVector
	out system.UInt url.ServerPathVector # Category(id) - ServerPathVector (updated)

	helpLine "Update the list of hits per category"
}
*******************************************************************/
namespace samson{
namespace url{


	class FuseCategoryHits : public samson::Reduce
	{
		samson::system::UInt catId;
		ServerPathVector pathsRef;
		ServerPathVector paths;

		std::vector <ServerPathCount> vpaths_out;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			size_t num_hits = environment->getSizeT("url.num_hits", 3);

			//OLM_T(LMT_User06,("Start: inputs[0].num_kvs:%d, inputs[1].num_kvs:%d", inputs[0].num_kvs, inputs[1].num_kvs));

			if (inputs[1].num_kvs != 1)
			{
				OLM_E(("We can have one and only one entry per category: inputs[1].num_kvs:%d", inputs[1].num_kvs));

			}

			vpaths_out.clear();

			if (inputs[0].num_kvs == 1)
			{
				catId.parse(inputs[0].kvs[0]->key);
				pathsRef.parse(inputs[0].kvs[0]->value);
				for (int k = 0; (k < pathsRef.serverPath_length); k++)
				{
					// Accumulate is also performed directly with the integers
					pathsRef.serverPath[k].count = pathsRef.serverPath[k].count.value /2;
					vpaths_out.push_back(pathsRef.serverPath[k]);
				}
			}

			if (inputs[1].num_kvs == 1)
			{
				catId.parse(inputs[1].kvs[0]->key);
				paths.parse(inputs[1].kvs[0]->value);
				for (int j = 0; (j < paths.serverPath_length); j++)
				{
					//OLM_T(LMT_User06,("cat:%d: paths.serverPath_length:%d", catId.value, paths.serverPath_length));

					size_t server = paths.serverPath[j].server.value;
					std::string path = paths.serverPath[j].path.value;
					size_t count = paths.serverPath[j].count.value;

					bool found = false;
					for (size_t k = 0; (!found && (k < vpaths_out.size())); k++)
					{
						if ((server == vpaths_out[k].server.value) && (!path.compare(vpaths_out[k].path.value)))
						{
							vpaths_out[k].count.value += count;
							//OLM_T(LMT_User06,("Detected: cat:%d server:%d, path:%s, count:%d", catId.value, server, path.c_str(), count));
							found = true;
						}
					}
					if (!found)
					{
						//OLM_T(LMT_User06,("New: cat:%d server:%d, path:%s, count:%d", catId.value, server, path.c_str(), count));
						vpaths_out.push_back(paths.serverPath[j]);
					}
				}
			}

			std::sort ( vpaths_out.begin() , vpaths_out.end(), ServerPathCount::compare_by_weight_and_count );

			size_t count_hits = 0;
			ServerPathCount serverPath;
			size_t countRef = 0;
			ServerPathVector paths_out;

			for ( std::vector<ServerPathCount>::iterator iter = vpaths_out.begin() ; ((iter < vpaths_out.end()) && (count_hits < num_hits)) ; iter++, count_hits++)
			{
				serverPath = *iter;
				paths_out.serverPathAdd()->copyFrom(&serverPath);
				countRef += iter->count.value;
				//OLM_T(LMT_User06,("Add: cat:%d server:%d, path:%s, count:%d", catId.value, serverPath.server.value, serverPath.path.value.c_str(), serverPath.count.value));
			}
			paths_out.countRef = countRef;
			writer->emit(0, &catId, &paths_out);
		}


	};


} // end of namespace samson
} // end of namespace url

#endif