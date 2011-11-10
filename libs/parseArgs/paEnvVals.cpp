#include <stdlib.h>             /* getenv                                    */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* lmVerbose, lmDebug, ...                   */

#include "paPrivate.h"          /* PaTypeUnion, config variables, ...        */
#include "paTraceLevels.h"      /* LmtPaEnvVal, ...                          */
#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paBuiltin.h"          /* paBuiltin, paBuiltinNoOf                  */
#include "paWarning.h"          /* paWaringInit, paWarningAdd                */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "parseArgs/paConfig.h"           /* paConfigActions                           */
#include "paEnvVals.h"          /* Own interface                             */



/* ****************************************************************************
*
* builtins - 
*/
extern int builtins;



/* ****************************************************************************
*
* paEnvName - get real name of variable (environment or RC-file variable)
*/
char* paEnvName(PaArgument* aP, char* out, int ix)
{
	if (aP->envName == NULL)
		out[0] = 0;
	else if (aP->envName[0] == 0)
		out[0] = 0;
	else if (aP->envName[0] == '!')
		strcpy(out, &aP->envName[1]);
	else if ((ix < builtins) && (paBuiltinPrefix != NULL) && (paBuiltinPrefix[0] != 0))
		sprintf(out, "%s%s", paPrefix, aP->envName);
	else if ((ix >= builtins) && (paPrefix != NULL) && (paPrefix[0] != 0))
		sprintf(out, "%s%s", paPrefix, aP->envName);
	else
		sprintf(out, "%s", aP->envName);

	return out;
}



/* ****************************************************************************
*
* paEnvVals - 
*/
int paEnvVals(PaArgument* paList)
{
	PaArgument*  aP;
	char         w[512];
	int          ix = 0;

	paIterateInit();
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char* val;
		char  envVarName[80];

		LM_T(LmtPaEnvVal, ("got aP '%s'", aP->name));

		if ((aP->what & PawVariable) == 0)
		{
			LM_T(LmtPaEnvVal, ("skipping aP '%s' as it is no variable", aP->name));
			continue;
		}

		paEnvName(aP, envVarName, ix);
		++ix;

		LM_T(LmtPaEnvVal, ("looking for '%s'", envVarName));

		val = getenv(envVarName);
		if (val)
		{
			aP->from = PafEnvVar;
			LM_T(LmtPaEnvVal, ("got value '%s' for %s", val, envVarName));

			switch (aP->type)
			{
			case PaString:
				strcpy((char*) aP->varP, val);
				break;

			case PaBoolean:
				if      ((strcmp(val, "TRUE") == 0) ||  (strcmp(val, "ON")  == 0) || (strcmp(val, "1") == 0))
					*((bool*) (long) aP->varP) = true;
				else if ((strcmp(val, "FALSE") == 0) || (strcmp(val, "OFF") == 0) || (strcmp(val, "0") == 0))
					*((bool*) (long) aP->varP) = false;
				else
				{
					sprintf(w, "bad value '%s' for boolean variable %s", val, envVarName);
					PA_WARNING(PasNoSuchBooleanValue, w);
				}
				break;

			case PaSList:
				LM_TODO(("string list ..."));
				break;

			case PaIList:
				LM_TODO(("int list ..."));
				break;

			case PaInt:
			case PaIntU:
				*((int*) aP->varP) = baStoi(val);
				LM_T(LmtPaEnvVal, ("got value %d for %s", *((int*) aP->varP), envVarName));
				break;

			case PaShort:
			case PaShortU:
				*((short*) (long) aP->varP) = baStoi(val);
				LM_T(LmtPaEnvVal, ("got value %d for %s", *((short*) aP->varP), envVarName));
				break;

			case PaFloat:
				*((float*) (long) aP->varP) = baStof(val);
				LM_T(LmtPaEnvVal, ("got value %f for %s", *((float*) aP->varP), envVarName));
				break;

			case PaDouble:
				*((double*) (long) aP->varP) = baStod(val);
				LM_T(LmtPaEnvVal, ("got value %f for %s", *((double*) aP->varP), envVarName));
				break;

			case PaChar:
			case PaCharU:
				*((char*) (long) aP->varP) = baStoi(val);
				LM_T(LmtPaEnvVal, ("got value %d for %s", *((char*) aP->varP), envVarName));
				break;

			default:
				sprintf(w, "bad type %d for variable %s", aP->type, envVarName);
				PA_WARNING(PasNoSuchType, w);
				return -1;
			}
		}
	}

	return 0;
}



