## Node specific configuration file for bios

HW_NAME=CoreCard
HW_VERSION=1

_AVRLIB_BIOS_=1

# Uncomment to make node autostart after X time-msgs has been sent to node
# Do not choose 0 as X, also 1 is a bad choice
# This should not be used on nodes with application under developement
AUTOSTART=5

# Node hardware id, this number should be unique on your network
# leave as <GENERATE_HW> at first compile, the build will then automatically set it to whatever gets generated (see config.h)
# Never set this manually if you are not sure what you are doing
NODE_HW_ID=<GENERATE_HW>

# example:
#NODE_HW_ID=0x12345678 

# This number should be unique on your network (for nodes with HW_ID it's not clear if this id is needed)

# F_CPU is the clock frequency of your microcontroller, check fuses below for external/internal clock source
F_CPU=8000000UL

# Device parameters
MCU=atmega168
HIGHFUSE=0xdc
LOWFUSE=0xe2
EXTFUSE=0x02
BOOT_START=0x3c00

# CAN configuration (old nodes: PORTB, DDRB, PB2)
CAN_CONTROLLER=MCP2515
MCP_CS_PORT=PORTC
MCP_CS_DDR=DDRC
MCP_CS_BIT=PC3
MCP_CAN_BITRATE_KBPS=250
MCP_CLOCK_FREQ_MHZ=20
MCP_CLOCK_PRESC=1



################################################################################
# Everything below is just help
################################################################################

# For 				MCP_CLOCK_FREQ_MHZ=20	MCP_CLOCK_FREQ_MHZ=16
# MCP_CLOCK_PRESC		external clock			external clock
# 		1 					20MHz					16MHz 
# 		2					10MHz					8MHz
# 		4					5MHz					4MHz
# 		8					2.5MHz					2MHz
# Make sure to change F_CPU accordingly if using external clock on AVR (LOWFUSE)


#---MEGA8---
#MCU=atmega8
#HIGHFUSE=0xd4
#LOWFUSE=0x24
#BOOT_START=0x1e00

#---MEGA88---
#MCU=atmega88
#HIGHFUSE=0xdc
#LOWFUSE=0xe2
#EXTFUSE=0x04
#BOOT_START=0x1e00

#---MEGA168---
#MCU=atmega168
#HIGHFUSE=0xdc
#LOWFUSE=0xe2
#EXTFUSE=0x02
#BOOT_START=0x3c00

#---MEGA328P---
#MCU=atmega328p
#HIGHFUSE=0xdc
#LOWFUSE=0xe2
#EXTFUSE=0x04
#BOOT_START=0x7c00

#Force bios to setup ports according below
#INITDDRB=0x0
#INITDDRC=0x0
#INITDDRD=0x0
#INITPORTB=0x0
#INITPORTC=0x0
#INITPORTD=0x0
#INITPINB=0x0
#INITPINC=0x0
#INITPIND=0x0

#Some common settings of fuses
#LOWFUSE for mega88 and mega168	
#0xe7		#CKDIV8=1, external crystal
#0xe0		#CKDIV8=1, external clock   (used if mcu is clocked from can-controller, F_CPU should be 16MHz or 20MHz)
#0xe2		#CKDIV8=1, internal RC      (internal AVR-clock, F_CPU should be 8MHz)
#LOWFUSE for mega8
#0x24		#BODLEVEL = 4.0V, BODEN=enabled, internal 8MHz
#0x20		#BODLEVEL = 4.0V, BODEN=enabled, external clock
