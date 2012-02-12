# $Id$
BIOS_VERSION   = 0x1342

# Paths: set default pathnames if they are not
# passed on from parent Makefile.
ifndef TARGET
TARGET = bios
endif
ifndef LIBDIR
LIBDIR = .
endif
ifndef SRCDIR
SRCDIR = bios
endif
ifndef CFGDIR
CFGDIR = ..
endif
LIBNAME = lib$(TARGET)

include $(CFGDIR)/bios.inc
include $(CFGDIR)/../system.inc

# VPATH: setup search path for source files.
vpath
vpath %.c $(SRCDIR):$(LIBDIR)
vpath %.S $(SRCDIR):$(LIBDIR)

# List source files here. (C dependencies are automatically generated.)
SOURCES =	vectors.S \
			bios_export.c \
			bios.c \
			flash.c

ifeq ($(CAN_CONTROLLER),MCP2515)
SOURCES += drivers/can/mcp2515/mcp2515.c
CAN_DRIVER_H = \<drivers/can/mcp2515/mcp2515.h\>
endif

# Start address for linking pass 1, will be overridden for optimizing pass
#BIOS_START = 0x100

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s
#OPT = 3

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs
#DEBUG = dwarf-2

# List any extra directories to look for include files here.
# Each directory must be separated by a space.
EXTRAINCDIRS = $(LIBDIR) $(SRCDIR)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU) \
		-DCAN_DRIVER_H=$(CAN_DRIVER_H) \
		-DBIOS_VERSION=$(BIOS_VERSION)

# Place -I options here
CINCS =


# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -mmcu=$(MCU) -I.
CFLAGS += -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(@:.o=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)
CFLAGS += $(GENDEPFLAGS)



# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
##ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp
ASFLAGS += -Wa,-g$(DEBUG)


#Additional libraries.

# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

PRINTF_LIB = 

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

SCANF_LIB = 

MATH_LIB = -lm

# External memory options

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

BIOS_START = $(shell $(AWK) '/0x[0-9a-f]+.*__bios_optimal_start/ { print $$1 }' bios.map)
BIOSOPTS = -Wl,--defsym,__bios_start=$(BIOS_START),--defsym,__boot_start=$(BOOT_START),-T$(LIBDIR)/ldscripts/$(MCU).ld

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(BIOSOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)

# ---------------------------------------------------------------------------

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_DEPEND = Generating dependencies:
MSG_CLEANING = Cleaning project:
MSG_CREATE = Creating:

# Define all object files.
OBJ = $(filter %.o,$(SOURCES:.c=.o) $(SOURCES:.S=.o))

# Define all listing files.
LST = $(OBJ:.o=.lst)

# Define all dependency files.
DEP = $(OBJ:.o=.d)

BUILDDIRS = $(sort $(filter-out ./,$(dir $(OBJ))))

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# Default target.
all: check_binaries sizebefore buildtree build lib sizeafter

build: elf hex eep lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym

buildtree: $(BUILDDIRS)

# Display size of file.
HEXSIZE = $(SIZE) --target=ihex $(TARGET).hex
ELFSIZE = $(SIZE) $(TARGET).elf
sizebefore:
	@if [ -f $(TARGET).elf ]; then $(ECHO); $(ECHO) $(MSG_SIZE_BEFORE); $(ELFSIZE); $(ECHO); fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then $(ECHO); $(ECHO) $(MSG_SIZE_AFTER); $(ELFSIZE); $(ECHO); fi

# Generate config.h from bios.inc
#HWID = `$(UUIDGEN) | $(MD5SUM) | $(CUT) -c 1-8`
HWID := $(shell $(UUIDGEN) | $(MD5SUM) | $(CUT) -c 1-8)
config.h: $(CFGDIR)/bios.inc
	@$(ECHO) Generating $@
	@$(CAT) $^ | $(AWK) -v apa=0x$(HWID) 'BEGIN {  } /.*/ {sub(/^NODE_HW_ID=<GENERATE_HW>/,"NODE_HW_ID="apa)}{print} END {  }' > tmpfile
	@$(CAT) tmpfile > $^
	@$(RM) tmpfile
	@$(ECHO) "/* This file is automatically generated. Do not edit. */" > $@
	@$(ECHO) "" >> $@
	@$(ECHO) "#ifndef CONFIG_H_" >> $@
	@$(ECHO) "#define CONFIG_H_" >> $@
	@$(ECHO) "" >> $@
	@$(CAT) $^ | $(AWK) 'BEGIN { FS="=" } /^[^#].*=.*/ { print "#define "$$1,$$2 } END {  }' >> $@
	@$(ECHO) "" >> $@
	@$(ECHO) "#endif /*CONFIG_H_*/" >> $@

# above echos does not use \n due to different handling on different os:es?
#	@$(CAT) $^ | $(AWK) -v apa=0x$(HWID) 'BEGIN {  } /.*/ {sub(/^NODE_HW_ID=<GENERATE_HW>/,"NODE_HW_ID="apa)}{print} END {  }' > $^

#	@$(CAT) $^ | $(GREP) "<GENERATE_HW>" | $(AWK) -v apa=0x$(HWID) 'BEGIN { FS="=" } /^[^#].*=.*/ { print "#define "$$1,apa } END {  }' >> $@
#	@$(CAT) $^ | $(GREP) -v "<GENERATE_HW>" | $(AWK) 'BEGIN { FS="=" } /^[^#].*=.*/ { print "#define "$$1,$$2 } END {  }' >> $@
	
# Display compiler version information.
gccversion : 
	@$(CC) --version

# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


coff: $(TARGET).elf
	@$(ECHO) $(MSG_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-avr $< $(TARGET).cof

extcoff: $(TARGET).elf
	@$(ECHO) $(MSG_EXTENDED_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@$(ECHO) $(MSG_FLASH) $@
	@$(OBJCOPY) -j .text -j .data -j .bootloader -O ihex $< $@

%.eep: %.elf
	@$(ECHO) $(MSG_EEPROM) $@
	-@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@$(ECHO) $(MSG_EXTENDED_LISTING) $@
	@$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@$(ECHO) $(MSG_SYMBOL_TABLE) $@
	@$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
#.PRECIOUS : $(OBJ)
$(TARGET)_pass1.elf: override BIOS_START=0x100
$(TARGET)_pass1.elf: config.h $(OBJ)
	@$(ECHO) Linking pass 1:
	@$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) $(LIBS)

$(TARGET).elf: config.h $(OBJ) | $(TARGET)_pass1.elf
	@$(ECHO) Linking pass 2: Optimizing flash usage
	@$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) $(LIBS)

$(LIBNAME).S: $(TARGET).elf
	@$(ECHO) Generating $@
	@$(AWK) '$$1 ~ /0x[0-9a-f]+/ && /BIOS_/ { print "\t.set "$$2","$$1"\n\t.global "$$2 }' $(TARGET).map > $@

bios.h: $(LIBDIR)/drivers/can/can.h $(LIBDIR)/bios/bios_export.h
	@$(ECHO) Generating $@
	@$(ECHO) "/* This file is automatically generated. Do not edit. */" > $@
	@$(ECHO) "" >> $@
	@$(ECHO) "#ifndef BIOS_H_" >> $@
	@$(ECHO) "#define BIOS_H_" >> $@
	@$(ECHO) "" >> $@
	@$(CAT) $^ >> $@
	@$(ECHO) "" >> $@
	@$(ECHO) "#endif /*BIOS_H_*/" >> $@

lib: bios.h $(LIBNAME).a($(LIBNAME).o)

# Compile: create object files from C source files.
%.o : %.c
	@$(ECHO) $(MSG_COMPILING) $<
	@$(CC) -c $(CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
%.s : %.c
	@$(CC) -S $(CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
%.o : %.S
	@$(ECHO) $(MSG_ASSEMBLING) $<
	@$(CC) -c $(ASFLAGS) $< -o $@

# Depends: create dependency files from c source files.
#%.d : %.c | $(BUILDDIRS)
#	@$(ECHO) $(MSG_DEPEND) $<
#	@$(CC) -c $(CFLAGS) $(GENDEPFLAGS) $<

# Buildtree: create directories for output files.
$(BUILDDIRS) :
	@$(ECHO) $(MSG_CREATE) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean: 
	@$(ECHO) $(MSG_CLEANING)
	$(RM) $(TARGET).eep
	$(RM) $(TARGET).cof
	$(RM) $(TARGET)_pass1.elf
	$(RM) $(TARGET).elf
	$(RM) $(TARGET).sym
	$(RM) $(TARGET).lss
	$(RM) $(OBJ)
	$(RM) $(DEP)
	$(RM) $(LST)
	$(RM) $(LIBNAME).S
	$(RM) config.h
	$(RM) -r drivers

check_binaries:
ifndef AWK
	@echo 
	@echo ERROR! This is because you have not defined build-binaries in system.inc
	@echo Please update system.inc.template from SVN and then copy the build binaries definitions to your system.inc
	@echo 
	@false
endif

# Include the dependency files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEP)),)
-include $(DEP)
endif
endif

# Listing of phony targets.
.PHONY : all sizebefore sizeafter build \
elf hex eep lss sym coff extcoff clean \
gccversion lib check_binaries

