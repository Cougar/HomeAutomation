/**
 * CAN test program.
 * 
 * @target	AVR
 * @date	2006-10-29
 * @author	Jimmy Myhrman
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
/* lib files */
#include <can.h>
#include <uart.h>
#include <timebase.h>


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define UART_START_BYTE 253
#define UART_END_BYTE 250


/*-----------------------------------------------------------------------------
 * Functions
 *---------------------------------------------------------------------------*/
/**
 * Parses an incoming UART byte by maintaining a state machine. The UART
 * bytes will build up a CAN message according to the protocol described here:
 * 
 * http://www.arune.se/projekt/doku.php?id=homeautomation:pc-mjukvara
 * 
 * @param c
 * 		The received UART byte.
 */
void UartParseByte(uint8_t c) {
	static CanMessage_t cm;
	static uint8_t waitingMessage = 0;
	static uint32_t startTime = 0;
	static int8_t count = 0;
	
	printf("Byte: %u\n", c);
	
	/* 50ms timeout */
	/*if (waitingMessage && TimebasePassedTimeMS(startTime) > 50) {
		waitingMessage = 0;
	}*/
	
	if (waitingMessage) {
		/* save start time */
		startTime = TimebaseCurrentTime();
		/* UART END */
		if (count >= 15) {
			PORTC ^= (1<<PC0);
			//if (c == UART_END_BYTE) {
				CanSend(&cm);
			//}
			waitingMessage = 0;
			return;
		}
		/* data */
		else if (count >= 7) {
			cm.Data.bytes[count-7] = c;
			count++;
			return;
		}
		/* data length */
		else if (count >= 6) {
			cm.DataLength = c;
			count++;
			return;
		}
		/* remote request flag */
		else if (count >= 5) {
			cm.RemoteFlag = c;
			count++;
			return;
		}
		/* extended */
		else if (count >= 4) {
			cm.ExtendedFlag = c;
			count++;
			return;
		}
		/* ident */
		else if (count >= 0) {
			cm.Id += ((uint32_t)c << (count*8));
			count++;
			return;
		}
	}
	
	if (c == UART_START_BYTE && !waitingMessage) {
		waitingMessage = 1;
		startTime = TimebaseCurrentTime();
		count = 0;
		cm.Id = 0;
		return;	
	}
}


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	TimebaseInit();
	UartInit();
	CanInit(CAN_BITRATE_1M);
	
	DDRC = 1<<PC1 | 1<<PC0;
	PORTC = (1<<PC1) | (1<<PC0);
	
	sei();
	
	CanMessage_t rxMsg;
	uint16_t rxByte;
	uint8_t i = 0;
	
	/* main loop */
	while (1) {
		/* any new CAN messages received? */
		if (CanReceive(&rxMsg) == CAN_OK) {
			PORTC ^= (1<<PC1);
			/* send message to CanWatcher */
			uart_putc(UART_START_BYTE);
			uart_putc((uint8_t)rxMsg.Id);
			uart_putc((uint8_t)(rxMsg.Id>>8));
			uart_putc((uint8_t)(rxMsg.Id>>16));
			uart_putc((uint8_t)(rxMsg.Id>>24));
			uart_putc(rxMsg.ExtendedFlag);
			uart_putc(rxMsg.RemoteFlag);
			uart_putc(rxMsg.DataLength);
			for (i=0; i<8; i++) {
				uart_putc(rxMsg.Data.bytes[i]);
			}
			uart_putc(UART_END_BYTE);
		}
		
		/* any UART bytes received? */
		rxByte = uart_getc();
		while (rxByte != UART_NO_DATA) {
			/* parse byte! */
			UartParseByte((uint8_t)(rxByte & 0x00FF));
			/* receive next */
			rxByte = uart_getc();
		}
	}
	
	return 0;
}
