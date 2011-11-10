
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_hit_reduceHitCountCollection
#define _H_SAMSON_hit_reduceHitCountCollection


#include <samson/module/samson.h>
#include <samson/modules/hit/HitCollection.h>
#include <samson/modules/hit/HitCount.h>
#include <samson/modules/hit/HitCountCollection.h>
#include <samson/modules/system/String.h>


namespace samson{
namespace hit{


	class reduceHitCountCollection : public samson::Reduce
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String hit.HitCount  
input: system.String hit.HitCountCollection  
output: system.String hit.HitCollection
output: system.String hit.HitCountCollection

helpLine: Filter top elements per global-concept
#endif // de INFO_COMMENT

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace hit
} // end of namespace samson

#endif