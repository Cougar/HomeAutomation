# Target file name (without extension).
TARGET = main

LIBDIR = ../../avr-lib
SRCDIR = src
BIOSDIR = bios
CFGDIR = .
BUILDDIR = build

include $(LIBDIR)/system.inc
include $(BIOSDIR)/bios.inc

MAKEFLAGS += 	TARGET=$(TARGET) \
				LIBDIR=../$(LIBDIR) \
				SRCDIR=../$(SRCDIR) \
				BIOSDIR=../$(BIOSDIR) \
				CFGDIR=../$(CFGDIR) 

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
	@$(MAKE) -C $(BUILDDIR) -f $(SRCDIR)/Makefile all

install: $(BUILDDIR)/$(TARGET).hex
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -f $< -r

reset: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -r

start: 
	$(CANUPLOAD) $(CANULFLAGS) -n $(NODE_ID) -s

$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)
	@$(MAKE) -C $(BUILDDIR) -f $(SRCDIR)/Makefile $(TARGET).hex

$(BUILDDIR):
	@echo $(MSG_CREATEDIR) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean:
	@$(MAKE) -C $(BUILDDIR) -f $(SRCDIR)/Makefile clean

distclean:
	@echo $(MSG_CLEANING)
	$(RM) -r $(BUILDDIR)

# Listing of phony targets.
.PHONY : all clean distclean install reset start

