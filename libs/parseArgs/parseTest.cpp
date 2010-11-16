#include "parseArgs.h"
#include "logMsg.h"


int        i = 19;
int        ui;
int        s;
int        us;
int        c;
int        uc;
bool       lmTest;
char       string[256];
char       str2[256];
char       str3[256];
int        i2;
int        i3;
int        i4;
int        i5;
int        i6;
int        i7;
int        p1;
int        p2;
int        v1;
int        v2;
int        il[32];
char*      cl[32];
float      f1;
double     d1;



/* ****************************************************************************
*
* paArgs - 
*/
PaArgument paArgs[] =
{
	{ "-il",      &il,     "IL",     PaIList,  PaOpt, PaND,     PaNL,    PaNL,    "integer list"        },
	{ "-c",       &c,      "C",      PaChar,   PaOpt, 19,       0,       100,     "char"                },
	{ "-cl",       cl,     "CL",     PaSList,  PaOpt, PaND,     PaNL,    PaNL,    "string list"         },
	{ "-i",       &i,      "I",      PaInt,    PaOpt, PaND,     0,       100,     "integer"             },
	{ "-ui",      &ui,     "UI",     PaIntU,   PaOpt, 19,       0,       100,     "unsigned integer"    },
	{ "-s",       &s,      "S",      PaShort,  PaOpt, PaND,     0,       100,     "short"               },
	{ "-us",      &us,     "US",     PaShortU, PaOpt, 19,       0,       100,     "unsigned short"      },
	{ "-uc",      &uc,     "UC",     PaCharU,  PaOpt, 19,       0,       100,     "unsigned char"       },
	{ "-str",      string, "STR",    PaStr,    PaOpt, _i "DEF", _i "1",  _i "E9", "string"              },
	{ "-ihid",    &i2,     "HID",    PaInt,    PaHid, 19,       0,       100,     "hidden"              },
	{ "-ireq",    &i3,     "REQ",    PaInt,    PaReq, 0,        0,       100,     "required"            },
	{ "-inoul",   &i4,     "NO_UL",  PaInt,    PaOpt, 19,       0,       PaNL,    "no upper limit"      },
	{ "-inoll",   &i5,     "NO_LL",  PaInt,    PaOpt, 19,       PaNL,    100,     "no lower limit"      },
	{ "-idefU",   &i6,     "DEFU",   PaInt,    PaOpt, 19,       0,       19,      "up limit def error"  },
	{ "-idefL",   &i7,     "DEFL",   PaInt,    PaOpt, 21,       20,      100,     "low limit def error" },
	{ "-isnoul",   str2,   "SNO_UL", PaStr,    PaOpt, _i "19",  _i "02", PaNL,    "no upper limit"      },
	{ "-isnoll",   str3,   "SNO_LL", PaStr,    PaOpt, _i "19",  PaNL,    _i "20", "no lower limit"      },
	{ " ",        &p1,     "P1",     PaInt,    PaOpt, 19,       PaNL,    1000,    "parameter 1"         },
	{ " ",        &p2,     "P2",     PaInt,    PaOpt, 19,       PaNL,    1000,    "parameter 2"         },
	{ "",         &v1,     "V1",     PaInt,    PaOpt, 19,       PaNL,    1000,    "env var 1"           },
	{ "-f1",      &f1,     "F",      PaFloat,  PaOpt, _i 3.1,   0,       100,     "float"               },
	{ "-d1",      &d1,     "D",      PaDouble, PaOpt, _i 3.3,   0,       100,     "double"              },
	{ "--lmTest", &lmTest, "",       PaBool,   PaOpt, FALSE,    FALSE,   TRUE,    "test logMsg"         },
	{ NULL,       &v2,     "V2",     PaInt,    PaOpt, 19,       PaNL,    1000,    "env var 2"           },

	PA_END_OF_ARGS
};

int main(int argC, char* argV[])
{
    paConfig("prefix", "PT_");
    // paConfig("trace levels", "0-255");        /* valid until finishing the paParse call */
    // paConfig("debug mode",   (void*) TRUE);   /* valid until finishing the paParse call */
    // paConfig("verbose mode", (void*) TRUE);   /* valid until finishing the paParse call */
    paConfig("log to file", "/tmp/");
    paConfig("log file line format", "DEF");
    paConfig("log file time format", "DEF");
    paConfig("usage and exit on any warning", (void*) TRUE);

    paConfig("log to screen", "only errors");
    paConfig("screen line format", "TYPE:FUNC: TEXT");

#if 0
    paConfig("log clearing", (void*) FALSE);
    paConfig("remove builtin", "-v");
#endif
    
    LM_V(("parsing arguments"));
    paParse(paArgs, argC, argV, 1, FALSE);

    if (il[0] != 0)
    {
	int ix;

	printf("got %d items in int-list\n", il[0]);

	for (ix = 1; ix <= il[0]; ix++)
	    printf(" int-list item %d: %d\n", ix, il[ix]);
    }

    if ((int) cl[0] != 0)
    {
	int ix;

	printf("got %d items in string-list\n", (int) cl[0]);

	for (ix = 1; ix <= (int) cl[0]; ix++)
	    printf(" string-list item %d: %s\n", ix, cl[ix]);
    }

    printf("parameter 1: %d\n", p1);
    printf("parameter 2: %d\n", p2);

    printf("f1: %f\n", f1);
    printf("d1: %f\n", d1);

    LM_V(("Verbose message"));
    LM_D(("Debug message"));

    {
	int ix;
	
	for (ix = 0; ix < 255; ix++)
	    LM_T(ix, ("Trace level %d is on", ix));
    }
		
    if (lmTest)
    {
	int ix;

	for (ix = 0; ix < 5000; ix++)
	{
	    LM_M(("MESSAGE  %d", ix));
	    LM_W(("WARNING  %d", ix));
	    LM_E(("ERROR    %d", ix));
	    LM_V(("VERBOSE  %d", ix));
	    LM_D(("DEBUG    %d", ix));
	}
    }
    return 0;
}
