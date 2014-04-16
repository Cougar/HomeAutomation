# Paths: default pathnames if they are not
# passed on from parent Makefile.
ifndef TARGET
TARGET = main
endif
ifndef LIBDIR
LIBDIR = ../../../avr-lib
endif
ifndef SRCDIR
SRCDIR = ../src
endif
ifndef MODDIR
MODDIR = ../modules
endif
ifndef BIOSDIR
BIOSDIR = ../bios
endif
ifndef CFGDIR
CFGDIR = ..
endif

include $(CFGDIR)/bios.inc

# VPATH: setup search path for source files.
vpath
vpath %.c $(LIBDIR):$(SRCDIR):$(MODDIR)
vpath %.S $(LIBDIR):$(SRCDIR):$(MODDIR)

# List source files here. (C dependencies are automatically generated.)
include $(CFGDIR)/sources.inc

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
EXTRAINCDIRS = $(LIBDIR) $(BIOSDIR)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
CDEFS =

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
ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp
ASFLAGS += -Wa,-adhlns=$(@:.o=.lst),-g$(DEBUG)


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

BIOSOPTS = -Wl,-L$(BIOSDIR),-Tdata,$(BIOS_RAM_END) 

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(BIOSOPTS)

LDLIBS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
LDLIBS += -lbios
# ---------------------------------------------------------------------------

# Define programs and commands.
SHELL = sh
CC = avr-gcc
AWK = awk
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
RM = rm -f
CP = cp
MKDIR = mkdir



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

#First filter out all .a library files
SOURCES_FILTER = 
# Define all object files.
#OBJ = $(filter %.o,$(SOURCES:.c=.o) $(SOURCES:.S=.o))
OBJ = $(addsuffix .o,$(basename $(SOURCES)))

# Define all listing files.
LST = $(OBJ:.o=.lst)

# Define all dependency files.
DEP = $(OBJ:.o=.d)

BUILDDIRS = $(sort $(filter-out ./,$(dir $(OBJ))))

BIOS_RAM_END := $(shell $(AWK) '/0x[0-9a-f]+.*__bios_ram_end/ { print $$1 }' $(BIOSDIR)/bios.map)

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MMD -MP

# Default target.
all: sizebefore buildtree config.h build sizeafter

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
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi

# Generate config.h from bios.inc and config.inc
config.h: $(CFGDIR)/bios.inc $(CFGDIR)/config.inc
	@echo Generating $@
	@echo "/* This file is automatically generated. Do not edit. */" > $@
	@echo "" >> $@
	@cat $^ | $(AWK) 'BEGIN { FS="="; print "#ifndef CONFIG_H_\n#define CONFIG_H_\n" } /^[^#].*=.*/ { print "#define "$$1,$$2 } END { print "\n#endif /*CONFIG_H_*/" }' >> $@

$(CFGDIR)/config.inc: $(SRCDIR)/config.inc.template
	@if [ ! -f $@ ]; then cp $< $@; fi

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
	@echo $(MSG_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-avr $< $(TARGET).cof

extcoff: $(TARGET).elf
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	@$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo $(MSG_FLASH) $@
	@$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eep: %.elf
	@echo $(MSG_EEPROM) $@
	-@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O ihex $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo $(MSG_EXTENDED_LISTING) $@
	@$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo $(MSG_SYMBOL_TABLE) $@
	@$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
# GCC bug workaround:
# The compiler driver adds '-Tdata 0x800100' to the ld command line for some
# AVRs, making it impossible to override the .data placement with a
# subsequent -Tdata option. Detect the broken case and re-run ld with the
# correct .data location substituted.
%.elf: $(OBJ)
	@echo $(MSG_LINKING) $@
	@eval $$($(CC) -v $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) 2>&1 | $(AWK) '{ if (gsub("0x800100","$(BIOS_RAM_END)") == 1) print }')

#$(LDLIBS)

# Compile: create object files from C source files.
%.o : %.c
	@echo $(MSG_COMPILING) $<
	@$(CC) -c $(CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
%.s : %.c
	@$(CC) -S $(CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
%.o : %.S
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ASFLAGS) $< -o $@

# Buildtree: create directories for output files.
$(BUILDDIRS) :
	@echo $(MSG_CREATE) $@
	@$(MKDIR) -p $@

# Target: clean project.
clean: 
	@echo $(MSG_CLEANING)
	$(RM) $(TARGET).hex
	$(RM) $(TARGET).eep
	$(RM) $(TARGET).cof
	$(RM) $(TARGET).elf
	$(RM) $(TARGET).map
	$(RM) $(TARGET).sym
	$(RM) $(TARGET).lss
	$(RM) $(OBJ)
	$(RM) $(DEP)
	$(RM) $(LST)
	$(RM) config.h

distclean: clean
	$(RM) -r $(BUILDDIRS)

# Include the dependency files.
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(DEP)),)
-include $(DEP)
endif
endif

# Listing of phony targets.
.PHONY : all sizebefore sizeafter build \
elf hex eep lss sym coff extcoff clean \
distclean gccversion

