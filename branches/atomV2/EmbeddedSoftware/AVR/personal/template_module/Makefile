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
all: check_binaries $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) all

bios: check_binaries $(abspath $(BIOSDIR))
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) all

installbios: bios
	$(UPLOAD) $(ULFLAGS)

upgradebios: bios
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f $(BIOSDIR)/$(BIOSTARGET).hex -r -b

install: $(BUILDDIR)/$(TARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -f $< -r

reset: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -r

start: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -i $(NODE_HW_ID) -s

$(BUILDDIR)/$(TARGET).hex: check_binaries $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) $(TARGET).hex

$(abspath $(BUILDDIR) $(BIOSDIR)): check_binaries
	@echo $(MSG_CREATEDIR) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean: check_binaries
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) clean

cleanbios: check_binaries
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) clean

distclean: check_binaries
	@echo $(MSG_CLEANING)
	$(RM) -r $(BUILDDIR)
	$(RM) -r $(BIOSDIR)

sethwid:
	@$(PERL) ../../../../PcSoftware/scripts/SetHwId/SetHwId.pl

check_binaries:
ifndef MKDIR
	@echo 
	@echo ERROR! This is because you have not defined build-binaries in system.inc
	@echo Please update system.inc.template from SVN and then copy the build binaries definitions to your system.inc
	@echo 
	@false
endif

# Listing of phony targets.
.PHONY : all bios installbios upgradebios \
install reset start \
clean cleanbios distclean check_binaries sethwid

