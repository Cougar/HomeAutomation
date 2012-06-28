/*
 * rfid.h
 *
 *  Created on: May 6, 2012
 *      Author: pengi
 */

#ifndef RFID_H_
#define RFID_H_

#include <inttypes.h>

#define RFID_FREQ		125000L
#define RFID_BITLENGTH		32
#define RFID_OSR		4

void rfid_init( void );
uint8_t rfid_fetch( uint8_t *version, uint32_t *tag );

#endif /* RFID_H_ */
