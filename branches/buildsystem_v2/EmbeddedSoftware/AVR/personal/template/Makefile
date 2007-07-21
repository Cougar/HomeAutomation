# Target file name (without extension).
TARGET = main
BIOSTARGET = bios

LIBDIR = $(abspath ../../avr-lib)
SRCDIR = $(abspath src)
CFGDIR = $(abspath .)
BIOSDIR = $(abspath bios)
BIOSSRCDIR = $(abspath $(LIBDIR)/bios)
BUILDDIR = $(abspath build)

include ../system.inc
include bios.inc

APPMAKEFLAGS = 	TARGET=$(TARGET) \
				LIBDIR=$(LIBDIR) \
				SRCDIR=$(SRCDIR) \
				BIOSDIR=$(BIOSDIR) \
				CFGDIR=$(CFGDIR) 

BIOSMAKEFLAGS = TARGET=$(BIOSTARGET) \
				LIBDIR=$(LIBDIR) \
				SRCDIR=$(BIOSSRCDIR) \
				CFGDIR=$(CFGDIR) 

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
all: $(BUILDDIR)
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(SRCDIR)/Makefile all

bios: $(BIOSDIR)
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(BIOSSRCDIR)/Makefile all

installbios: bios
	$(UPLOAD) $(ULFLAGS)

upgradebios: bios
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -f $(PRG).hex -r -b

install: $(BUILDDIR)/$(TARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -f $< -r

reset: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -r

start: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -s

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(SRCDIR)/Makefile $(TARGET).hex

$(BUILDDIR) $(BIOSDIR):
	@echo $(MSG_CREATEDIR) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean:
	@$(MAKE) $(APPMAKEFLAGS) -C $(BUILDDIR) -f $(SRCDIR)/Makefile clean

cleanbios:
	@$(MAKE) $(BIOSMAKEFLAGS) -C $(BIOSDIR) -f $(BIOSSRCDIR)/Makefile clean

distclean:
	@echo $(MSG_CLEANING)
	$(RM) -r $(BUILDDIR)
	$(RM) -r $(BIOSDIR)

# Listing of phony targets.
.PHONY : all bios installbios upgradebios \
install reset start \
clean cleanbios distclean

