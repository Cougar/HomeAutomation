/*********************************************************************
*
*       LCD Access Routines
*
*********************************************************************
* FileName:        LCDBlocking.c
* Dependencies:    Compiler.h
* Processor:       PIC24F
* Complier:        MCC30 v2.01 or higher
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
* Author               Date         Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Howard Schlunder     4/03/06		Original
* Howard Schlunder     4/12/06		Changed from using PMP to LCDWrite()
* Howard Schlunder	   8/10/06		Fixed a delay being too short 
*									when CLOCK_FREQ was a smaller 
*									value, added FOUR_BIT_MODE
********************************************************************/

#include "..\Include\StackTsk.h"
#include "..\Include\Compiler.h"
#include "..\Include\Delay.h"

// Do not include this source file if there is no LCD on the 
// target board
#ifdef USE_LCD

#if defined(PICDEMNET)
#define FOUR_BIT_MODE
#endif

// LCDText is a 32 byte shadow of the LCD text.  Write to it and 
// then call LCDUpdate() to copy the string into the LCD module.
BYTE LCDText[16*2+1];

/******************************************************************************
 * Function:        static void LCDWrite(BYTE RS, BYTE Data)
 *
 * PreCondition:    None
 *
 * Input:           RS - Register Select - 1:RAM, 0:Config registers
 *					Data - 8 bits of data to write
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Controls the Port I/O pins to cause an LCD write
 *
 * Note:            None
 *****************************************************************************/
static void LCDWrite(BYTE RS, BYTE Data)
{
	LCD_DATA_TRIS = 0x00;
	LCD_RS_TRIS = 0;
	LCD_RD_WR_TRIS = 0;
	LCD_RD_WR_IO = 0;
	LCD_RS_IO = RS;

#if defined(FOUR_BIT_MODE)
	LCD_DATA_IO = Data>>4;
	Nop();					// Wait Data setup time (min 40ns)
	Nop();
	LCD_E_IO = 1;
	Nop();					// Wait E Pulse width time (min 230ns)
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	LCD_E_IO = 0;
#endif

	LCD_DATA_IO = Data;
	Nop();					// Wait Data setup time (min 40ns)
	Nop();
	LCD_E_IO = 1;
	Nop();					// Wait E Pulse width time (min 230ns)
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	LCD_E_IO = 0;

	LCD_DATA_TRIS = 0xFF;
	LCD_RS_TRIS = 1;
	LCD_RD_WR_TRIS = 1;
}


/******************************************************************************
 * Function:        void LCDInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        LCDText[] is blanked, port I/O pin TRIS registers are 
 *					configured, and the LCD is placed in the default state
 *
 * Note:            None
 *****************************************************************************/
void LCDInit(void)
{
	BYTE i;

	for(i = 0; i < sizeof(LCDText)-1; i++)
	{
		LCDText[i] = ' ';
	}
	LCDText[sizeof(LCDText)-1] = 0;

	// Setup the I/O pins
	LCD_E_IO = 0;
	LCD_RD_WR_IO = 0;

	LCD_DATA_TRIS = 0x00;
	LCD_RD_WR_TRIS = 0;
	LCD_RS_TRIS = 0;
	LCD_E_TRIS = 0;


	// Wait the required time for the LCD to reset
	DelayMs(30);

	// Set the default function
#if defined(FOUR_BIT_MODE)
	LCDWrite(0, 0b00101000);
#else
	LCDWrite(0, 0b00111000);
#endif
	Delay10us(5);

	// Set the display control
	LCDWrite(0, 0b00001100);
	Delay10us(5);

	// Clear the display
	LCDWrite(0, 0b00000001);
	Delay10us(153);
	
	// Set the entry mode
	LCDWrite(0, 0b00000110);
	Delay10us(5);
}


/******************************************************************************
 * Function:        void LCDUpdate(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           LCDText[]
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies the contents of the local LCDText[] array into the 
 *					LCD's internal display buffer.  Null terminators in 
 *					LCDText[] terminate the current line, so strings may be 
 *					printed directly to LCDText[].
 *
 * Note:            None
 *****************************************************************************/
void LCDUpdate(void)
{
	BYTE i, j;

	// Go home
	LCDWrite(0, 0b00000010);
	DelayMs(1);
	Delay10us(53);

	// Output first line
	for(i = 0; i < 16; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0)
		{
			for(j=i; j < 16; j++)
			{
				LCDText[j] = ' ';
			}
		}
		LCDWrite(1, LCDText[i]);
		Delay10us(5);
	}
	
	// Set the address to the second line
	LCDWrite(0, 0xC0);
	Delay10us(5);

	// Output second line
	for(i = 16; i < 32; i++)
	{
		// Erase the rest of the line if a null char is 
		// encountered (good for printing strings directly)
		if(LCDText[i] == 0)
		{
			for(j=i; j < 32; j++)
			{
				LCDText[j] = ' ';
			}
		}
		LCDWrite(1, LCDText[i]);
		Delay10us(5);
	}
}

/******************************************************************************
 * Function:        void LCDErase(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clears LCDText[] and the LCD's internal display buffer
 *
 * Note:            None
 *****************************************************************************/
void LCDErase(void)
{
	// Clear display
	LCDWrite(0, 0b00000001);
	Delay10us(153);

	// Clear local copy
	memset(LCDText, ' ', 32);
}

#endif	//#ifdef USE_LCD
