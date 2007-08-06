## Node specific configuration file for bios

HW_NAME=NodeEssential
HW_VERSION=2

# Uncomment to make node autostart after X time-msgs has been sent to node
# Do not choose 0 as X, also 1 is a bad choice
# This should not be used on nodes with application under developement
#AUTOSTART=5

# This number should be unique on your network
NODE_ID=0x02
# F_CPU is the clock frequency of your microcontroller, check fuses below for external/internal clock source
F_CPU=8000000UL

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

# CAN configuration
CAN_CONTROLLER=MCP2515
MCP_CLOCK_FREQ_MHZ=20
MCP_CAN_BITRATE_KBPS=250


#Some common settings of fuses
#LOWFUSE for mega88 and mega168	
#0xe7		#CKDIV8=1, external crystal
#0xe0		#CKDIV8=1, external clock   (used if mcu is clocked from can-controller, F_CPU should be 16MHz or 20MHz)
#0xe2		#CKDIV8=1, internal RC      (internal AVR-clock, F_CPU should be 8MHz)
#LOWFUSE for mega8
#0x24		#BODLEVEL = 4.0V, BODEN=enabled, internal 8MHz
#0x20		#BODLEVEL = 4.0V, BODEN=enabled, external clock
