/*********************************************************************
 *
 *       Example Web Server Application using Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        MainDemo.c
 * Dependencies:    string.H
 *                  StackTsk.h
 *                  Tick.h
 *                  http.h
 *                  MPFS.h
 *				   	mac.h
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
 * Author              Date         Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti		4/19/01		Original (Rev. 1.0)
 * Nilesh Rajbharti		2/09/02		Cleanup
 * Nilesh Rajbharti		5/22/02		Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti		7/9/02		Rev 2.1 (See version.log for detail)
 * Nilesh Rajbharti		4/7/03		Rev 2.11.01 (See version log for detail)
 * Howard Schlunder		10/1/04		Beta Rev 0.9 (See version log for detail)
 * Howard Schlunder		10/8/04		Beta Rev 0.9.1 Announce support added
 * Howard Schlunder		11/29/04	Beta Rev 0.9.2 (See version log for detail)
 * Howard Schlunder		2/10/05		Rev 2.5.0
 * Howard Schlunder		1/5/06		Rev 3.00
 * Howard Schlunder		1/18/06		Rev 3.01 ENC28J60 fixes to TCP, 
 *									UDP and ENC28J60 files
 * Howard Schlunder		3/01/06		Rev. 3.16 including 16-bit micro support
 * Howard Schlunder		4/12/06		Rev. 3.50 added LCD for Explorer 16
 * Howard Schlunder		6/19/06		Rev. 3.60 finished dsPIC30F support, added PICDEM.net 2 support
 * Howard Schlunder		8/02/06		Rev. 3.75 added beta DNS, NBNS, and HTTP client (GenericTCPClient.c) services
 ********************************************************************/

/*
 * Following define uniquely deines this file as main
 * entry/application In whole project, there should only be one such
 * definition and application file must define AppConfig variable as
 * described below.
 */
#define THIS_IS_STACK_APPLICATION

#define VERSION 		"v3.75"		// TCP/IP stack version

// These headers must be included for required defs.
#include <string.h>
#include "..\Include\Compiler.h"
#include "..\Include\StackTsk.h"
#include "..\Include\Tick.h"
#include "..\Include\MAC.h"
#include "..\Include\Helpers.h"
#include "..\Include\Delay.h"

#include "..\canBoot\Include\boot.h"
#include <stackTasks.h>
#include <funcdefs.h>
#include <CAN.h>

#if defined(STACK_USE_HTTP_SERVER)
#include "..\Include\HTTP.h"
#endif

#if defined(STACK_USE_SGP_SERVER)
#include "..\Include\SGP.h"
#endif

#if defined(STACK_USE_DNS)
#include "..\Include\DNS.h"
#endif

#if defined(STACK_USE_TIMESYNC)
#include "..\Include\TimeSync.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "..\Include\XEEPROM.h"
#endif


// This is used by other stack elements.
// Main application must define this and initialize it with proper values.
APP_CONFIG AppConfig;

// Private helper functions.
// These may or may not be present in all applications.
static void InitAppConfig(void);
static void InitializeBoard(void);
static void ProcessIO(void);

#define SaveAppConfig()

#pragma interrupt high_isr
void high_isr(void)
{
	canISR();
}


#pragma interrupt low_isr
void low_isr(void)
{

}

void main(void)
{
    static TICK t = 0;
    
    // Initialize any application specific hardware.
    InitializeBoard();

	canInit();

    // Initialize Stack and application related NV variables.
    InitAppConfig();

    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif

#if defined(STACK_USE_SGP_SERVER)
	SGPInit();
#endif


    // Once all items are initialized, go into infinite loop and let
    // stack items execute their tasks.
    // If application needs to perform its own task, it should be
    // done at the end of while loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must broken
    // down into smaller pieces so that other tasks can have CPU time.
    while(1)
    {
        // Blink SYSTEM LED every second.
        if ( tickGet()-t >= TICK_1S/2 )
        {
            t = tickGet();
            LED0_IO ^= 1;
        }

        // This task performs normal stack task including checking
        // for incoming packet, type of packet and calling
        // appropriate stack entity to process it.
        StackTask();

#if defined(STACK_USE_HTTP_SERVER)
        // This is a TCP application.  It listens to TCP port 80
        // with one or more sockets and responds to remote requests.
        HTTPServer();
#endif

#if defined(STACK_USE_SGP_SERVER)
        // This is a TCP application.  It listens to TCP port 6666
        // with one or more sockets and responds to remote requests.
        SGPServer();
#endif

#if defined(STACK_USE_TIMESYNC)
	timeSync();
#endif

        // In future, as new TCP/IP applications are written, it
        // will be added here as new tasks.

        // Add your application speicifc tasks here.
        ProcessIO();

    }
}

static void ProcessIO(void)
{
}


/*********************************************************************
 * Function:        void HTTPExecCmd(BYTE** argv, BYTE argc)
 *
 * PreCondition:    None
 *
 * Input:           argv        - List of arguments
 *                  argc        - Argument count.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a "callback" from HTTPServer
 *                  task.  Whenever a remote node performs
 *                  interactive task on page that was served,
 *                  HTTPServer calls this functions with action
 *                  arguments info.
 *                  Main application should interpret this argument
 *                  and act accordingly.
 *
 *                  Following is the format of argv:
 *                  If HTTP action was : thank.htm?name=Joe&age=25
 *                      argv[0] => thank.htm
 *                      argv[1] => name
 *                      argv[2] => Joe
 *                      argv[3] => age
 *                      argv[4] => 25
 *
 *                  Use argv[0] as a command identifier and rests
 *                  of the items as command arguments.
 *
 * Note:            THIS IS AN EXAMPLE CALLBACK.
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)

void HTTPExecCmd(BYTE** argv, BYTE argc)
{
	setVariable(argv[1][0],(argv[2][0]=='1'?1:0));
}

void setVariable(BYTE var, DWORD val)
{
	if (var==VAR_LED1) LED1_IO=val;
}

DWORD getVariable(BYTE var)
{
	if (var==VAR_LED1) return LED1_IO;
}

void getVariableName(BYTE var,char *str)
{
	BYTE i;
	if (var==VAR_LED1) 
	{
		for(i=0;i<sizeof(VAR_LED1_HUMAN);i++)
			str[i]=VAR_LED1_HUMAN[i];
	}
}

#endif



/*
*	Function: canParse
*
*	Input:	Received can message
*	Output: none
*	Pre-conditions: canInit and received packet.
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/
void canParse(CAN_PACKET cp)
{

}





/*********************************************************************
 * Function:        void InitializeBoard(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize board specific hardware.
 *
 * Note:            None
 ********************************************************************/
static void InitializeBoard(void)
{
	// LEDs
	LED0_TRIS = 0;
	LED1_TRIS =0;
	LED0_IO = 0;
	LED1_IO = 0;


    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;

	// Enable Interrupts
    T0CON = 0;
	RCONbits.IPEN=1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
	

}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void InitAppConfig(void)
{

 AppConfig.Flags.bIsDHCPEnabled = FALSE;

	// IP
//	AppConfig.MyIPAddr.v[0]=193;
//	AppConfig.MyIPAddr.v[1]=11;
//	AppConfig.MyIPAddr.v[2]=249;
//	AppConfig.MyIPAddr.v[3]=228;

	AppConfig.MyIPAddr.v[0]=192;
	AppConfig.MyIPAddr.v[1]=168;
	AppConfig.MyIPAddr.v[2]=0;
	AppConfig.MyIPAddr.v[3]=228;
	
	//MAC
	AppConfig.MyMACAddr.v[0]=0x00;
	AppConfig.MyMACAddr.v[1]=0x10;
	AppConfig.MyMACAddr.v[2]=0xA7;
	AppConfig.MyMACAddr.v[3]=0x02;
	AppConfig.MyMACAddr.v[4]=0xF2;
	AppConfig.MyMACAddr.v[5]=0xBF;

	// Mask
	AppConfig.MyMask.v[0]=255;
	AppConfig.MyMask.v[1]=255;
	AppConfig.MyMask.v[2]=255;
	AppConfig.MyMask.v[3]=0;

	// Gateway
//	AppConfig.MyGateway.v[0]=193;
//	AppConfig.MyGateway.v[1]=11;
//	AppConfig.MyGateway.v[2]=249;
//	AppConfig.MyGateway.v[3]=1;

	// Gateway
	AppConfig.MyGateway.v[0]=192;
	AppConfig.MyGateway.v[1]=168;
	AppConfig.MyGateway.v[2]=0;
	AppConfig.MyGateway.v[3]=1;

	// Dns
	AppConfig.PrimaryDNSServer.v[0]=193;
	AppConfig.PrimaryDNSServer.v[1]=11;
	AppConfig.PrimaryDNSServer.v[2]=255;
	AppConfig.PrimaryDNSServer.v[3]=230;
}




// Below are mandantory when using bootloader
// define DEBUG_MODE to use without bootloader.

#ifndef DEBUG_MODE
extern void _startup (void); 
#pragma code _RESET_INTERRUPT_VECTOR = RM_RESET_VECTOR
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code
#endif

#pragma code _HIGH_INTERRUPT_VECTOR = RM_HIGH_INTERRUPT_VECTOR
void _high_ISR (void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code _LOW_INTERRUPT_VECTOR = RM_LOW_INTERRUPT_VECTOR
void _low_ISR (void)
{
    _asm GOTO low_isr _endasm
}
#pragma code

