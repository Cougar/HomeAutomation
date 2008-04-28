/*********************************************************************
 *
 *               Data SPI EEPROM Access Routines
 *
 *********************************************************************
 * FileName:        SPIEEPROM.c
 * Dependencies:    Compiler.h
 *                  XEEPROM.h
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/20/02     Original (Rev. 1.0)
 * Howard Schlunder		9/01/04		Rewritten for SPI EEPROMs
 * Howard Schlunder		8/10/06		Modified to control SPI module 
 *									frequency whenever EEPROM accessed 
 *									to allow bus sharing with different 
 *									frequencies.
********************************************************************/
#include "..\Include\Compiler.h"
#include "..\Include\XEEPROM.h"
#include "..\Include\StackTsk.h"

#if defined(MPFS_USE_EEPROM)

#if defined(PICDEMNET)
	#error "PICDEMNET is defined, but the PICDEM.net demo board does not have an SPI EEPROM on it.  Is I2CEEPROM.c intended instead?"
#endif


// IMPORTANT SPI NOTE: The code in this file expects that the SPI interrupt 
//		flag (EEPROM_SPI_IF) be clear at all times.  If the SPI is shared with 
//		other hardware, the other code should clear the EEPROM_SPI_IF when it is 
//		done using the SPI.


// EEPROM SPI opcodes
#define READ	0b00000011	// Read data from memory array beginning at selected address
#define WRITE	0b00000010	// Write data to memory array beginning at selected address
#define WRDI	0b00000100	// Reset the write enable latch (disable write operations)
#define WREN	0b00000110	// Set the write enable latch (enable write operations)
#define RDSR	0b00000101	// Read Status register
#define WRSR	0b00000001	// Write Status register

void DoWrite(void);

WORD EEPROMAddress;
BYTE EEPROMBuffer[EEPROM_BUFFER_SIZE];
BYTE *EEPROMBufferPtr;

/*********************************************************************
 * Function:        void XEEInit(unsigned char speed)
 *
 * PreCondition:    None
 *
 * Input:           speed - not used (included for compatibility only)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  EEPROM.
 *
 * Note:            Code sets SPI clock to Fosc/16.  
 ********************************************************************/
#if defined(HPC_EXPLORER) && !defined(__18F87J10)
	#define PROPER_SPICON1	(0x20)		/* SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level */
#elif defined(__PIC24F__)
    #define PROPER_SPICON1 	(0x0013 | 0x0120)	/* 1:1 primary prescale, 4:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC30F__)
    #define PROPER_SPICON1 	(0x0017 | 0x0120)	/* 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
    #define PROPER_SPICON1	(0x0003 | 0x0120)	/* 1:1 primary prescale, 8:1 secondary prescale, CKE=1, MASTER mode */
#else
	//#define PROPER_SPICON1	(0x21)		/* SSPEN bit is set, SPI in master mode, FOSC/16, IDLE state is low level */
	#define PROPER_SPICON1	(0x22)		/* SSPEN bit is set, SPI in master mode, FOSC/16, IDLE state is low level */
#endif

void XEEInit(unsigned char speed)
{
	EEPROM_CS_IO = 1;
	EEPROM_CS_TRIS = 0;		// Drive SPI EEPROM chip select pin

	EEPROM_SCK_TRIS = 0;	// Set SCK pin as an output
	EEPROM_SDI_TRIS = 1;	// Make sure SDI pin is an input
	EEPROM_SDO_TRIS = 0;	// Set SDO pin as an output

	EEPROM_SPICON1 = PROPER_SPICON1; // See PROPER_SPICON1 definition above
	#if defined(__C30__)
	    EEPROM_SPICON2 = 0;
	    EEPROM_SPISTAT = 0;    // clear SPI
	    EEPROM_SPISTATbits.SPIEN = 1;
	#elif defined(__18CXX)
		EEPROM_SPI_IF = 0;
		EEPROM_SPISTATbits.CKE = 1; 	// Transmit data on rising edge of clock
		EEPROM_SPISTATbits.SMP = 0;		// Input sampled at middle of data output time
	#endif
}


/*********************************************************************
 * Function:        XEE_RESULT XEEBeginRead(unsigned char control,
 *                                          XEE_ADDR address)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control - EEPROM control and address code.
 *                  address - Address at which read is to be performed.
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Sets internal address counter to given address.
 *                  Puts EEPROM in sequential read mode.
 *
 * Note:            This function does not release I2C bus.
 *                  User must call XEEEndRead() when read is not longer
 *                  needed; I2C bus will released after XEEEndRead()
 *                  is called.
 ********************************************************************/
XEE_RESULT XEEBeginRead(unsigned char control, XEE_ADDR address )
{
	// Save the address and emptry the contents of our local buffer
	EEPROMAddress = address;
	EEPROMBufferPtr = EEPROMBuffer + EEPROM_BUFFER_SIZE;
	return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEERead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Reads next byte from EEPROM; internal address
 *                  is incremented by one.
 *
 * Note:            This function does not release I2C bus.
 *                  User must call XEEEndRead() when read is not longer
 *                  needed; I2C bus will released after XEEEndRead()
 *                  is called.
 ********************************************************************/
unsigned char XEERead(void)
{
	// Check if no more bytes are left in our local buffer
	if( EEPROMBufferPtr == EEPROMBuffer + EEPROM_BUFFER_SIZE )
	{ 
		// Get a new set of bytes
		XEEReadArray(0, EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
		EEPROMAddress += EEPROM_BUFFER_SIZE;
		EEPROMBufferPtr = EEPROMBuffer;
	}

	// Return a byte from our local buffer
	return *EEPROMBufferPtr++;
}

/*********************************************************************
 * Function:        XEE_RESULT XEEEndRead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Ends sequential read cycle.
 *
 * Note:            This function ends sequential cycle that was in
 *                  progress.  It releases I2C bus.
 ********************************************************************/
XEE_RESULT XEEEndRead(void)
{
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEReadArray(unsigned char control,
 *                                          XEE_ADDR address,
 *                                          unsigned char *buffer,
 *                                          unsigned char length)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - Unused
 *                  address     - Address from where array is to be read
 *                  buffer      - Caller supplied buffer to hold the data
 *                  length      - Number of bytes to read.
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Reads desired number of bytes in sequential mode.
 *                  This function performs all necessary steps
 *                  and releases the bus when finished.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEReadArray(unsigned char control,
                        XEE_ADDR address,
                        unsigned char *buffer,
                        unsigned char length)
{
	BYTE Dummy;
	#if defined(__18CXX)
	BYTE SPICON1Save;
	#else
	WORD SPICON1Save;
	#endif	

	// Save SPI state (clock speed)
	SPICON1Save = EEPROM_SPICON1;
	EEPROM_SPICON1 = PROPER_SPICON1;

	EEPROM_CS_IO = 0;

	// Send READ opcode
	EEPROM_SSPBUF = READ;
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	
	// Send address
	EEPROM_SSPBUF = ((WORD_VAL*)&address)->v[1];
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	EEPROM_SSPBUF = ((WORD_VAL*)&address)->v[0];
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	
	while(length--)
	{
		EEPROM_SSPBUF = 0;
		while(!EEPROM_SPI_IF);
		*buffer++ = EEPROM_SSPBUF;
		EEPROM_SPI_IF = 0;
	};
	
	EEPROM_CS_IO = 1;

	// Restore SPI state
	EEPROM_SPICON1 = SPICON1Save;

	return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEESetAddr(unsigned char control,
 *                                        XEE_ADDR address)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - data EEPROM control code
 *                  address     - address to be set for writing
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            Unlike XEESetAddr() in xeeprom.c for I2C EEPROM 
 *					 memories, this function is used only for writing
 *					 to the EEPROM.  Reads must use XEEBeginRead(), 
 *					 XEERead(), and XEEEndRead().
 *					This function does not release the SPI bus.
 *                  User must close XEEClose() after this function
 *                   is called.
 ********************************************************************/
XEE_RESULT XEESetAddr(unsigned char control, XEE_ADDR address)
{
	EEPROMAddress = address;
	EEPROMBufferPtr = EEPROMBuffer;
	return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEWrite(unsigned char val)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           val - Byte to be written
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Adds a byte to the current page to be writen when
 *					XEEEndWrite() is called.
 *
 * Note:            Page boundary cannot be exceeded or the byte 
 *					to be written will be looped back to the 
 *					beginning of the page.
 ********************************************************************/
XEE_RESULT XEEWrite(unsigned char val)
{
	*EEPROMBufferPtr++ = val;
	if( EEPROMBufferPtr == EEPROMBuffer + EEPROM_BUFFER_SIZE )
    {
		DoWrite();
    }

    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEEndWrite(void)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Instructs EEPROM to begin write cycle.
 *
 * Note:            Call this function after either page full of bytes
 *                  written or no more bytes are left to load.
 *                  This function initiates the write cycle.
 *                  User must call for XEEIsBusy() to ensure that write
 *                  cycle is finished before calling any other
 *                  routine.
 ********************************************************************/
XEE_RESULT XEEEndWrite(void)
{
	if( EEPROMBufferPtr != EEPROMBuffer )
    {
		DoWrite();
    }

    return XEE_SUCCESS;
}

void DoWrite(void)
{
	BYTE Dummy;
	BYTE BytesToWrite;
	#if defined(__18CXX)
	BYTE SPICON1Save;
	#else
	WORD SPICON1Save;
	#endif	

	// Save SPI state (clock speed)
	SPICON1Save = EEPROM_SPICON1;
	EEPROM_SPICON1 = PROPER_SPICON1;
	
	// Set the Write Enable latch
	EEPROM_CS_IO = 0;
	EEPROM_SSPBUF = WREN;
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	EEPROM_CS_IO = 1;
	
	// Send WRITE opcode
	EEPROM_CS_IO = 0;
	EEPROM_SSPBUF = WRITE;
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	
	// Send address
	EEPROM_SSPBUF = ((WORD_VAL*)&EEPROMAddress)->v[1];
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	EEPROM_SSPBUF = ((WORD_VAL*)&EEPROMAddress)->v[0];
	while(!EEPROM_SPI_IF);
	Dummy = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	
	BytesToWrite = (BYTE)(EEPROMBufferPtr - EEPROMBuffer);
	
	EEPROMAddress += BytesToWrite;
	EEPROMBufferPtr = EEPROMBuffer;

	while(BytesToWrite--)
	{
		// Send the byte to write
		EEPROM_SSPBUF = *EEPROMBufferPtr++;
		while(!EEPROM_SPI_IF);
		Dummy = EEPROM_SSPBUF;
		EEPROM_SPI_IF = 0;
	}

	// Begin the write
	EEPROM_CS_IO = 1;

	EEPROMBufferPtr = EEPROMBuffer;

	// Restore SPI State
	EEPROM_SPICON1 = SPICON1Save;

	// Wait for write to complete
	while( XEEIsBusy(0) );
}


/*********************************************************************
 * Function:        XEE_RESULT XEEIsBusy(unsigned char control)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - EEPROM control and address code.
 *
 * Output:          XEE_READY if EEPROM is not busy
 *                  XEE_BUSY if EEPROM is busy
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Requests ack from EEPROM.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEIsBusy(unsigned char control)
{
	BYTE_VAL result;
	#if defined(__18CXX)
	BYTE SPICON1Save;
	#else
	WORD SPICON1Save;
	#endif	

	// Save SPI state (clock speed)
	SPICON1Save = EEPROM_SPICON1;
	EEPROM_SPICON1 = PROPER_SPICON1;

	EEPROM_CS_IO = 0;
	// Send RDSR - Read Status Register opcode
	EEPROM_SSPBUF = RDSR;
	while(!EEPROM_SPI_IF);
	result.Val = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	
	// Get register contents
	EEPROM_SSPBUF = 0;
	while(!EEPROM_SPI_IF);
	result.Val = EEPROM_SSPBUF;
	EEPROM_SPI_IF = 0;
	EEPROM_CS_IO = 1;

	// Restore SPI State
	EEPROM_SPICON1 = SPICON1Save;

	return result.bits.b0 ? XEE_BUSY : XEE_SUCCESS;
}


#endif //#if defined(MPFS_USE_EEPROM)
