PRG            = bios
OBJ            = vectors.o bios.o flash.o
MCU_TARGET     = atmega8
OPTIMIZE       = -Os -mcall-prologues
BIOS_START     = 0x1000
BOOT_START     = 0x1f00

DEFS           =
LIBS           =

TARGET_CLK_SRC = 0xe2
#TARGET_CLK_SRC for mega88		
#0xe7		#CKDIV8=1, external crystal
#0xe0		#CKDIV8=1, external clock
#0xe2		#CKDIV8=1, internal RC
#TARGET_CLK_SRC for mega8
#0x24		#BODLEVEL = 4.0V, BODEN=enabled, internal 8MHz
#0x20		#BODLEVEL = 4.0V, BODEN=enabled, external clock

#
# You should not have to change anything below here.
#

CC             = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map,--defsym,__bios_start=$(BIOS_START),--defsym,__boot_start=$(BOOT_START),-T$(MCU_TARGET)_bios.ld,--cref
override ASFLAGS       = -x assembler-with-cpp -Wa,-gstabs -mmcu=$(MCU_TARGET) $(DEFS)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size
SZFLAGS        = 
UPLOAD         = avrdude
ULFLAGS        = -p $(MCU_TARGET) -P /dev/ttyUSB0 -c avrispv2 -b 115200 -U flash:w:$(PRG).hex

ifeq ($(MCU_TARGET),atmega88)
#BOOTRST=0, BOOTSZ0=1, BOOTSZ1=1
EXTFUSE  = 0x06
#BODLEVEL = 4.3V
HIGHFUSE = 0xdc
LOWFUSE  = $(TARGET_CLK_SRC)
endif

ifeq ($(MCU_TARGET),atmega8)
#BOOTRST=0, BOOTSZ0=1, BOOTSZ1=1
HIGHFUSE = 0xd6
LOWFUSE  = $(TARGET_CLK_SRC)
endif

AVRDUDE_WRITE_FUSEBITS = -u
ifdef EXTFUSE
AVRDUDE_WRITE_FUSEBITS += -U efuse:w:$(EXTFUSE):m
endif
ifdef HIGHFUSE
AVRDUDE_WRITE_FUSEBITS += -U hfuse:w:$(HIGHFUSE):m
endif
ifdef LOWFUSE
AVRDUDE_WRITE_FUSEBITS += -U lfuse:w:$(LOWFUSE):m
endif

#comment this line for not programming fusebits
ULFLAGS      += $(AVRDUDE_WRITE_FUSEBITS)

default: $(PRG).elf lst
	@$(SIZE) $(SZFLAGS) $(PRG).elf

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -j .bootloader -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -j .bootloader -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -j .bootloader -O binary $< $@

# Rules for uploading rom images to flash

install: hex
	$(UPLOAD) $(ULFLAGS)

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.

FIG2DEV                 = fig2dev
EXTRA_CLEAN_FILES       = *.hex *.bin *.srec

dox: eps png pdf

eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf

%.eps: %.fig
	$(FIG2DEV) -L eps $< $@

%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@

%.png: %.fig
	$(FIG2DEV) -L png $< $@

