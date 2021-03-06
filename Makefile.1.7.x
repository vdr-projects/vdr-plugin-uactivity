#
# Makefile for a Video Disk Recorder plugin
#
# $Id$

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.

PLUGIN = uactivity

### The version number of this plugin (taken from the main source file):

VERSION = $(shell [ -d .git ] && git describe --tags --dirty| sed 's/^v//' || cat version)
ifeq ($(strip $(VERSION)),)
    $(error VERSION is undefined)
endif

### The directory environment:

# Use package data if installed...otherwise assume we're under the VDR source directory:
PKGCFG = $(if $(VDRDIR),$(shell pkg-config --variable=$(1) $(VDRDIR)/vdr.pc),$(shell pkg-config --variable=$(1) vdr || pkg-config --variable=$(1) ../../../vdr.pc))
LIBDIR = $(call PKGCFG,libdir)
LOCDIR = $(call PKGCFG,locdir)
PLGCFG = $(call PKGCFG,plgcfg)
BINDIR = $(call PKGCFG,bindir)
CFGDIR = $(call PKGCFG,configdir)
CACHEDIR = $(call PKGCFG,cachedir)

#
TMPDIR ?= /tmp

### The compiler options:

export CFLAGS   = $(call PKGCFG,cflags)
export CXXFLAGS = $(call PKGCFG,cxxflags)

### The version number of VDR's plugin API:

APIVERSION = $(call PKGCFG,apiversion)

### Allow global user defined options to overwrite defaults:

-include $(PLGCFG)

### Allow user defined options to overwrite defaults:

-include Make.config

### Default values:

PLUGIN_UACTIVITY_COMMAND?=vdr-uactivity -r \"%1$$s\" -o \"%2$$s\" -C \"%3$$s\" -c \"%4$$s\" -R \"%5$$s\" -A \"%6$$s\" -K \"%7$$s\" -V \"%8$$s\"

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The name of the shared object file:

SOFILE = libvdr-$(PLUGIN).so

### Includes and Defines (add further entries here):

INCLUDES +=

DEFINES += -DPLUGIN_NAME_I18N='"$(PLUGIN)"' -DUACTIVITY_COMMAND='"$(PLUGIN_UACTIVITY_COMMAND)"' -DPLUGIN_VERSION='"$(VERSION)"'

### The object files (add further files here):

OBJS = $(PLUGIN).o run.o

### The main target:

all: $(SOFILE) i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
I18Npo    = $(wildcard $(PODIR)/*.po)
I18Nmo    = $(addsuffix .mo, $(foreach file, $(I18Npo), $(basename $(file))))
I18Nmsgs  = $(addprefix $(DESTDIR)$(LOCDIR)/, $(addsuffix /LC_MESSAGES/vdr-$(PLUGIN).mo, $(notdir $(foreach file, $(I18Npo), $(basename $(file))))))
I18Npot   = $(PODIR)/$(PLUGIN).pot

%.mo: %.po
	msgfmt -c -o $@ $<

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP --package-name=vdr-$(PLUGIN) --package-version=$(VERSION) --msgid-bugs-address='<see README>' -o $@ `ls $^`

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q -N $@ $<
	@touch $@

$(I18Nmsgs): $(DESTDIR)$(LOCDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.mo
	install -D -m644 $< $@

.PHONY: i18n
i18n: $(I18Nmo) $(I18Npot)

install-i18n: $(I18Nmsgs)

### Targets:

$(SOFILE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared $(OBJS) -o $@

install-lib: $(SOFILE)
	install -D $^ $(DESTDIR)$(LIBDIR)/$^.$(APIVERSION)

install-bin:
ifndef PLUGIN_UACTIVITY_NOINSTALL_BIN
	install -D script/vdr-uactivity $(DESTDIR)$(BINDIR)/vdr-uactivity
	install -d $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/activity
	install script/uactivity/activity/* $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/activity
	chmod -x $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/activity/README
	install -d $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/key
	install script/uactivity/key/* $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/key
	chmod -x $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/key/README
	install -d $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/watchdog
	install script/uactivity/watchdog/* $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/watchdog
	chmod -x $(DESTDIR)$(CFGDIR)/plugins/$(PLUGIN)/watchdog/README
	[ -d "$(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/activity" ] && chmod ugo+w $(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/activity || true
	[ -d "$(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/key" ] && chmod ugo+w $(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/key || true
	[ -d "$(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/watchdog" ] && chmod ugo+w $(DESTDIR)$(CACHEDIR)/plugins/$(PLUGIN)/watchdog || true
endif

install: install-lib install-i18n install-bin

dist: $(I18Npo) clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(PODIR)/*.mo $(PODIR)/*.pot
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~
