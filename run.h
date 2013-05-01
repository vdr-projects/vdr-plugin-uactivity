#ifndef __RUN_H
#define __RUN_H

#include <vdr/keys.h>

enum eOrgin {oStartUp, oStarted, oShutDown, oRunning};

class cRun {
private:
  char *myConfigDirectory;
  char *myCacheDirectory;
  char *myResourceDirectory; 
  cKey myKey;
  static const char *OrginToString(eOrgin Orgin);
  static const char *ActivityStatusToString(bool Active);
  const char *KeyToString(eKeys Key);
public:
  cRun() { };
  ~cRun();
  void SetConfigDirectory(const char *Directory);
  void SetCacheDirectory(const char *Directory);
  void SetResourceDirectory(const char *Directory);
  void CallActivity(eOrgin Orgin, bool Active);
  void CallKey(eOrgin Orgin, bool Active, eKeys Key);
  void CallWatchdog(eOrgin Orgin, bool Active);
};

extern cRun uactivityRun;

#endif //__RUN_H
