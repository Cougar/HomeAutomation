/**
 * Configuration file .
 */

#ifndef ENC28J60_CFG_H_
#define ENC28J60_CFG_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
 
 /**
 * ENC28J60 Chip Select port on avr. 
 */ 
 
#define ENC28J60_CONTROL_PORT   PORTB
#define ENC28J60_CONTROL_DDR    DDRB
#define ENC28J60_CONTROL_CS     1

#endif /*ENC28J60_CFG_H_*/
