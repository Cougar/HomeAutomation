/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 *
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : main.c
 * Version : 0.9
 * 
 * Summary : Application Entry
 *           Test-Platform: Atmel ATmega + Microchip MCP2515
 *           Here the MCP2515 is clocked by the AVR (see clock_init()).
 *
 * This application uses Peter Fleury's free AVR-U(S)ART library which
 * has been slightly extended. See comments in uart.c.
 *
 *****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "uart.h"
#include "spi.h"
#include "can.h"
#include "delay.h"
#include "termio.h"
#include "terminal.h"
#include "gpio.h"
#include "timebase.h"
#include "debughelper.h"

// #define VERBOSE (1)

#define OSCAL_READEEPROM (1)

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__)
#define OSCAL_EEPROMADDR ((void*)0x1ff)
#elif defined(__AVR_ATmega32__)
#define OSCAL_EEPROMADDR ((void*)0x3ff)
#else
#error "eeprom addr. undefined for this target"
#endif

#define OSCAL_VALUE (0xA5)

#define BAUD 19200

#define AUTOSENDPORT PORTD
#define AUTOSENDDDR  DDRD
#define AUTOSENDPIN  PIND
#define AUTOSENDBIT  PD7

#if 0
void clock_init_tc2(void)
// to clock the MCP2515
// - CTC Mode of Timer-Counter 2
// - Freq at OC2 (datasheet): f=f_c/(2*N*(1+OCR2))
//   with: f_c=system clock=8MHz, N=prescaler=1, OCR2=0 -> 4MHz @ Pin D7
{
	// set OC2-Pin (PD7 on ATmega16) as output
	DDRD |= _BV(DDD7);
	// CTC limit in OCR2
	OCR2=0;
	// Clear Timer on Compare Match (CTC) mode; toogle OC2(PD7) on compare match;
	//  clock select (timer2 prescaler) to clk/1 (no prescaling CS20=1)
	TCCR2=_BV(WGM21)|_BV(COM20)|_BV(CS20);
}
#endif

void clock_init_tc1a(void)
// to clock the MCP2515 on Mega8/Mega32
// - CTC Mode of Timer-Counter 1 use OC1A as clock-pin
// - Freq at OC2 (datasheet): f=f_c/(2*N*(1+OCR2))
//   with: f_c=system clock=8MHz, N=prescaler=1, OCR1A=0 -> 4MHz 
//   @ Pin B1 Mega8
//   @ Pin D5 Mega32
{
	#if defined(__AVR_ATmega8__)
	// set OC2-Pin (PB1 on ATmega8) as output
	DDRB |= _BV(DDB1);
	#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
	// set OC2-Pin (PD5 on ATmega32) as output
	DDRD |= _BV(DDD5);
	#else
	#error "OCR1A pin undefined for this target"
	#endif
	
	// CTC limit in OCR1A
	OCR1A=0;
	// Clear Timer on Compare Match (CTC) mode; toogle OC1A(PB1) on compare match; 
	TCCR1A=_BV(COM1A0);
	// clock select (timer2 prescaler) 
	TCCR1B=_BV(CS10)|_BV(WGM12); // to clk/1 (no prescaling CS10=1)
}

static void process_CANMessages(void)
{
	CanMessage msg;
	uint8_t data2;

#ifdef VERBOSE
	term_puts_P("AutoProcessing\n");
#endif
	while ( can_readMessage(&msg) == CAN_OK ) {
		data2 = msg.dta[2];
		if ( data2<3 ) {
			gpio_toggle_led( (1<<data2) );
#ifdef VERBOSE
			Debug_ByteToUart_P("data2",data2);
#endif
		}
	}	
}

int main(void)
{
	uint8_t res, flag;
	uint8_t shellstate;
	uint16_t timebuf;

#if OSCAL_READEEPROM
	res = eeprom_read_byte(OSCAL_EEPROMADDR);
	if (res != 0xff) OSCCAL = res;
	else OSCCAL = OSCAL_VALUE; // take default from define
#else
	OSCCAL = OSCAL_VALUE;
#endif
	
	gpio_init();
	gpio_set_led(LED1);
	
	timebase_init();
	
	uart_init((UART_BAUD_SELECT((BAUD),F_OSC)));
	
	sei();
	
	term_puts_P( "\n\nMCP2515 Interface Demo  (c) Martin Thomas\n" );
	Debug_ByteToUart_P("OSCCAL", OSCCAL);
	
	term_puts_P("\nSetting up MCP clock to FOSC/2\n");
	clock_init_tc1a();	// init MCP2515 clock-source (=AVR FOSC/2)
	// old:  clock_init(); */
	
	term_puts_P("SPI init\n");
	spi_init();		// init SPI-Interface (as "Master")
	
	term_puts_P("CAN init ");
	// res = can_init(CAN_125KBPS);
	res = can_init(CAN_20KBPS);
	if (res == CAN_OK) term_puts_P("- OK\n");
	else term_puts_P("- FAIL\n");
	
	
	term_puts_P("Autosend (\"Autosendbit\"-State) : ");
	AUTOSENDDDR &= ~(1<<AUTOSENDBIT); // as input
	AUTOSENDPORT |= (1<<AUTOSENDBIT); // int pullup on
	if ( AUTOSENDPIN & (1<<AUTOSENDBIT) ) {
		gTimedSend = CANAUTOOFF;
		term_puts_P("off");
	}
	else {
		gTimedSend = CANAUTOON;
		term_puts_P("on");
	}
	term_puts_P("\n");
	
	can_debug1();
	
	shellstate = COMMANDSHELL_MODEINIT;
	
	flag = 0;
	timebuf = timebase_actTime();
	
#if 0
	for (;;) {
		if (timebase_passedTimeMS(timebuf)>=1000) {
			timebuf = timebase_actTime();
			term_puts_P("(.)");
		}
	}
#endif
	
	for (;;) {
		CommandShell(&shellstate);
		if ( shellstate == COMMANDSHELL_MODEEXIT ) {
			term_puts_P("I have no operation system to exit to :-( -> WD-Reset");
			wdt_enable ( WDTO_500MS );
			while(1);
		}
		
		if ( ( can_checkReceive() == CAN_MSGAVAIL ) ) {
			if (!flag) {
#ifdef VERBOSE
				term_puts_P("\nAt least one CAN-Message available!\n");
				shellstate = COMMANDSHELL_REDRAW;
#endif
				flag = 1;
			}
			if ( gCANAutoProcess == CANAUTOON ) {
				process_CANMessages();
#ifdef VERBOSE
				shellstate = COMMANDSHELL_REDRAW;
#endif
			}
		}
		else {
			flag = 0;
		}
		
		if (timebase_passedTimeMS(timebuf)>=1000) {
			timebuf = timebase_actTime();
			if (gTimedSend == CANAUTOON) {
				if ( can_testTransmit(CAN_STDID, 1) != CAN_OK ) {
					term_puts_P("\nAuto-Transmit (debug) failed\n");
					can_dumpStatus();
					shellstate = COMMANDSHELL_REDRAW;
				}
			}
			if ( can_checkError() != CAN_OK ) {
				can_dumpStatus();
			}
		}
		
		// else term_puts_P("Checking\n");
	}
	
	return 0;
}
