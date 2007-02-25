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
#include <CANdefs.h>
#include <CAN.h>
#include <crc16.h>
#include <boot.h>
#include <funcdefs.h>
#include <EEaccess.h>
#include <reset.h>

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



static CAN_PACKET cp;
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

static int MY_ID = DEFAULT_ID;

void main()
{
	static PROGRAM_STATE pgs = pgsWAITING_START;
	static PROGRAM_STATE pgsAckNext = pgsWAITING_START;

	static BYTE programmerId=0;
	static BYTE pgmData[64];

	static BOOT_TYPE ackType = btACK;
	
	static DWORD pgmAddress = 0;
	static DWORD newPgmAddress = 0;
	static DWORD tBoot;
	static DWORD t = 0;
	
	CAN_PACKET outCp;

	// Inits
	mainInit();

	canInit();

	tBoot = tickGet();
	
	// Read ID if exist.
	if (EERead(NODE_ID_EE)==NODE_HAS_ID)
	{
		MY_ID=EERead(NODE_ID_EE + 1);
	}
	
	for(t=0;t<64;t++) pgmData[t]=0x0;

	// Send bootloader startup
	outCp.type=ptPGM;
	outCp.pgm.class=pcCTRL;
	outCp.pgm.id=pctBOOTBOOT;
	outCp.length=1;
	outCp.data[0] = (isBOR()<<7)+(isLVD()<<6)+(isMCLR()<<5)+(isPOR()<<4)+(isWDTTO()<<3)+(isWDTWU()<<2)+(isWU()<<1);
	StatusReset();
	while(!canSendMessage(outCp));
	
	LED0_IO= 1;

	for(;;)
	{
		BYTE i;
		CAN_PACKET outCm;
		BOOL hasPacket = canGetPacket() && (cp.type == ptBOOT && cp.boot.rid == MY_ID);

		tickSubCounter++;
		if (tickSubCounter>TICK_SUB_COUNTER_10MS)
		{
			tickCounter++;
			tickSubCounter=0;
			ClrWdt();
		}

		switch(pgs)
		{
			case pgsWAITING_START:
				// Wait for first addresspacket.
				// Set startAddress to data[2]:[0].
				// Send ack. Goto next state.
				if (hasPacket==TRUE && cp.boot.type==btADDR)
				{
					programmerId = cp.sid;
					pgmAddress = (((DWORD)cp.data[2])<<16)+(((DWORD)cp.data[1])<<8)+((DWORD)cp.data[0]);
					t = tickGet();
					ackType = btACK;
					pgsAckNext = pgsWAIT_FIRST_DATA;
					pgs = pgsSEND_ACK;
				}	

				// Bootloader timeout
				if ((tickGet()-tBoot)>BOOTLOADER_INIT_TIMOUT)
				{
					pgs = pgsDONE;
				}
				break;

			case pgsSEND_ACK:
				// send ack
				outCp.type = ptBOOT;
				outCp.boot.rid = programmerId;
				outCp.boot.type = ackType;
				outCp.length=0;
				t = tickGet();
				pgs	= pgsAckNext;

				while(!canSendMessage(outCp));
				break;



			case pgsWAIT_FIRST_DATA:
				// Wait for first pgm packet. use offset to determine data index.
				// When receiving crc from host, check crc for offsets bytes.
				
				if ((tickGet()-t)>PACKET_TIMEOUT)
				{
					// Timeout, resend ack/nack.
					pgsAckNext = pgsWAIT_FIRST_DATA;
					pgs = pgsSEND_ACK;
				}				

				if (hasPacket==TRUE && cp.boot.type==btPGM)
				{
					BYTE localIndex = cp.boot.offset;
					pgmData[localIndex+0]=cp.data[0]; pgmData[localIndex+1]=cp.data[1]; pgmData[localIndex+2]=cp.data[2]; pgmData[localIndex+3]=cp.data[3];
					pgmData[localIndex+4]=cp.data[4]; pgmData[localIndex+5]=cp.data[5]; pgmData[localIndex+6]=cp.data[6]; pgmData[localIndex+7]=cp.data[7];
					pgs = pgsWAIT_DATA_OR_CRC;
				}

				if (hasPacket==TRUE && cp.boot.type==btADDR)
				{
					tBoot = tickGet();
					pgs   = pgsWAITING_START;
				}

				if (hasPacket==TRUE && cp.boot.type==btCRC)
				{
					pgs   = pgsWAIT_DATA_OR_CRC;
				}

				if (hasPacket==TRUE && cp.boot.type==btDONE)
				{
					pgs = pgsDONE;
				}


				break;

			case pgsWAIT_DATA_OR_CRC:
				// Wait for pgm packet. use offset to determine data index.
				// When receiving crc from host, check crc for offsets bytes.
				
				if (hasPacket==TRUE && cp.boot.type==btCRC)
				{
					WORD crc16 = (((WORD)cp.data[4])<<8)+((WORD)cp.data[3]);
					WORD crc16calc = calc_crc(pgmData,64);

					// check CRC.
					if (crc16calc==crc16)
					{
						newPgmAddress = (((DWORD)cp.data[2])<<16)+(((DWORD)cp.data[1])<<8)+((DWORD)cp.data[0]);
						pgsAckNext = pgsWAIT_FIRST_DATA;
						pgs = pgsWRITE_DATA;
					}
					else
					{
						t = tickGet();
						ackType = btNACK;
						pgsAckNext = pgsWAIT_FIRST_DATA;
						pgs = pgsSEND_ACK;
					}
				}
				
				if (hasPacket==TRUE && cp.boot.type==btPGM)
				{
					BYTE localIndex = cp.boot.offset;
					pgmData[localIndex+0]=cp.data[0]; pgmData[localIndex+1]=cp.data[1]; pgmData[localIndex+2]=cp.data[2]; pgmData[localIndex+3]=cp.data[3];
					pgmData[localIndex+4]=cp.data[4]; pgmData[localIndex+5]=cp.data[5]; pgmData[localIndex+6]=cp.data[6]; pgmData[localIndex+7]=cp.data[7];
				}

				if (hasPacket==TRUE && cp.boot.type==btADDR)
				{
					tBoot = tickGet();
					pgs   = pgsWAITING_START;
				}

				if (hasPacket==TRUE && cp.boot.type==btDONE)
				{
					pgs = pgsDONE;
				}

				break;


			case pgsWRITE_DATA:

				ClrWdt();

				// Write program and send ack.

				// Check addr limit
				if (pgmAddress<RM_RESET_VECTOR)
				{		
					ackType = btNACK;
					pgsAckNext = pgsWAIT_FIRST_DATA;		
					pgs 			= pgsSEND_ACK;
					//Not allowed to write here!
					break;
				}

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
					pgmData[i]=0x0;
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

				pgmAddress	   = newPgmAddress;
				ackType = btACK;		
				pgs 			= pgsSEND_ACK;

			break;

			case pgsDONE:
				LED0_IO= 0;
				_asm  goto RM_RESET_VECTOR _endasm
			break;

		
			default:
				pgs = pgsWAITING_START;
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

	// 500Khz@40Mhz (new by the book)
	BRGCON1 = 0x04;
	BRGCON2 = 0xD1;
	BRGCON3 = 0x81;

	ECANCON=0;
	ECANCONbits.MDSEL1 = 1;
	ECANCONbits.MDSEL0 = 0;


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
		ECANCON=(ECANCON&0b00000)|(0b10000|(CANCON&0x0F)); 
		if (!RXB0CONbits.RXFUL) return FALSE; 
	

		//RXB0SIDH 28 27 26 25 24 23 22 21
        //RXB0SIDL 20 19 18 xx xx xx 17 16
        //RXB0EIDH 15 14 13 12 11 10 09 08
        //RXB0EIDL 07 06 05 04 03 02 01 00

		cp.type = ((RXB0SIDH&0xC)>>6);


		if (cp.type==ptBOOT)
		{
			cp.boot.type  = ((RXB0SIDH&0x38)>>3);
			cp.boot.offset= ((RXB0SIDH&0x7)<<5)+((RXB0SIDL&0xE0)>>3)+(RXB0SIDL&0x3);
			cp.boot.rid   = RXB0EIDH;
		}
		else
		{
			cp.pgm.class= ((RXB0SIDH&0x3C)>>2);
			cp.pgm.id   = (((WORD)RXB0SIDH&0x3)<<13)+(((WORD)RXB0SIDL&0xE0)<<5)+((RXB0SIDL&0x3)<<8)+RXB0EIDH;
		}
		cp.sid   = RXB0EIDL;


		// Data length
		cp.length=(RXB0DLCbits.DLC3<<3)+(RXB0DLCbits.DLC2<<2)+(RXB0DLCbits.DLC1<<1)+RXB0DLCbits.DLC0;

		// Data one bye one, for speed
		cp.data[0]=RXB0D0;	cp.data[1]=RXB0D1;
		cp.data[2]=RXB0D2; 	cp.data[3]=RXB0D3;
		cp.data[4]=RXB0D4; 	cp.data[5]=RXB0D5;
		cp.data[6]=RXB0D6; 	cp.data[7]=RXB0D7;

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
BOOL canSendMessage(CAN_PACKET cp)
{
	BYTE prio = 3;	

	if ( TXB0CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00011; } 
	if ( TXB1CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00100; } 
	if ( TXB2CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00101; } 
	// None of the transmit buffers were empty. 
	else { return FALSE;} 


		//RXB0SIDH 28 27 26 25 24 23 22 21
        //RXB0SIDL 20 19 18 xx xx xx 17 16
        //RXB0EIDH 15 14 13 12 11 10 09 08
        //RXB0EIDL 07 06 05 04 03 02 01 00

		if (cp.type==ptBOOT)
		{
			RXB0SIDH = ((cp.type&0x3)<<6)+((cp.boot.type&0x7)<<3)+((cp.boot.offset&0xE0)>>5);
			RXB0SIDL = ((cp.boot.offset&0x1C)<<3)+(cp.boot.offset&0x3);
			RXB0EIDH = cp.boot.rid;
		}
		else
		{
			RXB0SIDH = ((cp.type&0x3)<<6)+((cp.pgm.class&0xF)<<2)+((cp.pgm.id&0x6000)>>13);
			RXB0SIDL = ((cp.pgm.id&0x1C00)>>5)+((cp.pgm.id&0x300)>>8);
			RXB0EIDH = (cp.pgm.id&0xFF);
		}
		RXB0EIDL = MY_ID;

		// Data length
		RXB0DLC = 0;
		if (cp.length>8) RXB0DLC=8; else RXB0DLC=cp.length;
		
		// NOT Remote request
		_asm 
			bcf RXB0DLC, 6, 0
		_endasm 


		// Data one bye one, for speed
		RXB0D0=cp.data[0];	RXB0D1=cp.data[1];
		RXB0D2=cp.data[2]; 	RXB0D3=cp.data[3];
		RXB0D4=cp.data[4]; 	RXB0D5=cp.data[5];
		RXB0D6=cp.data[6]; 	RXB0D7=cp.data[7];

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
