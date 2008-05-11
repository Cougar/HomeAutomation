#ifndef CONFIG_H_
#define CONFIG_H_

#define HW_NAME CoreCard
#define HW_VERSION 1
#define _AVRLIB_BIOS_ 1
#define AUTOSTART 5
#define NODE_HW_ID 0xb7900b1f
#define F_CPU 8000000UL
#define MCU atmega168
#define HIGHFUSE 0xdc
#define LOWFUSE 0xe2
#define EXTFUSE 0x02
#define BOOT_START 0x3c00
#define CAN_CONTROLLER MCP2515
#define MCP_CS_PORT PORTC
#define MCP_CS_DDR DDRC
#define MCP_CS_BIT PC3
#define MCP_CAN_BITRATE_KBPS 250
#define MCP_CLOCK_FREQ_MHZ 20
#define MCP_CLOCK_PRESC 1
#define TIMER_NUM_TIMERS 3
#define TIMER_USE_TIMER 2
#define TIMER_TICKFREQ 1000
#define APP_HEARTBEAT_TIMER 0
#define STDCAN_HEARTBEAT_PERIOD 10000
#define NUMBEROFMODULES 1
#define sns_ds18x20_ID 1
#define sns_ds18x20_SEND_PERIOD 20000
#define sns_ds18x20_SEND_DELAY 500
#define sns_ds18x20_SEARCH_TIMEOUT 5
#define sns_ds18x20_TIMER 1

#endif /*CONFIG_H_*/
