
#include "run.h"
#include <vdr/thread.h>

cRun Run;

cRun::~cRun()
{
  if (myConfigDirectory) free(myConfigDirectory);
  if (myCacheDirectory) free(myCacheDirectory);
  if (myResourceDirectory) free(myResourceDirectory);
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
  char *OrginStr = NULL;
  if (Orgin == oStartUp) OrginStr = strdup("startup");
  else if (Orgin == oShutDown) OrginStr = strdup("shutdown");
  else if (Orgin == oRunning) OrginStr = strdup("running");

  char *ActivityStatusStr = NULL;
  if (Active)
    ActivityStatusStr = strdup("true");
  else
    ActivityStatusStr = strdup("false");

  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "activity", OrginStr, ActivityStatusStr, myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 

  if (ActivityStatusStr) free(ActivityStatusStr);
  if (OrginStr) free(OrginStr);
}

void cRun::Call(eKeys Key)
{
  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "key", "none", myKey.ToString(Key), myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 
}

void cRun::Call()
{
  char *buffer;
  asprintf(&buffer, UACTIVITY_COMMAND, "watchdog", "none", "none", myConfigDirectory, myCacheDirectory, myResourceDirectory);
  SystemExec(buffer, true);
  free(buffer); 
}
