
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Time
#define _H_SAMSON_system_Time


#include <samson/modules/system/Time_base.h>
#include <iostream>
#include <iomanip>

namespace samson{
namespace system{


	class Time : public Time_base
	{
	public:
		
		bool operator== (Time& time) {
			if( hour.value 		!= time.hour.value)		return false;
			if( minute.value 	!= time.minute.value)	return false;
			if( seconds.value 	!= time.seconds.value)	return false;
			return true;
		}
		
		bool operator!= (Time& time) {
			if( hour.value 		!= time.hour.value)		return true;
			if( minute.value 	!= time.minute.value)	return true;
			if( seconds.value 	!= time.seconds.value)	return true;
			return false;
		}
		
		std::string str(){
			std::ostringstream o;
			o << std::setw(2) << std::setfill('0')<< (int)hour.value << ":" << std::setfill('0')<<std::setw(2) << (int) minute.value << ":" << std::setfill('0')<<std::setw(2) << (int) seconds.value;
			return o.str();
		}
		
		void setFromStr_24H(const char *strTime)
		{
#define Char_to_int(x) ((x)-48)
		    hour.value    = Char_to_int(strTime[0])*10 + Char_to_int(strTime[1]);
		    minute.value  = Char_to_int(strTime[3])*10 + Char_to_int(strTime[4]);
		    seconds.value = Char_to_int(strTime[6])*10 + Char_to_int(strTime[7]);


#undef Char_to_int

		}


		void setFromStr_12H_AMPM(const char * strTime)
		{

#define Char_to_int(x) ((x)-48)
            hour.value    = Char_to_int(strTime[0])*10 + Char_to_int(strTime[1]);
            minute.value  = Char_to_int(strTime[3])*10 + Char_to_int(strTime[4]);
            seconds.value = Char_to_int(strTime[6])*10 + Char_to_int(strTime[7]);

            //change hour from AM/PM to 24H
            const char *am_pm = &(strTime[16]);
              if(strncmp(am_pm, "pm", strlen("pm")) == 0)
              {
                hour.value += 12;
            }

#undef Char_to_int

		}

		
	};


} // end of namespace samson
} // end of namespace system

#endif