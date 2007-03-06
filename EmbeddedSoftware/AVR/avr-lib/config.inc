## Node specific configuration file

HW_NAME=NodeEssential
HW_VERSION=1

#NODE_ID should be allocated from a database and not specified here
NODE_ID=2
F_CPU=8000000UL

MCU=atmega8
HIGHFUSE=0xd4
LOWFUSE=0x24
BOOT_START=0x1e00

#MCU=atmega88
#HIGHFUSE=0xdc
#LOWFUSE=0xe2
#EXTFUSE=0x04
#BOOT_START=0x1e00

# CAN configuration
CAN_CONTROLLER=MCP2515
MCP_CLOCK_FREQ_MHZ=16
MCP_CAN_BITRATE_KBPS=1000
