/*
 * uactivity.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <time.h>
#include <vdr/plugin.h>
#include <vdr/osdbase.h>
#include <vdr/shutdown.h>
#include "run.h"

#define STRINGIFYX(x) #x
#define STRINGIFY(x) STRINGIFYX(x)
#define WATCHDOG 0

static const char *VERSION        = "0.0.1pre";
static const char *DESCRIPTION    = trNOOP("running shellscipts based upton user aktivity changes");

class cPluginUactivity : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  bool LastActivity;
  int WatchdogTimer;
  time_t LastTime;
public:
  cPluginUactivity(void);
  virtual ~cPluginUactivity() { };
  virtual const char *Version(void) { return VERSION; };
  virtual const char *Description(void) { return tr(DESCRIPTION); };
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
protected:
  virtual eOSState ProcessKey(eKeys Key) {
    eOSState state = cOsdMenu::ProcessKey(Key);
    if (state == osUnknown)
      if ((Key >= kUp) && (Key < kNone)) {
        Run.Call(Key);
        return osEnd;
      }
    return state;
  }
public:
  cPluginUactivityMenu() : cOsdMenu(PLUGIN_NAME_I18N) { }
};

cPluginUactivity::cPluginUactivity(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  WatchdogTimer = WATCHDOG;
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
    {"watchdog", required_argument, NULL, 'w'},
    { NULL }
  };

  int c;
  while ((c = getopt_long(argc, argv, "w:", long_options, NULL)) != -1) {
    switch (c) {
      case 'w': if (isnumber(optarg))
                  if (atoi(optarg) > 0) WatchdogTimer = atoi(optarg);
                  else return false;
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
  Run.SetConfigDirectory(ConfigDirectory(PLUGIN_NAME_I18N));
#if VDRVERSNUM >= 10729
  Run.SetCacheDirectory(CacheDirectory(PLUGIN_NAME_I18N));
  Run.SetResourceDirectory(ResourceDirectory(PLUGIN_NAME_I18N));
#else
  Run.SetCacheDirectory(ConfigDirectory(PLUGIN_NAME_I18N));
  Run.SetResourceDirectory(ConfigDirectory(PLUGIN_NAME_I18N));
#endif

  bool ActivityStatus = !ShutdownHandler.IsUserInactive();
  LastActivity = ActivityStatus;
  Run.Call(oStartUp, ActivityStatus);

  time(&LastTime);
  if (WatchdogTimer > 0) Run.Call();

  return true;
}

void cPluginUactivity::Stop(void)
{
  // Stop any background activities the plugin is performing.
  Run.Call(oShutDown);
}

void cPluginUactivity::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
  bool ActivityStatus = !ShutdownHandler.IsUserInactive();
  if (ActivityStatus != LastActivity) {
    LastActivity = ActivityStatus;
    Run.Call(oRunning, ActivityStatus);
  }

  if (WatchdogTimer > 0) {
    time_t Seconds;
    time(&Seconds);
    if (difftime(Seconds, LastTime) >= WatchdogTimer) {
      time(&LastTime);
      Run.Call();
    }
  }
}

cOsdObject *cPluginUactivity::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return new cPluginUactivityMenu();
}

VDRPLUGINCREATOR(cPluginUactivity); // Don't touch this!
