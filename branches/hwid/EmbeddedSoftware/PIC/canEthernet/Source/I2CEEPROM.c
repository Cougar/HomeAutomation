/*********************************************************************
 *
 *               Data I2C EEPROM Access Routines
 *
 *********************************************************************
 * FileName:        I2CEEPROM.c
 * Dependencies:    Compiler.h
 *                  XEEPROM.h
 * Processor:       PIC18
 * Complier:        Microchip C18 v3.03 or higher
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
 * Howard Schlunder		8/10/06		Renamed registers/bits to use 
 *									C18/C30 style standard names
********************************************************************/
#include "..\Include\Compiler.h"
#include "..\Include\XEEPROM.h"

#if defined(MPFS_USE_EEPROM)

#if !defined(PICDEMNET)
	#error "PICDEMNET is not defined, but I2CEEPROM.c is included.  Was SPIEEPROM.c supposed to be included instead?"
#endif

#define IdleI2C()       while( (EEPROM_SPICON2 & 0x1F) | (EEPROM_SPISTATbits.R_W) );
#define StartI2C()      EEPROM_SPICON2bits.SEN=1
#define RestartI2C()    EEPROM_SPICON2bits.RSEN=1
#define StopI2C()       EEPROM_SPICON2bits.PEN=1
#define NotAckI2C()     EEPROM_SPICON2bits.ACKDT=1, EEPROM_SPICON2bits.ACKEN=1

static unsigned char getcI2C( void );
static unsigned char WriteI2C( unsigned char data_out );
XEE_RESULT XEEClose(void);



/*********************************************************************
 * Function:        void XEEInit(unsigned char baud)
 *
 * PreCondition:    None
 *
 * Input:           baud    - SSPADD value for bit rate.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize I2C module to communicate to serial
 *                  EEPROM.
 *
 * Note:            None
 ********************************************************************/
#define   MASTER    8     /* I2C Master mode                    */
/* SSPSTAT REGISTER */
#define   SLEW_OFF  0xC0  /* Slew rate disabled for 100kHz mode */
#define   SLEW_ON   0x00  /* Slew rate enabled for 400kHz mode  */
#define   SSPENB    0x20  /* Enable serial port and configures
                             SCK, SDO, SDI                      */
void XEEInit(unsigned char baud)
{
  SSPSTAT &= 0x3F;                // power on state
  SSPCON1 = 0x00;                 // power on state
  EEPROM_SPICON2 = 0x00;                 // power on state
  SSPCON1 |= MASTER;            // select serial mode
  SSPSTAT |= SLEW_ON;                // slew rate on/off

  EEPROM_SCL_TRIS = 1;           // Set SCL (PORTC,3) pin to input
  EEPROM_SDA_TRIS = 1;           // Set SDA (PORTC,4) pin to input

  SSPCON1 |= SSPENB;              // enable synchronous serial port

  SSPADD = baud;
}


/*********************************************************************
 * Function:        XEE_RESULT XEESetAddr(unsigned char control,
 *                                        XEE_ADDR address)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           control     - data EEPROM control code
 *                  address     - address to be set
 *
 * Output:          XEE_SUCCESS if successful
 *                  other value if failed.
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            This function does not release the I2C bus.
 *                  User must close XEEClose() after this function
 *                  is called.
 ********************************************************************/
XEE_RESULT XEESetAddr(unsigned char control, XEE_ADDR address)
{
    union
    {
        unsigned short int Val;
        struct
        {
            unsigned char LSB;
            unsigned char MSB;
        } bytes;
    } tempAddress;

    tempAddress.Val = address;

    IdleI2C();                      // ensure module is idle
    StartI2C();                     // initiate START condition
    while ( EEPROM_SPICON2bits.SEN );      // wait until start condition is over
    if ( PIR2bits.BCLIF )           // test for bus collision
        return XEE_BUS_COLLISION;                // return with Bus Collision error

    if ( WriteI2C( control ) )    // write 1 byte
        return XEE_BUS_COLLISION;              // set error for write collision

    IdleI2C();                    // ensure module is idle
    if ( !EEPROM_SPICON2bits.ACKSTAT )   // test for ACK condition, if received
    {
        if ( WriteI2C( tempAddress.bytes.MSB ) )  // WRITE word address to EEPROM
            return XEE_BUS_COLLISION;            // return with write collision error

        IdleI2C();                  // ensure module is idle

        if ( WriteI2C( tempAddress.bytes.LSB ) )  // WRITE word address to EEPROM
            return XEE_BUS_COLLISION;            // return with write collision error

        IdleI2C();                  // ensure module is idle
        if ( !EEPROM_SPICON2bits.ACKSTAT ) // test for ACK condition received
            return XEE_SUCCESS;
        else
            return XEE_NAK;            // return with Not Ack error
    }
    else
        return XEE_NAK;              // return with Not Ack error
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
    unsigned char r;

    r = XEESetAddr(control, address);
    if ( r != XEE_SUCCESS )
        return r;

    r = XEEClose();
    if ( r != XEE_SUCCESS )
        return r;


    IdleI2C();
    StartI2C();
    while( EEPROM_SPICON2bits.SEN );
    if ( PIR2bits.BCLIF )
        return XEE_BUS_COLLISION;

    if ( WriteI2C(control+1) )
        return XEE_BUS_COLLISION;

    IdleI2C();
    if ( !EEPROM_SPICON2bits.ACKSTAT )
        return XEE_SUCCESS;
    return XEE_NAK;
}


XEE_RESULT XEEWrite(unsigned char val)
{
    IdleI2C();                  // ensure module is idle

    if ( WriteI2C( val ) )   // data byte for EEPROM
        return XEE_BUS_COLLISION;          // set error for write collision

    IdleI2C();
    if ( !EEPROM_SPICON2bits.ACKSTAT )
        return XEE_SUCCESS;
    return XEE_NAK;
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
 *                  User must call for XEEWait() to ensure that write
 *                  cycle is finished before calling any other
 *                  routine.
 ********************************************************************/
XEE_RESULT XEEEndWrite(void)
{
    IdleI2C();
    StopI2C();
    while(EEPROM_SPICON2bits.PEN);
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
    getcI2C();
    while( EEPROM_SPICON2bits.RCEN );  // check that receive sequence is over.

    EEPROM_SPICON2bits.ACKDT = 0;      // Set ack bit
    EEPROM_SPICON2bits.ACKEN = 1;
    while( EEPROM_SPICON2bits.ACKEN );

    return EEPROM_SSPBUF;
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
 * Note:            This function ends seuential cycle that was in
 *                  progress.  It releases I2C bus.
 ********************************************************************/
XEE_RESULT XEEEndRead(void)
{
    getcI2C();
    while( EEPROM_SPICON2bits.RCEN );  // check that receive sequence is over.

    NotAckI2C();
    while( EEPROM_SPICON2bits.ACKEN );

    StopI2C();
    while( EEPROM_SPICON2bits.PEN );

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
 * Input:           control     - EEPROM control and address code.
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
    XEE_RESULT r;

    r = XEEBeginRead(control, address);
    if ( r != XEE_SUCCESS )
        return r;

    while( length-- )
        *buffer++ = XEERead();

    r = XEEEndRead();

    return r;
}




XEE_RESULT XEEClose(void)
{
    IdleI2C();
    StopI2C();
    while( EEPROM_SPICON2bits.PEN );           // wait until stop condition is over.
    if ( PIR2bits.BCLIF )
        return XEE_BUS_COLLISION;
    return XEE_SUCCESS;
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
    XEE_RESULT r;

    IdleI2C();                      // ensure module is idle
    StartI2C();                     // initiate START condition
    while ( EEPROM_SPICON2bits.SEN );      // wait until start condition is over
    if ( PIR2bits.BCLIF )           // test for bus collision
    {
        return XEE_BUS_COLLISION;                // return with Bus Collision error
    }

    else
    {
        if ( WriteI2C( control ) )    // write byte - R/W bit should be 0
            return XEE_BUS_COLLISION;              // set error for write collision

        IdleI2C();                    // ensure module is idle
        if ( PIR2bits.BCLIF )         // test for bus collision
            return XEE_BUS_COLLISION;              // return with Bus Collision error

        if ( !EEPROM_SPICON2bits.ACKSTAT )
            r = XEE_READY;
        else
            r = XEE_BUSY;

#if 0
        while ( EEPROM_SPICON2bits.ACKSTAT ) // test for ACK condition received
        {
            RestartI2C();               // initiate Restart condition
            while ( EEPROM_SPICON2bits.RSEN ); // wait until re-start condition is over
            if ( PIR2bits.BCLIF )       // test for bus collision
                return XEE_BUS_COLLISION;            // return with Bus Collision error

            if ( WriteI2C( control ) )  // write byte - R/W bit should be 0
                return XEE_BUS_COLLISION;            // set error for write collision

            IdleI2C();                  // ensure module is idle
        }
#endif
    }


    StopI2C();                      // send STOP condition
    while ( EEPROM_SPICON2bits.PEN );      // wait until stop condition is over
    if ( PIR2bits.BCLIF )           // test for bus collision
        return XEE_BUS_COLLISION;                // return with Bus Collision error

    return r;
    //return XEE_READY;                   // return with no error
}


/********************************************************************
*     Function Name:    WriteI2C                                    *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C bus.               *
*     Description:      This routine writes a single byte to the    *
*                       I2C bus.                                    *
********************************************************************/
static unsigned char WriteI2C( unsigned char data_out )
{
  EEPROM_SSPBUF = data_out;           // write single byte to EEPROM_SSPBUF
  if ( SSPCON1bits.WCOL )      // test if write collision occurred
   return ( -1 );              // if WCOL bit is set return negative #
  else
  {
    while( SSPSTATbits.BF );   // wait until write cycle is complete
    return ( 0 );              // if WCOL bit is not set return non-negative #
  }
}



/********************************************************************
*     Function Name:    ReadI2C                                     *
*     Return Value:     contents of EEPROM_SSPBUF register                 *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C bus.              *
********************************************************************/
static unsigned char getcI2C( void )
{
	EEPROM_SPICON2bits.RCEN = 1;		// enable master for 1 byte reception
	while(!SSPSTATbits.BF);		// wait until byte received
	return EEPROM_SSPBUF;				// return with read byte
}


#endif //#if defined(MPFS_USE_EEPROM)
