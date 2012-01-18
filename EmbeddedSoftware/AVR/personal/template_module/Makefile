# Target file name (without extension).
TARGET = main
BIOSTARGET = bios

LIBDIR = ../../avr-lib
SRCDIR = src
MODDIR = modules
CFGDIR = .
#MODDIR = modules
BIOSDIR = bios
BIOSSRCDIR = $(LIBDIR)/bios
BUILDDIR = build

include bios.inc
include ../system.inc
#include modules.inc

APPMAKEFLAGS = 	TARGET=$(TARGET) \
				LIBDIR=$(abspath $(LIBDIR)) \
				SRCDIR=$(abspath $(SRCDIR)) \
				BIOSDIR=$(abspath $(BIOSDIR)) \
				CFGDIR=$(abspath $(CFGDIR)) 

BIOSMAKEFLAGS = TARGET=$(BIOSTARGET) \
				LIBDIR=$(abspath $(LIBDIR)) \
				SRCDIR=$(abspath $(BIOSSRCDIR)) \
				CFGDIR=$(abspath $(CFGDIR)) 

# Define Messages
# English
MSG_CLEANING = Cleaning project:
MSG_CREATEDIR = Creating directory:


# Default target.
all: check_systeminc $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) all

bios: check_systeminc $(abspath $(BIOSDIR))
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) all

installbios: bios
	$(UPLOAD) $(ULFLAGS)

upgradebios: bios
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f $(BIOSDIR)/$(BIOSTARGET).hex -r -b

defaultbios: bios
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f $(BIOSDIR)/$(BIOSTARGET).hex -r -d

install: $(BUILDDIR)/$(TARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f $< -r

reset: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -r

start: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -s

$(BUILDDIR)/$(TARGET).hex: check_systeminc $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) $(TARGET).hex

$(abspath $(BUILDDIR) $(BIOSDIR)): check_systeminc
	@echo $(MSG_CREATEDIR) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean: check_systeminc
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) clean

cleanbios: check_systeminc
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) clean

distclean: check_systeminc
	@echo $(MSG_CLEANING)
	$(RM) -r $(BUILDDIR)
	$(RM) -r $(BIOSDIR)

setconfig: check_systeminc
	@$(SETCONF) $(CANULFLAGS)

installnoappl:
	@$(ECHO) ":02000000FFFF00" > tmpfil
	@$(ECHO) ":00000001FF" >> tmpfil
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f tmpfil -r
	@$(RM) tmpfile

check_systeminc:
ifndef SETCONF
	@echo 
	@echo ERROR! This is because your system.inc is out of date
	@echo Please update system.inc.template from SVN and then diff with your system.inc and add the appropriate settings
	@echo 
	@false
endif

# Listing of phony targets.
.PHONY : all bios installbios upgradebios defaultbios \
install reset start \
clean cleanbios distclean check_systeminc setconfig installnoappl

