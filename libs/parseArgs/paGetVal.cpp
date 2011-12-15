#include <stdio.h>              /* stderr, stdout, ...                       */
#include <stdlib.h>             /* strtol, atoi                              */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* lmVerbose, lmDebug, ...                   */

#include "parseArgs/parseArgs.h"          /* progName                                  */
#include "paWarning.h"          /* paWarningAdd                              */
#include "paGetVal.h"           /* Own interface                             */



/* ****************************************************************************
*
* paGetVal - calculate the integer value of a string
*/
void* paGetVal(char* string, int* error, char* fullname)
{
	char*     digs;
	char*     str;
	void*     value;
	int       type;
	bool      negative = false;

	*error = PaOk;

	if (string[0] == '-')
	{
		++string;
		negative = true;
	}

	if      (strncmp(string, "0x", 2) == 0)	 type = PaHex;
	else if (strncmp(string, "0",  1) == 0)	 type = PaOct;
	else                                     type = PaDec;

	switch (type)
	{
	case PaHex: digs   = (char*) "0123456789abcdefABCDEF"; str = &string[2]; break;
	case PaDec: digs   = (char*) "0123456789";             str = &string[0]; break;
	case PaOct: digs   = (char*) "01234567";               str = &string[1]; break;
	default:    printf("Bad type ...\n"); *error = PaDef;                            return NULL;
	}

	if (strspn(str, digs) != strlen(str))
	{
		char w[512];

		sprintf(w, "%s: value '%s' for %s is not %sdecimal value", 
				progName, string, fullname,
				((type == PaHex)? "a hexa" : (type == PaOct)? "an octa" : "a "));
		PA_WARNING(PasBadValue, w);
		*error = type;
        // printf("%s\n", w);
		return NULL;
	}
	
    // printf("String: '%s'\n", string);
    value = (void*) 19;
	switch (type)
	{
	case PaHex: value  = (void*) strtoull(string, NULL, 16); break;
	case PaDec: value  = (void*) baStoi(string);                   break;
	case PaOct: value  = (void*) strtoull(string, NULL, 8);  break;
	default:    *error = PaDef; return NULL;            break;
	}

    // printf("Value: %llu\n", (long long) value);

	return (void*) ((negative == true)? - (long long) value : (long long) value);
}
