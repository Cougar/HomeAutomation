/*
 * main.c
 *
 *  Created on: May 5, 2012
 *      Author: pengi
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "config.h"
#include "serial.h"
#include "rfid.h"
#include "timer.h"

int main( void ) {
	uint8_t version, card_avalible = 0;
	uint32_t id;
	rfid_init();
	Timer_Init();
	sei();
	printf( "Start...\n" );
	for(;;){
		if( 0 == rfid_fetch( &version, &id ) ) {
			if( !card_avalible ) { /* FIXME: change card without left-timeout */
				printf( "\nGot %02x%08lx...", version, id );
			}
			Timer_SetTimeout(TIMER_CARD_LEFT, CARD_LEFT_TIMEOUT, TimerTypeOneShot, NULL);
			card_avalible = 1;
		}
		if( Timer_Expired( TIMER_CARD_LEFT ) ) {
			printf( " Left..." );
			card_avalible = 0;
		}
	}
}
