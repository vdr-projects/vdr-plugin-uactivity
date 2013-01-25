#ifndef __RUN_H
#define __RUN_H

#include <vdr/keys.h>

enum eOrgin {oStartUp, oShutDown, oRunning};

class cRun {
private:
  char *myConfigDirectory;
  char *myCacheDirectory;
  char *myResourceDirectory; 
  cKey myKey;
public:
  cRun() { };
  ~cRun();
  void SetConfigDirectory(const char *Directory);
  void SetCacheDirectory(const char *Directory);
  void SetResourceDirectory(const char *Directory);
  void Call(eOrgin Orgin, bool Active=false);
  void Call(eKeys Key);
  void Call();
};

extern cRun Run;

#endif //__RUN_H
