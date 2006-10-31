/*********************************************************************
 *
 *                  Main application
 *
 *********************************************************************
 * FileName:        Main.c
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#include <compiler.h>
#include <stackTasks.h>
#include <CANdefs.h>
#include <CAN.h>
//#include <Tick.h>
#include <boot.h>

/** V E C T O R  R E M A P P I N G *******************************************/

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto RM_HIGH_INTERRUPT_VECTOR _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto RM_LOW_INTERRUPT_VECTOR _endasm
}

#pragma code


#ifdef USE_CAN
	#include <CAN.h>
#endif

static CAN_PROTO_MESSAGE cm;
static BYTE ful=0;
static DWORD cnt =0;

static void mainInit(void);
static BOOL canGetPacket(void);
void TBLWT_Func(void);
void TBLWTPOSTINC_Func (void);
void TBLRD_Func(void);

#define TICK_SUB_COUNTER_10MS 312
#define PACKET_TIMEOUT 14
#define BOOTLOADER_INIT_TIMOUT 500

DWORD tickCounter = 0;
DWORD tickSubCounter = 0;

DWORD tickGet(void);

void main()
{
	static PROGRAM_STATE pgs = pgsWATING_START;
	static DWORD t = 0;
	static WORD programmerSid=0;
	
	static BYTE errMsg = ERR_NO_ERROR;
	static PROGRAM_STATE afterAckPgs = pgsWATING_START;
	static DWORD pgmAddress = 0;
	static BYTE bytesReceived = 0;

	DWORD tBoot;
	
	static BYTE pgmData[64];

	static BYTE tmp;

	// Inits
	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif

	tBoot = tickGet();
	

	for(;;)
	{
		BYTE i;
		CAN_PROTO_MESSAGE outCm;
		BOOL hasPacket = canGetPacket();

		tickSubCounter++;
		if (tickSubCounter>TICK_SUB_COUNTER_10MS)
		{
			tickCounter++;
			tickSubCounter=0;
		}

		switch(pgs)
		{
			case pgsWATING_START:
				// Check if there is bootloader start packet
				if (hasPacket==TRUE && cm.funct==FUNCT_BOOTLOADER && cm.funcc==FUNCC_BOOT_INIT && cm.data[RID_INDEX]==MY_ID && cm.nid==MY_NID)
				{
					// Has new start packet
					pgmAddress 		= (((DWORD)cm.data[ADDRU_INDEX])<<16)+(((DWORD)cm.data[ADDRH_INDEX])<<8)+((DWORD)cm.data[ADDRL_INDEX]);
					programmerSid 	= cm.sid;
					t 				= tickGet();
					errMsg			= ERR_NO_ERROR;
					afterAckPgs		= pgsWAIT_FIRST_PGM_PACKET;
					pgs 			= pgsSEND_ACK;
					// save pgm adress.
					
				}

				// Bootloader timeout
				if ((tickGet()-tBoot)>BOOTLOADER_INIT_TIMOUT)
				{
					pgs = pgsDONE;
				}
			break;			

			case pgsSEND_ACK:
				// send ack
				outCm.funct 			= FUNCT_BOOTLOADER;
				outCm.funcc 			= FUNCC_BOOT_ACK;
				outCm.nid   			= MY_NID;
				outCm.sid   			= MY_ID;
				outCm.data_length 		= 8;
				outCm.remote_request 	= FALSE;
				outCm.data[ERR_INDEX]	= errMsg;
				t						= tickGet();
				pgs						= afterAckPgs;

				while(!canSendMessage(outCm,PRIO_HIGH));
			break;


			case pgsWAIT_FIRST_PGM_PACKET:
				// wait for first pgm packet.
				if ((tickGet()-t)>PACKET_TIMEOUT)
				{
					// Timeout, resend ack.
					errMsg			= ERR_NO_ERROR;
					afterAckPgs		= pgsWAIT_FIRST_PGM_PACKET;
					pgs 			= pgsSEND_ACK;
				}

				// If programming packet...
				if (hasPacket==TRUE && cm.funct==FUNCT_BOOTLOADER && (cm.funcc & 0x3)==FUNCC_BOOT_PGM && cm.nid==MY_NID && cm.sid==programmerSid)
				{
					BYTE localIndex = (BYTE)((cm.funcc & 0x1C)>>2)*8;

					// Save bytes in memory, setup start adress etc..
					bytesReceived 	= 8;		
					pgmData[localIndex+0]=cm.data[0]; pgmData[localIndex+1]=cm.data[1]; pgmData[localIndex+2]=cm.data[2]; pgmData[localIndex+3]=cm.data[3];
					pgmData[localIndex+4]=cm.data[4]; pgmData[localIndex+5]=cm.data[5]; pgmData[localIndex+6]=cm.data[6]; pgmData[localIndex+7]=cm.data[7];
				
					pgs 			= pgsWAIT_PGM_PACKET;
				}


			break;


			case pgsWAIT_PGM_PACKET:
		
			
				// If programming packet...
				if (hasPacket==TRUE && cm.funct==FUNCT_BOOTLOADER && (cm.funcc & 0x3)==FUNCC_BOOT_PGM && cm.nid==MY_NID && cm.sid==programmerSid)
				{
					BYTE localIndex = (BYTE)((cm.funcc & 0x1C)>>2)*8;
					// Save bytes in memory, inc received bytes.
					pgmData[localIndex+0]=cm.data[0]; pgmData[localIndex+1]=cm.data[1]; pgmData[localIndex+2]=cm.data[2]; pgmData[localIndex+3]=cm.data[3];
					pgmData[localIndex+4]=cm.data[4]; pgmData[localIndex+5]=cm.data[5]; pgmData[localIndex+6]=cm.data[6]; pgmData[localIndex+7]=cm.data[7];
					
					bytesReceived 	+= 8;
					pgs 			 = pgsWAIT_PGM_PACKET;
				}

				// If adress packet (control packet)..
				if (hasPacket==TRUE && cm.funct==FUNCT_BOOTLOADER && cm.funcc==FUNCC_BOOT_ADDR && cm.nid==MY_NID && cm.sid==programmerSid)
				{
					// Send ack and goto wait first packet.
					// save adress.
					errMsg			= ERR_NO_ERROR;
					afterAckPgs		= pgsWAIT_FIRST_PGM_PACKET;
					pgs 			= pgsSEND_ACK;
				}
				
				// If end packet...
				if (hasPacket==TRUE && cm.funct==FUNCT_BOOTLOADER && cm.funcc==FUNCC_BOOT_DONE && cm.nid==MY_NID && cm.sid==programmerSid)
				{
					// write program.
					// If no bytes received at all, assume end.
					// If not 64 bytes received, send error ack and wait for first packet.
					// If 64, write.

					if (bytesReceived==0)
					{
						// No received, done.
						errMsg			= ERR_NO_ERROR;
						afterAckPgs		= pgsDONE;
						pgs 			= pgsSEND_ACK;
					}
					else if (bytesReceived!=64)
					{
						errMsg			= ERR_ERROR;
						afterAckPgs		= pgsWAIT_FIRST_PGM_PACKET;
						pgs 			= pgsSEND_ACK;
					}
					else
					{
						pgs 			 = pgsWRITE_PROGRAM;
						afterAckPgs		= pgsDONE;
					}
				}

				// If bytes received 64, write..
				if (bytesReceived==64)
				{
					LED0_IO=~LED0_IO;
					pgs 			 = pgsWRITE_PROGRAM;
					afterAckPgs		= pgsWAIT_PGM_PACKET;
					// TODO! Send ack more than once..
				}


			break;


			case pgsWRITE_PROGRAM:


				// Write program and send ack.

				//Load Table pointer registers with base address to erase.
				TBLPTRU = (BYTE)((pgmAddress & 0xFF0000)>>16);
				TBLPTRH = (BYTE)((pgmAddress & 0xFF00)>>8);
				TBLPTRL = (BYTE)(pgmAddress & 0xFF);

				// point to Flash program memory
				EECON1bits.EEPGD = 1;
				// access Flash program memory
				EECON1bits.CFGS = 0;
				// enable write to memory
				EECON1bits.WREN = 1;
				// enable Row Erase operation
				EECON1bits.FREE = 1;
				// disable interrupts
				INTCONbits.GIE = 0;

				// Do it!
				EECON2 = 0x55;
				EECON2 = 0xAA;
				EECON1bits.WR = 1;

				// enable interrupts
				INTCONbits.GIE = 1;
				
				//Load Table pointer registers with base address to program.
				TBLPTRU = (BYTE)((pgmAddress & 0xFF0000)>>16);
				TBLPTRH = (BYTE)((pgmAddress & 0xFF00)>>8);
				TBLPTRL = (BYTE)(pgmAddress & 0xFF);

				// Load bytes
				for(i=0;i<63;i++)
				{
					TABLAT=pgmData[i];
					TBLWTPOSTINC_Func();
				}
				TABLAT=pgmData[63];	
				TBLWT_Func();


				// point to Flash program memory
				EECON1bits.EEPGD = 1;
				// access Flash program memory
				EECON1bits.CFGS = 0;
				// enable write to memory
				EECON1bits.WREN = 1;
				// disable interrupts
				INTCONbits.GIE = 0;
				
				// Do it!
				EECON2 = 0x55;
				EECON2 = 0xAA;
				EECON1bits.WR = 1;

				// enable interrupts
				INTCONbits.GIE = 1;

				// disable write to memory
				EECON1bits.WREN = 0;

				pgmAddress	   += 64;
				bytesReceived 	= 0;
				errMsg			= ERR_NO_ERROR;
				pgs 			= pgsSEND_ACK;

			break;


			case pgsDONE:
				_asm  goto RM_RESET_VECTOR _endasm
			break;

		


			default:
				pgs = pgsWATING_START;
			break;
		}
	

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

    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;
	
	// Enable Interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

	// Enable interrupt prirority
	RCONbits.IPEN=1;
	

}



/// CAN-------------------------------------------------------


#ifdef USE_CAN


/*
*	Function: canInit
*
*	Input:	none
*	Output: none
*	Pre-conditions: none
*	Affects: Changes can registers.
*	Depends: none.
*/
void canInit()
{
	CANCON = 0x80; //request config mode
	while ((CANSTAT & 0xE0) != 0x80 ); //wait for config mode request

	// BAUD AND TQS

	BRGCON1=0;
	
	// Sync_Seq = 2 x TQ = 1
	BRGCON1bits.SJW1=0;
	BRGCON1bits.SJW0=1;
	
	// Setting BRP.
	BRGCON1=BRGCON1|CAN_BRP;
	
	// Maximum PHEG1
	BRGCON2bits.SEG2PHTS = 1;
	
	// Phase_Seg1 = 2 x TQ = 1
	BRGCON2bits.SEG1PH2 = 0;
	BRGCON2bits.SEG1PH1 = 0;
	BRGCON2bits.SEG1PH0 = 1;
	
	// Prop_Seq = 2 x TQ = 1
	BRGCON2bits.PRSEG2 = 0;
	BRGCON2bits.PRSEG1 = 0;
	BRGCON2bits.PRSEG0 = 1;
	
	//  Enableing bus wake-up
	BRGCON3bits.WAKDIS = 0;
	
	// Dont use filter when wakeup
	BRGCON3bits.WAKFIL = 0;
	
	// Phase_Seg2 = 2 x TQ = 1
	BRGCON3bits.SEG2PH2 = 0;
	BRGCON3bits.SEG2PH1 = 0;
	BRGCON3bits.SEG2PH0 = 1;

	ECANCON=0;
	ECANCONbits.MDSEL1 = 0;
	ECANCONbits.MDSEL0 = 1;


	// FILTERS AND MASKS
	RXB0CONbits.RXM1=1;
	RXB0CONbits.RXM0=0;


	// INTERRUPTS
	
	// Diable transmit interrupt
	TXBIE = 0;

	//rx no interrupt
	PIR3 = 0;
	PIE3 = 0; 


	// Programmable no buffers interrupt
	BIE0  = 0x00;	


	//loopback mode or not
	#ifdef CAN_LOOPBACK_MODE
		CANCON = 0x40;//request loopback mode
		while ((CANSTAT & 0xE0) != 0x40 );//wait for loopback mode
	#else
		CANCON = 0x00;//request normal mode
		while ((CANSTAT & 0xE0) != 0x00 );//wait for normal mode
	#endif

}




/*
*	Function: canGetPacket
*
*	Input:	none
*	Output: none
*	Pre-conditions: a call to canISR and an packet is available and banked to RXB0
*	Affects: Calls canParse() function in main for parsing received packet.
*	Depends: none.
*/
BOOL canGetPacket()
{

		// Get which buffer that is ful.
			 if (RXB0CONbits.RXFUL) { ECANCON=(ECANCON&0b00000)|0b10000;  }
		else if (RXB1CONbits.RXFUL) { ECANCON=(ECANCON&0b00000)|0b10001;  }
		else if (B0CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10010;  }
		else if (B1CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10011;  }
		else if (B2CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10100;  }
		else if (B3CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10101;  }
		else if (B4CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10110;  }
		else if (B5CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10111;  }
		else return FALSE;


		//RXB0SIDH 28 27 26 25 24 23 22 21
        //RXB0SIDL 20 19 18 xx xx xx 17 16
        //RXB0EIDH 15 14 13 12 11 10 09 08
        //RXB0EIDL 07 06 05 04 03 02 01 00

		//funct xx xx xx xx 28 27 26 25
		//funcc xx xx xx xx xx xx 24 23 22 21 20 19 18 17 16 15 
		//nid   xx xx 14 13 12 11 10 09 
		//sid   xx xx xx xx xx xx xx 08 07 06 05 04 03 02 01 00


       	cm.funct=((RXB0SIDH & 0xF0)>>4);
        cm.funcc=(((WORD)(RXB0SIDH & 0x0F))<<6)+(((WORD)(RXB0SIDL & 0xE0))>>2)+(((WORD)(RXB0SIDL & 0x03))<<1)+(((WORD)(RXB0EIDH & 0x80))>>7);
        cm.nid=((RXB0EIDH & 0x7E)>>1);
        cm.sid=(((WORD)(RXB0EIDH & 0x01))<<8)+RXB0EIDL;


		// Data length
		cm.data_length=(RXB0DLCbits.DLC3<<3)+(RXB0DLCbits.DLC2<<2)+(RXB0DLCbits.DLC1<<1)+RXB0DLCbits.DLC0;

		// Remote request
		cm.remote_request=(RXB0DLCbits.RXRTR==0?FALSE:TRUE);

		// Data one bye one, for speed
		cm.data[0]=RXB0D0;	cm.data[1]=RXB0D1;
		cm.data[2]=RXB0D2; 	cm.data[3]=RXB0D3;
		cm.data[4]=RXB0D4; 	cm.data[5]=RXB0D5;
		cm.data[6]=RXB0D6; 	cm.data[7]=RXB0D7;

		// Clear ful status
		RXB0CONbits.RXFUL=0;

		return TRUE;
}


/*
*	Function: canSendMessage
*
*	Input:	CAN_MESSAGE to send
*	Output: True if packet scheduled sucess, false otherwise.
*	Pre-conditions: canInit
*	Affects: ..
*	Depends: ..
*/
BOOL canSendMessage(CAN_PROTO_MESSAGE cm,CAN_PRIORITY prio)
{
	
	if ( TXB0CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00011; } 
	if ( TXB1CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00100; } 
	if ( TXB2CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00101; } 
	// None of the transmit buffers were empty. 
	else { return FALSE;} 

	if (prio<0 || prio>3) prio = 0;


		//RXB0SIDH 28 27 26 25 24 23 22 21
        //RXB0SIDL 20 19 18 xx xx xx 17 16
        //RXB0EIDH 15 14 13 12 11 10 09 08
        //RXB0EIDL 07 06 05 04 03 02 01 00

		//funct xx xx xx xx 28 27 26 25
		//funcc xx xx xx xx xx xx 24 23 22 21 20 19 18 17 16 15 
		//nid   xx xx 14 13 12 11 10 09 
		//sid   xx xx xx xx xx xx xx 08 07 06 05 04 03 02 01 00

		RXB0SIDH = (BYTE)((cm.funct & 0x0F)<<4)+(BYTE)((cm.funcc & 0x03C0)>>6);
		RXB0SIDL = (BYTE)((cm.funcc & 0x003F)<<2)+(BYTE)((cm.funcc & 0x0006)>>1);
		RXB0EIDH = (BYTE)((cm.funcc & 0x0001)<<7)+(BYTE)((cm.nid & 0x3F)<<1)+(BYTE)((cm.sid & 0x0100)>>8);
		RXB0EIDL = (BYTE)((cm.sid & 0x00FF));

		// Data length
		RXB0DLC = 0;
		if (cm.data_length>8) RXB0DLC=8; else RXB0DLC=cm.data_length;
		
		// Remote request
		if (cm.remote_request==TRUE)
		{
			_asm 
				bsf RXB0DLC, 6, 0
			_endasm 
		}
		else
		{
			_asm 
				bcf RXB0DLC, 6, 0
			_endasm 
		}

		// Data one bye one, for speed
		RXB0D0=cm.data[0];	RXB0D1=cm.data[1];
		RXB0D2=cm.data[2]; 	RXB0D3=cm.data[3];
		RXB0D4=cm.data[4]; 	RXB0D5=cm.data[5];
		RXB0D6=cm.data[6]; 	RXB0D7=cm.data[7];

		// set extended or not
		RXB0SIDLbits.EXID=1;

		// Priority
		RXB0CON = (RXB0CON & 0b11111100) | prio;

		// mark as redy to transmit
		_asm 
			bsf RXB0CON, 3, 0
		_endasm 

		return TRUE;

}



#endif //USE_CAN




void TBLRD_Func()
{
_asm
	TBLRD
_endasm
}

void TBLWT_Func()
{
_asm
	TBLWT
_endasm
}


void TBLWTPOSTINC_Func (void){
_asm
	TBLWTPOSTINC
_endasm
}

DWORD tickGet()
{
	return tickCounter;
}


#pragma code user = RM_RESET_VECTOR
