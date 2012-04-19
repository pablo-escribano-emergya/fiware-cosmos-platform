
#include "logMsg/logMsg.h"

#include "ClusterNodeType.h" // Own interface

namespace samson
{
   
   const char* ClusterNodeType2str( ClusterNodeType type )
   {
	  switch ( type )
	  {
	  case DelilahNode: return "delilah";
	  case WorkerNode:  return "worker";
	  case UnknownNode: return "unknown";
	  }
	  
	  LM_X(1, ("Impossible to got here"));
	  return "Error";
   }
   
}
