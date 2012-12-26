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
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_hit_Hit
#define _H_SAMSON_hit_Hit

#include <time.h>
#include <limits.h>
#include <stdint.h>

#include <samson/modules/hit/Hit_base.h>


namespace samson{
namespace hit{


	class Hit : public Hit_base
	{

	public:

	   void init( )
	   {
		  init( ""  , 0 );
	   }

	   void init( std::string _concept , time_t ts )
	   {
		  concept.value = _concept;
		  count.value = 0;
		  time.value = ts;
	   }
/*
	   void setTime( time_t current_ts )
	   {
		  if( current_ts < time.value )
			 return;

          ::time_t diff =  current_ts - time.value;
          count.value = (( (double) count.value ) * pow( 0.99666666  , (double) diff ));
		  time.value = current_ts;
	   }
*/


	   void setTime( time_t current_ts , double forgetting_factor )
	   {
		  if( current_ts < time.value )
			 return;

          ::time_t diff =  current_ts - time.value;

          count.value = (( (double) count.value ) * pow( forgetting_factor  , (int) diff ));
		  time.value = current_ts;
	   }

	   std::string getConcept()
	   {
		  size_t pos = concept.value.find(' ');

		  if( ( pos == std::string::npos ) || ( pos == 0 ))
			 return concept.value;

		  //std::string category_concept = concept.value.substr( 0 , pos );
		  std::string name = concept.value.substr( pos+1 , std::string::npos );
		  
		  return name;
	   }

	   std::string getCategory()
	   {
		  size_t pos = concept.value.find(' ');
		  
		  if( ( pos == std::string::npos ) || ( pos == 0 ))
			 return "top";
		  
		  std::string category_concept = concept.value.substr( 0 , pos );
		  //std::string name = concept.value.substr( pos+1 , std::string::npos );
		  
		  return category_concept;
	   }
	};


} // end of namespace samson
} // end of namespace hit

#endif
