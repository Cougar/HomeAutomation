/**
 * CAN test program.
 * 
 * @target	AVR
 * @date	2006-10-29
 * @author	Jimmy Myhrman, Andreas Fritiofsson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/

/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>

#include <config.h>
/* lib files */
#ifdef USE_STDCAN
#include <stdcan.h>
#else
#include <mcp2515.h>
#endif
#include <serial.h>
#include <uart.h>
#include <timebase.h>

#ifdef USE_STDCAN
#if SENDTIMESTAMP == 1
#error SENDTIMESTAMP not implemented with StdCan yet.
#endif
#endif


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define UART_START_BYTE 253
#define UART_END_BYTE 250

#ifndef USE_STDCAN
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag
#endif

const uint8_t days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

union {
	uint32_t packed;
	struct date_t {
		unsigned ss:6;
		unsigned mm:6;
		unsigned hh:5;
		unsigned DD:5;
		unsigned MM:4;
		unsigned YY:6;
	} unpacked;
} date;

#if SENDTIMESTAMP == 1
void IncTime(void) {
	static uint8_t YY=07, MM=03, DD=02, hh=23, mm=58, ss=00;
	
	if (ss++ > 59) {
		ss=0;
		if (mm++ > 59) {
			mm=0;
			if (hh++ > 23) {
				hh=0;
				if (DD++ > days[MM-1] + ((YY%4) & (MM==2)) ? 1 : 0) {
					DD=1;
					if (MM++ > 12) {
						MM=1; YY++;
					}
				}
			}
		}
	}
	date.unpacked.YY = YY;
	date.unpacked.MM = MM;
	date.unpacked.DD = DD;
	date.unpacked.hh = hh;
	date.unpacked.mm = mm;
	date.unpacked.ss = ss;
}
#endif
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
#ifndef USE_STDCAN
	static Can_Message_t cm;
#else
	static StdCan_Msg_t cm;
#endif
	static uint8_t waitingMessage = 0;
	static uint32_t startTime = 0;
	static int8_t count = 0;
	
	/* 50ms timeout */
	if (waitingMessage && Timebase_PassedTimeMillis(startTime) > 50) {
		waitingMessage = 0;
	}
	
	if (waitingMessage) {
		/* save start time */
		startTime = Timebase_CurrentTime();
		/* UART END */
		if (count >= 15) {
			if (c == UART_END_BYTE) {
				PORTC ^= (1<<PC0);
#ifndef USE_STDCAN
				Can_Send(&cm);
#else
				StdCan_Put(&cm);
#endif
			}
			waitingMessage = 0;
			return;
		}
		/* data */
		else if (count >= 7) {
#ifndef USE_STDCAN
			cm.Data.bytes[count-7] = c;
#else
			cm.Data[count-7] = c;
#endif
			count++;
			return;
		}
		/* data length */
		else if (count >= 6) {
#ifndef USE_STDCAN
			cm.DataLength = c;
#else
			cm.Length = c;
#endif
			count++;
			return;
		}
		/* remote request flag */
		else if (count >= 5) {
#ifndef USE_STDCAN
			cm.RemoteFlag = c;
#endif
			count++;
			return;
		}
		/* extended */
		else if (count >= 4) {
#ifndef USE_STDCAN
			cm.ExtendedFlag = c;
#endif
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
		startTime = Timebase_CurrentTime();
		count = 0;
		cm.Id = 0;
		return;	
	}
}

#ifndef USE_STDCAN
void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.

	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}
#endif


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
//	For calibrating internal oscillator
//	OSCCAL = eeprom_read_byte(0);
	Timebase_Init();
	Serial_Init();
#ifndef USE_STDCAN
	Can_Init();
#else
	StdCan_Init(0);
#endif	
	
	DDRC = 1<<PC1 | 1<<PC0;
	PORTC = (1<<PC1) | (1<<PC0);
	
	sei();
	
#if SENDTIMESTAMP == 1
	Can_Message_t timeMsg;
	uint32_t time = Timebase_CurrentTime();
	uint32_t time1 = time;
	uint32_t unixtime = 0;
#endif
	uint16_t rxByte;
	uint8_t i = 0;
	
#if SENDTIMESTAMP == 1
	timeMsg.ExtendedFlag = 1;
	timeMsg.RemoteFlag = 0;
	timeMsg.Id = (CAN_NMT << CAN_SHIFT_CLASS) | (CAN_NMT_TIME << CAN_SHIFT_NMT_TYPE);
	//timeMsg.Id = 0; //Same thing, and lib's can.h is not updated.
	timeMsg.DataLength = 8;
#endif
	
#ifdef USE_STDCAN
	StdCan_Msg_t rxMsg;
#endif	
	
	/* main loop */
	while (1) {
		/* any new CAN messages received? */
#ifndef USE_STDCAN
		if (rxMsgFull) {
#else
		if (StdCan_Get(&rxMsg) == StdCan_Ret_OK) {
#endif
			// Toggle activity LED
			PORTC ^= (1<<PC1);
			/* send message to CanWatcher */
			uart_putc(UART_START_BYTE);
			uart_putc((uint8_t)rxMsg.Id);
			uart_putc((uint8_t)(rxMsg.Id>>8));
			uart_putc((uint8_t)(rxMsg.Id>>16));
			uart_putc((uint8_t)(rxMsg.Id>>24));
#ifndef USE_STDCAN
			uart_putc(rxMsg.ExtendedFlag);
			uart_putc(rxMsg.RemoteFlag);
			uart_putc(rxMsg.DataLength);
			for (i=0; i<8; i++) {
				uart_putc(rxMsg.Data.bytes[i]);
			}
#else
			uart_putc(1);
			uart_putc(0);
			uart_putc(rxMsg.Length);
			for (i=0; i<8; i++) {
				uart_putc(rxMsg.Data[i]);
			}
#endif
			uart_putc(UART_END_BYTE);
			
#ifndef USE_STDCAN
			rxMsgFull = 0;
#endif
		}
		
		/* any UART bytes received? */
		rxByte = uart_getc();
		while (rxByte != UART_NO_DATA) {
			/* parse byte! */
			UartParseByte((uint8_t)(rxByte & 0x00FF));
			/* receive next */
			rxByte = uart_getc();
		}
		
#if SENDTIMESTAMP == 1
		time1 = Timebase_CurrentTime();
		if ((time1 - time) > 1000) {
			time = time1;
			timeMsg.Data.dwords[0] = ++unixtime;
			IncTime();
			timeMsg.Data.dwords[1] = date.packed;
			Can_Send(&timeMsg);
		}
#endif
	}
	
	return 0;
}
