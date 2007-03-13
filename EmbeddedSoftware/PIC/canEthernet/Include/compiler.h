/*********************************************************************
 *
 *                  Compiler and hardware specific definitions
 *
 *********************************************************************
 * FileName:        Compiler.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     11/14/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Howard Schlunder		11/30/04 Added some more defines
 * Howard Schlunder		6/14/06	Added hardware definitions
 * Howard Schlunder		8/10/06	Added PICDEMNET, PIC18F97J60_TEST_BOARD, FS_USB hardware definitions
 ********************************************************************/
#ifndef COMPILER_H
#define COMPILER_H

#define APP_VERSION 0x0002

#include <p18cxxx.h>
#define CLOCK_FREQ		(40000000)      // Hz
#define INSTR_FREQ			(CLOCK_FREQ/4)

	// I/O pins
	#define LED0_TRIS		(TRISCbits.TRISC0)
	#define LED0_IO			(PORTCbits.RC0)
	#define LED1_TRIS		(TRISCbits.TRISC1)
	#define LED1_IO			(PORTCbits.RC1)

	
	#define VAR_LED1_HUMAN "LED1"
	#define VAR_LED1_STR "l"
	#define VAR_LED1 VAR_LED1_STR[0]

	#define BUTTON0_TRIS	(TRISBbits.TRISB1)
	#define	BUTTON0_IO		(PORTBbits.RB1)
	
	// ENC28J60 I/O pins
	#define ENC_RST_TRIS	(TRISBbits.TRISB5)	// Not connected by default
	#define ENC_RST_IO		(LATBbits.LATB5)
	#define ENC_CS_TRIS		(TRISBbits.TRISB3)
	#define ENC_CS_IO		(LATBbits.LATB3)
	#define ENC_SCK_TRIS	(TRISCbits.TRISC3)
	#define ENC_SDI_TRIS	(TRISCbits.TRISC4)
	#define ENC_SDO_TRIS	(TRISCbits.TRISC5)
	#define ENC_SPI_IF		(PIR1bits.SSPIF)
	#define ENC_SSPBUF		(SSPBUF)
	#define ENC_SPISTAT		(SSPSTAT)
	#define ENC_SPISTATbits	(SSPSTATbits)
	#define ENC_SPICON1		(SSPCON1)
	#define ENC_SPICON1bits	(SSPCON1bits)
	#define ENC_SPICON2		(SSPCON2)

	// 25LC256 I/O pins
	#define EEPROM_CS_TRIS		(TRISBbits.TRISB4)
	#define EEPROM_CS_IO		(LATBbits.LATB4)
	#define EEPROM_SCK_TRIS		(TRISCbits.TRISC3)
	#define EEPROM_SDI_TRIS		(TRISCbits.TRISC4)
	#define EEPROM_SDO_TRIS		(TRISCbits.TRISC5)
	#define EEPROM_SPI_IF		(PIR1bits.SSPIF)
	#define EEPROM_SSPBUF		(SSPBUF)
	#define EEPROM_SPICON1		(SSPCON1)
	#define EEPROM_SPICON1bits	(SSPCON1bits)
	#define EEPROM_SPICON2		(SSPCON2)
	#define EEPROM_SPISTAT		(SSPSTAT)
	#define EEPROM_SPISTATbits	(SSPSTATbits)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ROM                 	rom
#define strcpypgm2ram(a, b)		strcpypgm2ram(a,(far rom char*)b)

	#define	__attribute__(a)
	#define BusyUART()				BusyUSART()
	#define CloseUART()				CloseUSART()
	#define ConfigIntUART(a)		ConfigIntUSART(a)
	#define DataRdyUART()			DataRdyUSART()
	#define OpenUART(a,b,c)			OpenUSART(a,b,c)
	#define ReadUART()				ReadUSART()
	#define WriteUART(a)			WriteUSART(a)
	#define getsUART(a,b,c)			getsUSART(b,a)
	#define putsUART(a)				putsUSART(a)
	#define getcUART()				ReadUSART()
	#define putcUART(a)				WriteUSART(a)
	#define putrsUART(a)			putrsUSART((far rom char*)a)

#endif
