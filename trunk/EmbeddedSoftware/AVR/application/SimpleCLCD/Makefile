PRG            = main
OBJ            = main.o\
				../lib/src/lcd/lcd_HD44780.o\
#				../lib/src/lcd/clcd20x4.o\

MCU_TARGET     = atmega88
OPTIMIZE       = -Os -mcall-prologues
# grep bios.map for __bios_ram_end to retreive this value
# In the future, this should be automatically inserted into *_app.ld at every bios compilation
BIOS_RAM_END   = 0x0080012a
#0x0080015d
ifeq ($(MCU_TARGET),atmega88)
	#uncomment this line if you're using m88, remove line when gcc-bug fixed
	EXTRA_LD        = avr-ld -m avr4 -o $(PRG).elf /usr/avr/lib/avr4/crtm88.o -L/usr/lib/gcc/avr/4.1.0/avr4 -L/usr/lib/gcc/avr/4.1.0/avr4 -L/usr/avr/lib/avr4 -Map $(PRG).map -L../AVR-Bios --defsym __bios_ram_end=$(BIOS_RAM_END) -T$(MCU_TARGET)_app.ld $(PRG).o -lgcc -lc -lgcc
endif
BIOS_RAM_END   = 0x0080012a

DEFS           = -I../AVR-Bios\
				-I../lib/src/lcd\
				-I../lib/funcdefs\

LIBS           =

# You should not have to change anything below here.

CC             = avr-gcc

# Override is only needed by avr-lib build system.
 
override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -v -Wl,-Map,$(PRG).map,-L../AVR-Bios,--defsym,__bios_ram_end=$(BIOS_RAM_END),-T$(MCU_TARGET)_app.ld
override ASFLAGS       = -x assembler-with-cpp -Wa,-gstabs -mmcu=$(MCU_TARGET) $(DEFS)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump
SIZE           = avr-size
SZFLAGS        = 
UPLOAD         = avrdude
ULFLAGS        = -p $(MCU_TARGET) -D -P /dev/ttyUSB0 -c avrispv2 -b 115200 -U flash:w:$(PRG).hex
# -D disables erase!

default: $(PRG).elf lst text
	@$(SIZE) $(SZFLAGS) $(PRG).elf

all: $(PRG).elf lst text eeprom

$(PRG).elf: $(OBJ)
ifeq ($(MCU_TARGET),atmega88)
	#remove this workaround when gcc-bug fixed
	avr-ld -m avr4 -o $(PRG).elf /usr/avr/lib/avr4/crtm88.o -L/usr/lib/gcc/avr/4.1.0/avr4 -L/usr/lib/gcc/avr/4.1.0/avr4 -L/usr/avr/lib/avr4 -Map $(PRG).map -L../AVR-Bios --defsym __bios_ram_end=$(BIOS_RAM_END) -T$(MCU_TARGET)_app.ld $(OBJ) -lgcc -lc -lgcc
else
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
endif

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak $(OBJ)
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
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

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

