# Target file name (without extension).
TARGET = main
BIOSTARGET = bios

LIBDIR = ../../avr-lib
SRCDIR = src
CFGDIR = .
BIOSDIR = bios
BIOSSRCDIR = $(LIBDIR)/bios
BUILDDIR = build

include bios.inc
include ../system.inc

APPMAKEFLAGS = 	TARGET=$(TARGET) \
				LIBDIR=$(abspath $(LIBDIR)) \
				SRCDIR=$(abspath $(SRCDIR)) \
				BIOSDIR=$(abspath $(BIOSDIR)) \
				CFGDIR=$(abspath $(CFGDIR)) 

BIOSMAKEFLAGS = TARGET=$(BIOSTARGET) \
				LIBDIR=$(abspath $(LIBDIR)) \
				SRCDIR=$(abspath $(BIOSSRCDIR)) \
				CFGDIR=$(abspath $(CFGDIR)) 

# Define programs and commands.
SHELL = sh
CC = avr-gcc
AWK = awk
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
RM = rm -f
COPY = cp
MKDIR = mkdir

# Define Messages
# English
MSG_CLEANING = Cleaning project:
MSG_CREATEDIR = Creating directory:


# Default target.
all: $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) all

bios: $(abspath $(BIOSDIR))
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) all

installbios: $(BIOSDIR)/$(BIOSTARGET).hex
	$(UPLOAD) $(ULFLAGS)

upgradebios: $(BIOSDIR)/$(BIOSTARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -f $(BIOSDIR)/$(BIOSTARGET).hex -r -b

install: $(BUILDDIR)/$(TARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -f $< -r

reset: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -r

start: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -s

$(BUILDDIR)/$(TARGET).hex: $(abspath $(BUILDDIR))
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) $(TARGET).hex

$(abspath $(BUILDDIR) $(BIOSDIR)):
	@echo $(MSG_CREATEDIR) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean:
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(abspath $(SRCDIR)/Makefile) clean

cleanbios:
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(abspath $(BIOSSRCDIR)/Makefile) clean

distclean:
	@echo $(MSG_CLEANING)
	$(RM) -r $(BUILDDIR)
	$(RM) -r $(BIOSDIR)

# Listing of phony targets.
.PHONY : all bios installbios upgradebios \
install reset start \
clean cleanbios distclean

