/*
 * rfid.c
 *
 *  Created on: May 6, 2012
 *      Author: pengi
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <drivers/mcu/gpio.h>
#include "rfid.h"
#include "config.h"

/* edge detection, for resynchronization */
volatile uint8_t rfid_m_lastbit = 0;

/* number of timer steps left until next sample */
volatile uint8_t rfid_m_nextsample = 0;

/* Current phase in manchester encoding */
volatile uint8_t rfid_m_phase = 0;

/* stores last bit, used for manchester decoding */
volatile uint8_t rfid_m_buffer = 0;

/* Circular buffer for reading */
volatile uint8_t rfid_data[8];
volatile uint8_t rfid_ptr = 0;

/* Output */
volatile uint8_t rfid_pkt[8];
volatile uint8_t rfid_trig = 0;

/* Parse tables */
const uint8_t rfid_chk_valid[32] PROGMEM = {
		1, // 00000
		0, // 00001
		0, // 00010
		1, // 00011
		0, // 00100
		1, // 00101
		1, // 00110
		0, // 00111
		0, // 01000
		1, // 01001
		1, // 01010
		0, // 01011
		1, // 01100
		0, // 01101
		0, // 01110
		1, // 01111
		0, // 10000
		1, // 10001
		1, // 10010
		0, // 10011
		1, // 10100
		0, // 10101
		0, // 10110
		1, // 10111
		1, // 11000
		0, // 11001
		0, // 11010
		1, // 11011
		0, // 11100
		1, // 11101
		1, // 11110
		0  // 11111
};

void rfid_handlebit( uint8_t bit );

static inline uint32_t rfid_bitrev32(uint32_t x);
static inline uint8_t rfid_bitrev8(uint8_t x);

static inline uint32_t rfid_bitrev32(uint32_t x) {
	x = (x >> 16) | (x << 16);
	x = (x >> 8 & 0x00ff00ff) | (x << 8 & 0xff00ff00);
	x = (x >> 4 & 0x0f0f0f0f) | (x << 4 & 0xf0f0f0f0);
	x = (x >> 2 & 0x33333333) | (x << 2 & 0xcccccccc);
	x = (x >> 1 & 0x55555555) | (x << 1 & 0xaaaaaaaa);
	return x;
}
static inline uint8_t rfid_bitrev8(uint8_t x) {
	x = (x >> 4 & 0x0f) | (x << 4 & 0xf0);
	x = (x >> 2 & 0x33) | (x << 2 & 0xcc);
	x = (x >> 1 & 0x55) | (x << 1 & 0xaa);
	return x;
}

ISR( TIMER1_COMPA_vect ) {
	gpio_toggle_pin( EXP_M );
	uint8_t bit = gpio_get_state( sns_rfid_READ_PIN );

	if( rfid_m_lastbit != bit ) {
		rfid_m_nextsample = RFID_OSR/2;
	}
	if( rfid_m_nextsample == 0 ) {
		rfid_m_nextsample = RFID_OSR;

		/* Sample */
		if( rfid_m_phase == 0 ) {
			rfid_m_buffer = bit;
			rfid_m_phase = 1;
		} else {
			if( rfid_m_buffer == bit ) { /* Out of phase, resynchroinzation */
				rfid_m_buffer = bit;
				rfid_m_phase = 1;
			} else {
				/* Second phase in manchester encoded bit, pass to bitstream */
				rfid_handlebit( !bit ); /* First bit counts */
				rfid_m_phase = 0;
			}
		}
	}
	rfid_m_nextsample--;
	rfid_m_lastbit = bit;
}

void rfid_handlebit( uint8_t bit ) {

	if( bit ) {
		rfid_data[rfid_ptr>>3] |= 1<<(rfid_ptr&7);
	} else {
		rfid_data[rfid_ptr>>3] &= ~(1<<(rfid_ptr&7));
	}
	rfid_ptr = (rfid_ptr+1)%64;

	if( rfid_trig ) return; /* Ignore if still trigged, someone might parse */

	rfid_pkt[0] = (rfid_data[(0+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(1+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	if( rfid_pkt[0] != 0xFE ) return;
	rfid_pkt[1] = (rfid_data[(1+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(2+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[2] = (rfid_data[(2+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(3+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[3] = (rfid_data[(3+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(4+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[4] = (rfid_data[(4+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(5+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[5] = (rfid_data[(5+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(6+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[6] = (rfid_data[(6+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(7+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));
	rfid_pkt[7] = (rfid_data[(7+(rfid_ptr>>3))%8]>>(rfid_ptr&7)) | rfid_data[(0+(rfid_ptr>>3))%8]<<(8-(rfid_ptr&7));

	rfid_trig = 1;
}

void rfid_init( void ) {
	DDRD |= (1<<PORTD5); // Port out
	TCCR0A = (1<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(1<<WGM00);
	TCCR0B = (1<<WGM02)|(2<<CS00); // Fast PWM OCR0A top, prescaler 8
	OCR0A = (F_CPU/RFID_FREQ/8)-1; // 125kHz
	OCR0B = (F_CPU/RFID_FREQ/8)/2; // 50% dutycycle

	TCCR1A = (0<<WGM11)|(0<<WGM10);
	TCCR1B = (0<<WGM13)|(1<<WGM12)|(2<<CS20); // CTC OCR1A top, prescaler 8
	OCR1A = (RFID_BITLENGTH*F_CPU/RFID_OSR/RFID_FREQ/8)-1;
	TIMSK1 = (1<<OCIE1A);

	gpio_set_in( sns_rfid_READ_PIN );

	gpio_set_out( EXP_M );
}

uint8_t rfid_fetch( uint8_t *version, uint32_t *tag ) {
	uint8_t colchksum;

	if( rfid_trig == 0 ) return 1;


	if( (rfid_pkt[0] & 0xFF) != 0xFE ) goto rfid_fetchfail;
	if( (rfid_pkt[1] & 0x03) != 0x03 ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[1]&0x7C)>>2)                           ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[1]&0x80)>>7) + ((rfid_pkt[2]&0x0F)<<1) ) ) goto rfid_fetchfail;

	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[2]&0xF0)>>4) + ((rfid_pkt[3]&0x01)<<4) ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[3]&0x3E)>>1)                           ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[3]&0xC0)>>6) + ((rfid_pkt[4]&0x07)<<2) ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[4]&0xF8)>>3)                           ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[5]&0x1F)   )                           ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[5]&0xE0)>>5) + ((rfid_pkt[6]&0x03)<<3) ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[6]&0x7C)>>2)                           ) ) goto rfid_fetchfail;
	if( !pgm_read_byte(rfid_chk_valid + ((rfid_pkt[6]&0x80)>>7) + ((rfid_pkt[7]&0x0F)<<1) ) ) goto rfid_fetchfail;

	*version = ((rfid_pkt[1]&0x3C)>>2)    // ..####.. => ....####
			 | ((rfid_pkt[1]&0x80)>>3)    // #....... => ...#....
			 | ((rfid_pkt[2]&0x07)<<5);   // .....### => ###.....

	*tag     = ((uint32_t)(rfid_pkt[2]&0xF0)>> 4)    // ####.... => ........ ........ ........ ....####
			 | ((uint32_t)(rfid_pkt[3]&0x1E)<< 3)    // ...####. => ........ ........ ........ ####....
			 | ((uint32_t)(rfid_pkt[3]&0xC0)<< 2)    // ##...... => ........ ........ ....--## ........
			 | ((uint32_t)(rfid_pkt[4]&0x03)<<10)    // ......## => ........ ........ ....##-- ........
			 | ((uint32_t)(rfid_pkt[4]&0x78)<< 9)    // .####... => ........ ........ ####.... ........
			 | ((uint32_t)(rfid_pkt[5]&0x0F)<<16)    // ....#### => ........ ....#### ........ ........
			 | ((uint32_t)(rfid_pkt[5]&0xE0)<<15)    // ###..... => ........ -###.... ........ ........
			 | ((uint32_t)(rfid_pkt[6]&0x01)<<23)    // .......# => ........ #---.... ........ ........
			 | ((uint32_t)(rfid_pkt[6]&0x3C)<<22)    // ..####.. => ....#### ........ ........ ........
			 | ((uint32_t)(rfid_pkt[6]&0x80)<<21)    // #....... => ---#.... ........ ........ ........
			 | ((uint32_t)(rfid_pkt[7]&0x07)<<29);   // .....### => ###-.... ........ ........ ........

	colchksum  = *version;
	colchksum ^= (*tag >>  0) & 0xFF;
	colchksum ^= (*tag >>  8) & 0xFF;
	colchksum ^= (*tag >> 16) & 0xFF;
	colchksum ^= (*tag >> 24) & 0xFF;
	colchksum ^= colchksum>>4;
	colchksum ^= rfid_pkt[7]>>4;
	colchksum &= 0x0F;

	if( colchksum != 0 ) goto rfid_fetchfail;

	*version = rfid_bitrev8( *version );
	*tag = rfid_bitrev32( *tag );

	rfid_trig = 0;
	return 0;

rfid_fetchfail:
	rfid_trig = 0;
	return 2;
}
