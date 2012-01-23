
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_fuseServerUserPath
#define _H_SAMSON_url_fuseServerUserPath


#include <samson/module/samson.h>

/**************************************************************
reduce fuseServerUserPath
{
	in system.String	url.UserPath	# Server(string) User(id)-Path(string)
	in system.String	system.UInt	    # Server(string) server(id)
	out system.UInt		system.String	# Server - Path
	out system.UInt		url.ServerPath	# User - ServerPath

	helpLine "From the URL information and server encoding. generates the subsets related to the complete URL path, needed in the next steps. Splitted from reduce fuseServer because of limited number of outputs in the platform"

}
***************************************************************/

namespace samson{
namespace url{


	class fuseServerUserPath : public samson::Reduce
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		    if ((inputs[1].num_kvs == 0) && (inputs[0].num_kvs == 0))
		    {
		      OLM_E(("Server without key-values"));
		      return;
		    }

		    if ( inputs[0].num_kvs == 0)
		    {
		      samson::system::String s;
		      s.parse( inputs[1].kvs[0]->key );
		      OLM_D(("'%s' has no hits",s.value.c_str()));
		      return;
		    }

		    if ( inputs[1].num_kvs == 0)
		    {
		      samson::system::String s;
		      s.parse( inputs[0].kvs[0]->key );
		      OLM_E(("Input '%s' not included in server encoding table",s.value.c_str()));
		      return;
		    }

		    if ( inputs[1].num_kvs != 1)
		    {
		      samson::system::String s;
		      s.parse( inputs[1].kvs[0]->key );
		      OLM_E(("Input '%s' with multiple entries in server encoding table: inputs[1].num_kvs:%d",s.value.c_str(), inputs[1].num_kvs));
		    }

		  samson::system::UInt server;


		  server.parse(inputs[1].kvs[0]->value);

		  for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
		  {
			  UserPath userPath;


		    userPath.parse( inputs[0].kvs[i]->value );
		    writer->emit( 0 , &server , &userPath.path );

		    ServerPath serverPath;
		    serverPath.server = server;
		    serverPath.path = userPath.path;

		    writer->emit( 1  , &userPath.user, &serverPath);

		  }

		}


	};


} // end of namespace samson
} // end of namespace url

#endif