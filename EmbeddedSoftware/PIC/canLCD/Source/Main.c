/*********************************************************************
 *
 *                  Main application
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/


#include <compiler.h>
#include <stackTasks.h>
#include <Tick.h>
#include <funcdefs.h>
#include <EEaccess.h>

#ifdef USE_LCD
	#include "lcd.h"
#endif

#ifdef USE_CAN
	#include <CAN.h>
#endif

static void mainInit(void);


static int MY_ID = DEFAULT_ID;
static BYTE MY_NID = DEFAULT_NID;

void main()
{
	static TICK t = 0;
	CAN_MESSAGE outCm;

	// Inits

	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif

	#ifdef USE_LCD
		lcdInit();
		lcdClear();
		lcdPutrs("Welcome!");
	#endif

	tickInit();

	// Read ID and NID if exist.
	if (EERead(NODE_ID_EE)==NODE_HAS_ID)
	{
		MY_ID=(((WORD)EERead(NODE_ID_EE + 1))<<8)+EERead(NODE_ID_EE + 2);
		MY_NID=EERead(NODE_ID_EE + 3);
	}

	// Send user program startup heatbeat
	outCm.funct 					= FUNCT_BOOTLOADER;
	outCm.funcc 					= FUNCC_BOOT_HEARTBEAT;
	outCm.nid   					= MY_NID;
	outCm.sid   					= MY_ID;
	outCm.data_length 				= 1;
	outCm.data[BOOT_DATA_HEARTBEAT_INDEX] = HEARTBEAT_USER_STARTUP;
	while(!canSendMessage(outCm,PRIO_HIGH));


	while(1)
	{
		static TICK t = 0;
		static TICK heartbeat = 0;

		
		if ((tickGet()-heartbeat)>TICK_SECOND*5)
		{
			// Send alive heartbeat
			outCm.funct 					= FUNCT_BOOTLOADER;
			outCm.funcc 					= FUNCC_BOOT_HEARTBEAT;
			outCm.nid   					= MY_NID;
			outCm.sid   					= MY_ID;
			outCm.data_length 				= 1;
			outCm.data[BOOT_DATA_HEARTBEAT_INDEX] = HEARTBEAT_ALIVE;
			while(!canSendMessage(outCm,PRIO_HIGH));
			
			heartbeat = tickGet();
		}
		if ((tickGet()-t)>TICK_SECOND)
		{
			LED0_IO=~LED0_IO;
			t = tickGet();
		}
	}
}


#pragma interrupt high_isr
void high_isr(void)
{
	ClrWdt();

	#ifdef USE_CAN
		canISR();
	#endif

	tickUpdate();

}


#pragma interrupt low_isr
void low_isr(void)
{

}



/*
*	Function: mainInit
*
*	Input:	none
*	Output: none
*	Pre-conditions: none
*	Affects: Se code
*	Depends: none.
*/
void mainInit()
{
	LED0_TRIS=0;	

	// Enable Interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

	// Enable interrupt prirority
	RCONbits.IPEN=1;
	

}

/*
*	Function: canParse
*
*	Input:	Received can message
*	Output: none
*	Pre-conditions: canInit and received packet.
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/
#ifdef USE_CAN
void canParse(CAN_MESSAGE cm)
{
	CAN_MESSAGE outCm;

	ClrWdt();

	switch(cm.funct)
	{
		case FUNCT_BOOTLOADER:
			if (cm.nid==MY_NID && ((((unsigned int)cm.data[BOOT_DATA_RID_HIGH_INDEX])<<8)+cm.data[BOOT_DATA_RID_LOW_INDEX])==MY_ID) 
			{
				switch(cm.funcc)
				{
					case FUNCC_BOOT_INIT: Reset(); break; //User whant to program me, reset me.
					case FUNCC_BOOT_SET_ID:
						// Change my ID and NID
						MY_ID=(((WORD)cm.data[BOOT_DATA_NEW_ID_HIGH_INDEX])<<8)+cm.data[BOOT_DATA_NEW_ID_LOW_INDEX];
						MY_NID=cm.data[BOOT_DATA_NEW_NID_INDEX];
						EEWrite(NODE_ID_EE+1,cm.data[BOOT_DATA_NEW_ID_HIGH_INDEX]);
						EEWrite(NODE_ID_EE+2,cm.data[BOOT_DATA_NEW_ID_LOW_INDEX]);
						EEWrite(NODE_ID_EE+3,cm.data[BOOT_DATA_NEW_NID_INDEX]);
						EEWrite(NODE_ID_EE,NODE_HAS_ID);
						// Send ack with new id.
						outCm.funct 					= FUNCT_BOOTLOADER;
						outCm.funcc 					= FUNCC_BOOT_ACK;
						outCm.nid   					= MY_NID;
						outCm.sid   					= MY_ID;
						outCm.data_length 				= 8;
						outCm.data[BOOT_DATA_ERR_INDEX]	= ACK_ERR_NO_ERROR;
						while(!canSendMessage(outCm,PRIO_HIGH));
					break;
				}
			}


		break;
	}
}
#endif



// Below are mandantory when using bootloader
// define DEBUG_MODE to use without bootloader.

#ifndef DEBUG_MODE
extern void _startup (void); 
#pragma code _RESET_INTERRUPT_VECTOR = 0x001000
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code
#endif

#pragma code _HIGH_INTERRUPT_VECTOR = 0x001008
void _high_ISR (void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code _LOW_INTERRUPT_VECTOR = 0x001018
void _low_ISR (void)
{
    _asm GOTO low_isr _endasm
}
#pragma code
