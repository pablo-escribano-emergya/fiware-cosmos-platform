
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_Float
#define _H_SAMSON_system_Float

#include <samson/modules/system/FixedLengthDataInstance.h>
#include <cmath>

namespace samson{
namespace system{


class Float : public FixedLengthDataInstance<float>
{
public:

   std::string getName()
   {
	  return "system.Float";
   }


	int hash(int max_num_partitions)
	{
		return fmod(abs(static_cast<int>(1000*value)), max_num_partitions);
	}

	void setFromString(const char *_data)
	{
		value = strtod(_data, NULL);
	}

	int *getDataPath(const std::string &dataPathString){
		return(getDataPathStatic(dataPathString));
	}

	static int *getDataPathStatic(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		int nlevels = 1;
		int *dataPathIntP;

		const char *p_sep = dataPathCharP;
		while ((p_sep = strchr(p_sep, '.')) != NULL)
		{
			nlevels++;
			p_sep++;
		}

		if ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)
		{
			return ((int *)NULL);
		}

		int retError = getDataPath(dataPathCharP, dataPathIntP);

		if (retError)
		{
			free(dataPathIntP);
			dataPathIntP = NULL;
		}

		return  (dataPathIntP);
	}

	static int getDataPath(const char * dataPathCharP, int *dataPathIntP){
		if (*dataPathCharP == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		if (strcmp(dataPathCharP, "Float") == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		return -1;
	}

	std::string getTypeFromPath(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		return(getTypeFromPathStatic(dataPathCharP));
	}

	static std::string getTypeFromPathStatic(const char * dataPathCharP){
		if (*dataPathCharP == 0)
		{
			return ("system.Float");
		}
		if (strcmp(dataPathCharP, ".") == 0)
		{
			return ("system.Float");
		}


		if (strcmp(dataPathCharP, "Float") == 0)
		{
			return ("system.Float");
		}
		return("_ERROR_");
	}

	std::string getTypeFromPath(const int *dataPathIntP){
		return(getTypeFromPathStatic(dataPathIntP));
	}

	static std::string getTypeFromPathStatic(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
		case -1:
			return ("system.Float");
			break;
		default:
			return ("_ERROR_");
			break;
		};
		return NULL;
	}

	static const char *getTypeStatic()
	{
		return ("system.Float");
	}

	const char *getType()
	{
		return ("system.Float");
	}

	static bool checkTypeStatic(const char *type)
	{
		if (strcmp(type, "system.Float") == 0)
		{
			return true;
		}
		return false;
	}

	bool checkType(const char *type)
	{
		if (strcmp(type, "system.Float") == 0)
		{
			return true;
		}
		return false;
	}

	static size_t getHashTypeStatic(){
		return(16724176993843471292ULL);
	}

	size_t getHashType(){
		return(16724176993843471292ULL);
	}

	static bool checkHashTypeStatic(size_t valType){
		if (valType == 16724176993843471292ULL)
		{
			return true;
		}		return false;
	}

	 bool checkHashType(size_t valType){
		if (valType == 16724176993843471292ULL)
		{
			return true;
		}		return false;
	}

	DataInstance * getDataInstanceFromPath(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
		case -1:
			return (this);
			break;
		default:
			return (NULL);
			break;
		};
		return NULL;
	}



};


} // end of namespace samson
} // end of namespace system

#endif
