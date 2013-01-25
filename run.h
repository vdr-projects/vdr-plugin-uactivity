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
  static const char *OrginToString(eOrgin Orgin);
public:
  cRun() { };
  ~cRun();
  void SetConfigDirectory(const char *Directory);
  void SetCacheDirectory(const char *Directory);
  void SetResourceDirectory(const char *Directory);
  void Call(eOrgin Orgin, bool Active);
  void Call(eOrgin Orgin, eKeys Key);
  void Call(eOrgin Orgin);
};

extern cRun Run;

#endif //__RUN_H
