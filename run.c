
#include "run.h"
#include <vdr/thread.h>

cRun Run;

cRun::~cRun()
{
  if (myConfigDirectory) free(myConfigDirectory);
  if (myCacheDirectory) free(myCacheDirectory);
  if (myResourceDirectory) free(myResourceDirectory);
}

const char *cRun::OrginToString(eOrgin Orgin)
{
  const char *OrginStr = "";
  if (Orgin == oStartUp) OrginStr = "startup";
  else if (Orgin == oShutDown) OrginStr = "shutdown";
  else if (Orgin == oRunning) OrginStr = "running";

  return OrginStr;
}

void cRun::SetConfigDirectory(const char *Directory)
{
  myConfigDirectory = strdup(Directory);
} 

void cRun::SetCacheDirectory(const char *Directory)
{
  myCacheDirectory = strdup(Directory);
} 

void cRun::SetResourceDirectory(const char *Directory)
{
  myResourceDirectory = strdup(Directory);
} 

void cRun::Call(eOrgin Orgin, bool Active)
{
  const char *ActivityStatusStr = "";
  if (Active)
    ActivityStatusStr = "true";
  else
    ActivityStatusStr = "false";

  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "activity", OrginToString(Orgin), ActivityStatusStr, myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 
}

void cRun::Call(eOrgin Orgin, eKeys Key)
{
  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "key", OrginToString(Orgin), myKey.ToString(Key), myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 
}

void cRun::Call(eOrgin Orgin)
{
  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "watchdog", OrginToString(Orgin), "none", myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 
}
