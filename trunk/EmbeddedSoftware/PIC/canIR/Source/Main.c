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


#include <p18cxxx.h>
#include <compiler.h>
#include <stackTasks.h>
#include <Tick.h>
#include <funcdefs.h>
#include <CAN.h>
#include <EEAccess.h>
#include <crc16.h>
#include "..\canBoot\Include\boot.h"


typedef enum _EE_STATE {eesWAITING_START,eesSEND_ACK,eesWAIT_FIRST_DATA,eesWAIT_DATA_OR_CRC,eesWRITE_DATA,eesDONE} EE_STATE;

#ifdef USE_IREC
	#include <IRec.h>
#endif

static void mainInit(void);
static void loadIrCommands(void);

#define EE_PACKET_TIMOUT 5*TICK_100MS
#define IR_TIMOUT 2*TICK_100MS

static BOOL hasCtrlPacket = FALSE;
static CAN_PACKET ctrlPacket;

typedef struct _IR_COMMAND
{
	BYTE repeat;
	BYTE addr;
	BYTE data;
	BYTE pgm_class;
	WORD pgm_id;
	BYTE pgm_data[2];
} IR_COMMAND;

#define IR_COUNT 32
static BYTE irValids = 0;
static BYTE irCounts = 0;
static TICK tick_irTimout = 0;
static BYTE pgmData[56];

#pragma udata big_mem
static IR_COMMAND irCommands[IR_COUNT];
#pragma udata

void main()
{
	static EE_STATE ees = eesWAITING_START;
	static EE_STATE eesAckNext = eesWAITING_START;
	static BYTE ee_programmerId = 0;
	static BOOL ee_externalEE = FALSE;
	static DWORD ee_pgmAddress = 0;
	static DWORD ee_newPgmAddress = 0;
	static PGM_CTRL_TYPE ee_ackType = pctEE_ACK;
	
	static BYTE ee_lastLocalIndex = 0;
	CAN_PACKET outCp;

	// Inits
	mainInit();

	#ifdef USE_IREC
		loadIrCommands();
	#endif


	canInit();

	#ifdef USE_IREC
		irecInit();
	#endif

	for(ee_programmerId=0;ee_programmerId<56;ee_programmerId++) pgmData[ee_programmerId]=0xFF;

	while(1)
	{	
		TICK currentTick = tickGet();

		static TICK tick_led = 0;
		static TICK tick_ee = 0;
		


		if ((currentTick-tick_led)>(1*TICK_1S))
		{
			//LED0_IO=~LED0_IO;
			tick_led = currentTick;
		}

		if ((currentTick-tick_irTimout)>IR_TIMOUT)
		{
			irCounts = 0;
			tick_irTimout = currentTick;
		}

		switch(ees)
		{
			case eesWAITING_START:

				if (hasCtrlPacket == TRUE && ctrlPacket.pgm.id == pctEE_START && ctrlPacket.data[4] == myId())
				{
					ee_programmerId = ctrlPacket.sid;
					ee_externalEE = (ctrlPacket.data[3]==1?TRUE:FALSE);
					ee_pgmAddress = (((DWORD)ctrlPacket.data[2])<<16)+(((DWORD)ctrlPacket.data[1])<<8)+((DWORD)ctrlPacket.data[0]);
					tick_ee = currentTick;
					ee_ackType = pctEE_ACK;
					eesAckNext = eesWAIT_FIRST_DATA;
					ees = eesSEND_ACK;
				}
				break;

			case eesSEND_ACK:
				// send ack
				outCp.type = ptPGM;
				outCp.pgm.class = pcCTRL;
				outCp.pgm.id = ee_ackType;
				outCp.length=0;
				tick_ee = currentTick;
				ees	= eesAckNext;
				while(!canSendMessage(outCp,PRIO_HIGH));
				break;

			case eesWAIT_FIRST_DATA:
				// Wait for first pgm packet. use offset to determine data index.
				// When receiving crc from host, check crc for offsets bytes.
				
				if ((currentTick-tick_ee)>EE_PACKET_TIMOUT)
				{
					// Timeout, resend ack/nack.
					eesAckNext = eesWAIT_FIRST_DATA;
					ees = eesSEND_ACK;
				}				

				if (hasCtrlPacket == TRUE && ctrlPacket.pgm.id==pctEE_PGM)
				{
					BYTE localIndex = ctrlPacket.data[7];
					ee_lastLocalIndex = localIndex;
					pgmData[localIndex+0]=ctrlPacket.data[0]; pgmData[localIndex+1]=ctrlPacket.data[1]; pgmData[localIndex+2]=ctrlPacket.data[2]; pgmData[localIndex+3]=ctrlPacket.data[3];
					pgmData[localIndex+4]=ctrlPacket.data[4]; pgmData[localIndex+5]=ctrlPacket.data[5]; pgmData[localIndex+6]=ctrlPacket.data[6];
					ees = eesWAIT_DATA_OR_CRC;
				}

				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_START)
				{
					ees   = eesWAITING_START;
				}

				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_CRC)
				{
					ees   = eesWAIT_DATA_OR_CRC;
				}

				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_DONE)
				{
					ees = eesDONE;
				}


				break;

			case eesWAIT_DATA_OR_CRC:
				// Wait for pgm packet. use offset to determine data index.
				// When receiving crc from host, check crc for offsets bytes.
				
				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_CRC)
				{
					WORD crc16 = (((WORD)ctrlPacket.data[4])<<8)+((WORD)ctrlPacket.data[3]);
					WORD crc16calc = calc_crc(pgmData,56);

					// check CRC.
					if (crc16calc==crc16)
					{
						ee_newPgmAddress = (((DWORD)ctrlPacket.data[2])<<16)+(((DWORD)ctrlPacket.data[1])<<8)+((DWORD)ctrlPacket.data[0]);
						eesAckNext = eesWAIT_FIRST_DATA;
						ees = eesWRITE_DATA;
					}
					else
					{
						tick_ee = currentTick;
						ee_ackType = pctEE_NACK;
						eesAckNext = eesWAIT_FIRST_DATA;
						ees = eesSEND_ACK;
					}
				}
				
				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_PGM)
				{
					BYTE localIndex = ctrlPacket.data[7];
					ee_lastLocalIndex = localIndex;
					pgmData[localIndex+0]=ctrlPacket.data[0]; pgmData[localIndex+1]=ctrlPacket.data[1]; pgmData[localIndex+2]=ctrlPacket.data[2]; pgmData[localIndex+3]=ctrlPacket.data[3];
					pgmData[localIndex+4]=ctrlPacket.data[4]; pgmData[localIndex+5]=ctrlPacket.data[5]; pgmData[localIndex+6]=ctrlPacket.data[6];
				}

				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_START)
				{
					ees   = eesWAITING_START;
				}

				if (hasCtrlPacket==TRUE && ctrlPacket.pgm.id==pctEE_DONE)
				{
					ees = eesDONE;
				}

				break;


			case eesWRITE_DATA:

				ClrWdt();

				// Write EE and send ack.

				if (ee_externalEE == FALSE)
				{
					BYTE i;

					ee_pgmAddress = EE_IR_START+1+(WORD)((EE_INTERNAL_SIZE-1) & ee_pgmAddress);

					for(i=0;i<56;i++)
					{
						EEWrite(ee_pgmAddress+i,pgmData[i]);

						pgmData[i]=0xFF;
					}	


				}

				ee_pgmAddress	= ee_newPgmAddress;
				ee_ackType 		= pctEE_ACK;		
				ees 			= eesSEND_ACK;

			break;

			case eesDONE:
				EEWrite(EE_IR_START,32);
				loadIrCommands();

				ees = eesWAITING_START;
			break;

		
			default:
				ees = eesWAITING_START;
			break;

		}

			hasCtrlPacket = FALSE;

	}
}


#pragma interrupt high_isr
void high_isr(void)
{
	canISR();

	#ifdef USE_IREC
		irecISR();
	#endif

}


#pragma interrupt low_isr
void low_isr(void)
{

}

void loadIrCommands()
{

	// Load IRs
	BYTE i;
	irValids = EERead(EE_IR_START);
	for(i=0;(i<irValids && i<IR_COUNT);i++)
	{
		IR_COMMAND * buf_ptr = &irCommands[0];

		
		buf_ptr[i].repeat = EERead(EE_IR_START+1+(i<<3));
		buf_ptr[i].addr = EERead(EE_IR_START+2+(i<<3));
		buf_ptr[i].data = EERead(EE_IR_START+3+(i<<3));
		buf_ptr[i].pgm_class = EERead(EE_IR_START+4+(i<<3));
		buf_ptr[i].pgm_id = ((WORD)EERead(EE_IR_START+5+(i<<3)) << 8)+(WORD)EERead(EE_IR_START+6+(i<<3));
		buf_ptr[i].pgm_data[1] = EERead(EE_IR_START+7+(i<<3));
		buf_ptr[i].pgm_data[0] = EERead(EE_IR_START+8+(i<<3));

		/*
		buf_ptr[0].repeat = 5;
		buf_ptr[0].addr = 5;
		buf_ptr[0].data = 5;
		buf_ptr[0].pgm_class = 5;
		buf_ptr[0].pgm_id = 5;
		buf_ptr[0].pgm_data[1] = 5;
		buf_ptr[0].pgm_data[0] = 5;

		buf_ptr[1].repeat = 7;
		buf_ptr[1].addr = 7;
		buf_ptr[1].data = 7;
		buf_ptr[1].pgm_class = 7;
		buf_ptr[1].pgm_id = 7;
		buf_ptr[1].pgm_data[1] = 7;
		buf_ptr[1].pgm_data[0] = 7;

		buf_ptr[2].repeat = 9;
		buf_ptr[2].addr = 9;
		buf_ptr[2].data = 9;
		buf_ptr[2].pgm_class = 9;
		buf_ptr[2].pgm_id = 9;
		buf_ptr[2].pgm_data[1] = 9;
		buf_ptr[2].pgm_data[0] = 9;
		*/
	}
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

void canParse(CAN_PACKET cp)
{
	BYTE i;


	if (cp.pgm.class == pcCTRL)
	{
		hasCtrlPacket = TRUE;
		ctrlPacket.sid = cp.sid;
		for(i=0;i<8;i++) ctrlPacket.data[i]=cp.data[i];
		ctrlPacket.length = cp.length;
		ctrlPacket.pgm.id = cp.pgm.id;
	}

}



/*
*	Function: irecParse
*
*	Input:	Received IR message
*	Output: none
*	Pre-conditions: irecInit
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/
#ifdef USE_IREC
void irecParse(IR_TYPE type, BYTE toggle, BYTE addr, BYTE data)
{
	static BYTE lastToogle = 0;
	
	CAN_PACKET outCp;
	BYTE i;
	IR_COMMAND * buf_ptr = &irCommands[0];

	tick_irTimout = tickGet();

	irCounts++;

	outCp.type 		= ptPGM;
	outCp.pgm.class = pcSENSOR;
	outCp.pgm.id 	= pstIR;
	outCp.length 	= 4;
	outCp.data[3]	= type;
	outCp.data[2]	= toggle;
	outCp.data[1]	= addr;
	outCp.data[0]	= data;
	while(!canSendMessage(outCp,PRIO_HIGH));

	// DUMP
	if (addr==0x15 && data>=0x00)
	{
		for(i=0;i<7;i++)
		{
			outCp.type 		= ptPGM;
			outCp.pgm.class = 0x05;
			outCp.pgm.id 	= 0x91;
			outCp.length 	= 8;
			outCp.data[7] = buf_ptr[i].repeat;
			outCp.data[6]=buf_ptr[i].addr;
			outCp.data[5]=buf_ptr[i].data;
			outCp.data[4]=buf_ptr[i].pgm_class;
			outCp.data[3]=(BYTE)((buf_ptr[i].pgm_id & 0xFF00) >> 8);
			outCp.data[2]=(BYTE)buf_ptr[i].pgm_id;
			outCp.data[1]=buf_ptr[i].pgm_data[1];
			outCp.data[0]=buf_ptr[i].pgm_data[0];
			while(!canSendMessage(outCp,PRIO_HIGH));
		}		
	}

	/*
	if (addr==0x15 && data>=0x09)
	{
		for(i=0;i<7;i++)
		{
			outCp.type 		= ptPGM;
			outCp.pgm.class = 0x05;
			outCp.pgm.id 	= 0x91;
			outCp.length 	= 8;
			outCp.data[7] = pgmData[i*8];
			outCp.data[6]=pgmData[i*8+1];
			outCp.data[5]=pgmData[i*8+2];
			outCp.data[4]=pgmData[i*8+3];
			outCp.data[3]=pgmData[i*8+4];
			outCp.data[2]=pgmData[i*8+5];
			outCp.data[1]=pgmData[i*8+6];
			outCp.data[0]=pgmData[i*8+7];
			while(!canSendMessage(outCp,PRIO_HIGH));
		}
	}
	*/

	// slow? bad?
	for(i=0;(i<irValids && i<IR_COUNT);i++)
	{
		if (buf_ptr[i].addr==addr && buf_ptr[i].data==data && ((buf_ptr[i].repeat == 0 && (lastToogle != toggle || irCounts==1)) || (buf_ptr[i].repeat == irCounts)))
		{
			outCp.type 		= ptPGM;
			outCp.pgm.class = buf_ptr[i].pgm_class;
			outCp.pgm.id 	= buf_ptr[i].pgm_id;
			outCp.length 	= 2;
			outCp.data[1]	= buf_ptr[i].pgm_data[1];
			outCp.data[0]	= buf_ptr[i].pgm_data[0];
			while(!canSendMessage(outCp,PRIO_HIGH));

			if (buf_ptr[i].repeat>0) irCounts = 0;
		}
	}


	lastToogle	= toggle;

}
#endif

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
