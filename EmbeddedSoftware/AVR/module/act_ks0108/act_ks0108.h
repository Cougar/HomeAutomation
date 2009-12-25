#ifndef ACT_KS0108
#define ACT_KS0108

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/timer/timer.h>

#include <drivers/lcd/glcd/ks0108.h>
#include <avr/pgmspace.h>

#if act_ks0108_USE_EXTERNAL_EEPROM==1
  #include <drivers/mcu/TWI_Master.h>
  #define MEMORY_ADDR  0xA0	// For first 256 bytes of EEPROM
  #define MESSAGEBUF_SIZE       131		// Three more than the memory block length
#endif

void act_ks0108_Init(void);
void act_ks0108_Process(void);
void act_ks0108_HandleMessage(StdCan_Msg_t *rxMsg);
void act_ks0108_List(uint8_t ModuleSequenceNumber);

#endif // ACT_KS0108
