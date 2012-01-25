#ifndef _DHT11wire_h_
#define _DHT11wire_h_

#include <inttypes.h>
#include <drivers/mcu/gpio.h>
#include <drivers/can/canprintf.h>
#include "config.h"
/*******************************************/
/* Hardware connection                     */
/*******************************************/

/* Define DHT11_ONE_BUS if only one 1-Wire-Bus is used
   in the application -> shorter code.
   If not defined make sure to call dht11_set_bus() before using
   a bus. Runtime bus-select increases code size by around 300 
   bytes so use DHT11_ONE_BUS if possible */
#define DHT11_ONE_BUS

#ifdef DHT11_ONE_BUS

#ifndef DHT11_PIN
  #warning DHT11_PIN shall be defined in config.inc, using DHT11_PIN=EXP_F...
  #define DHT11_PIN  EXP_F
#endif

#define DHT11_CONF_DELAYOFFSET 0

#else
#error The lib does not support not defining DHT11_ONE_BUS
#if F_CPU<1843200
 #warning | experimental multi-bus-mode is not tested for 
 #warning | frequencies below 1,84MHz - use DHT11_ONE_WIRE or
 #warning | faster clock-source (i.e. internal 2MHz R/C-Osc.)
#endif
#define DHT11_CONF_CYCLESPERACCESS 13
#define DHT11_CONF_DELAYOFFSET ( (uint16_t)( ((DHT11_CONF_CYCLESPERACCESS)*1000000L) / F_CPU  ) )
#endif

/*******************************************/

extern uint8_t dht11_start(void);
extern uint8_t dht11_readData(uint8_t* data);


#ifndef DHT11_ONE_BUS
extern void dht11_set_bus(volatile uint8_t* in,
	volatile uint8_t* out,
	volatile uint8_t* ddr,
	uint8_t pin);
#endif

#endif

