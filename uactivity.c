/*
 * uactivity.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include <vdr/osdbase.h>
#include <vdr/keys.h>
#include <vdr/shutdown.h>

#define STRINGIFYX(x) #x
#define STRINGIFY(x) STRINGIFYX(x)
#define WATCHDOG 0

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = trNOOP("running shellscipts based upton user aktivity changes");

enum eOrgin {oStartUp, oShutDown, oRunning};

class cPluginUactivity : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  void Call(eOrgin Orgin, bool Active=false);
  void Call();
  bool LastActivity;
  bool WatchdogTimer;
  char *myConfigDirectory;
  char *myCacheDirectory;
  char *myResourceDirectory;
public:
  cPluginUactivity(void);
  virtual ~cPluginUactivity();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return tr(DESCRIPTION); }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void) { return true; };
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void) {  };
  virtual void MainThreadHook(void);
  virtual cString Active(void) { return NULL; };
  virtual time_t WakeupTime(void) { return 0; };
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void) { return NULL; };
  virtual bool SetupParse(const char *Name, const char *Value) { return false; };
  virtual bool Service(const char *Id, void *Data = NULL) { return false; };
  virtual const char **SVDRPHelpPages(void) { return NULL; };
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) { return NULL; };
  };

class cPluginUactivityMenu : public cOsdMenu {
private:
  cKey myKey;
  void Call(eKeys Key) {
    esyslog("%s: Key -> %s", PLUGIN_NAME_I18N, myKey.ToString(Key));
  }

protected:
  virtual eOSState ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown)
      if ((Key >= kUp) && (Key < kNone)) {
        Call(Key);
        return osEnd;
      }
    return osContinue;
  }
public:
  cPluginUactivityMenu() : cOsdMenu(PLUGIN_NAME_I18N) { }
};

cPluginUactivity::cPluginUactivity(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  LastActivity = false;
  WatchdogTimer = WATCHDOG;
}

cPluginUactivity::~cPluginUactivity()
{
  // Clean up after yourself!
  if (myConfigDirectory) free(myConfigDirectory);
  if (myCacheDirectory) free(myCacheDirectory);
  if (myResourceDirectory) free(myResourceDirectory);
}

void cPluginUactivity::Call(eOrgin Orgin, bool Active)
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

  esyslog("%s: Orgin -> %s Activity -> %s", PLUGIN_NAME_I18N, OrginStr, ActivityStatusStr);

  if (ActivityStatusStr) free(ActivityStatusStr);
  if (OrginStr) free(OrginStr);
}

void cPluginUactivity::Call()
{
  esyslog("%s: Watchdog", PLUGIN_NAME_I18N);
}

const char *cPluginUactivity::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return "  -w <Seconds>,   --watchdog=<Seconds>"
         "   Call the watchdog script every <Seconds> seconds (default: '" STRINGIFY(WATCHDOG) "')\n"
         "                                      "
         "   '0' disables the call of the watchdog script\n";
}

bool cPluginUactivity::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  static struct option long_options[] = {
    {"watchdog-timer", required_argument, NULL, 'w'},
    { NULL }
  };

  int c;
  while ((c = getopt_long(argc, argv, "w:", long_options, NULL)) != -1) {
    switch (c) {
      case 'w': if (isnumber(optarg))
                  if (atoi(optarg) > 0)
                    WatchdogTimer = atoi(optarg);
                  else
                    return false;
                else
                  return false;
                break;
      default:  return false;
    }
  }
  return true;
}

bool cPluginUactivity::Start(void)
{
  // Start any background activities the plugin shall perform.
  myConfigDirectory = strdup(ConfigDirectory(PLUGIN_NAME_I18N));
#if VDRVERSNUM >= 10729
  myCacheDirectory = strdup(CacheDirectory(PLUGIN_NAME_I18N));
  myResourceDirectory = strdup(ResourceDirectory(PLUGIN_NAME_I18N));
#else
  myCacheDirectory = strdup(myConfigDirectory);
  myResourceDirectory = strdup(myConfigDirectory);
#endif

  bool ActivityStatus = !ShutdownHandler.IsUserInactive();
  Call(oStartUp, ActivityStatus);
  LastActivity = ActivityStatus;
  return true;
}

void cPluginUactivity::Stop(void)
{
  // Stop any background activities the plugin is performing.
  Call(oShutDown);
}

void cPluginUactivity::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
  bool ActivityStatus = !ShutdownHandler.IsUserInactive();
  if (ActivityStatus != LastActivity) {
    Call(oRunning, ActivityStatus);
    LastActivity = ActivityStatus;
  }
}

cOsdObject *cPluginUactivity::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cPluginUactivityMenu();
}

VDRPLUGINCREATOR(cPluginUactivity); // Don't touch this!
