/*********************************************************************
 *
 *                  CAN module
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#include <CANdefs.h>
#include <stackTasks.h>
#include <CAN.h>
#include <EEaccess.h>
#include <funcdefs.h>
#include <Tick.h>

TICK heartbeat;
static int MY_ID = DEFAULT_ID;
static CAN_PACKET outCp;

static void canGetPacket(void);


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

	//BRGCON1=0;
	
	// Sync_Seq = 2 x TQ = 1
	//BRGCON1bits.SJW1=0;
	//BRGCON1bits.SJW0=1;
	
	// Setting BRP.
	//BRGCON1=BRGCON1|CAN_BRP;
	
	// Maximum PHEG1
	//BRGCON2bits.SEG2PHTS = 1;
	
	// Phase_Seg1 = 2 x TQ = 1
	//BRGCON2bits.SEG1PH2 = 0;
	//BRGCON2bits.SEG1PH1 = 0;
	//BRGCON2bits.SEG1PH0 = 1;
	
	// Prop_Seq = 2 x TQ = 1
	//BRGCON2bits.PRSEG2 = 0;
	//BRGCON2bits.PRSEG1 = 0;
	//BRGCON2bits.PRSEG0 = 1;
	
	//  Enableing bus wake-up
	//BRGCON3bits.WAKDIS = 0;
	
	// Dont use filter when wakeup
	//BRGCON3bits.WAKFIL = 0;
	
	// Phase_Seg2 = 2 x TQ = 1
	//BRGCON3bits.SEG2PH2 = 0;
	//BRGCON3bits.SEG2PH1 = 0;
	//BRGCON3bits.SEG2PH0 = 1;

	ECANCON=0;
	ECANCONbits.MDSEL1 = 1; // 0  For mode 1 
	ECANCONbits.MDSEL0 = 0; // 1  For mode 1 


	// FILTERS AND MASKS
	RXB0CONbits.RXM1=1;
	RXB0CONbits.RXM0=0;


	// INTERRUPTS
	
	// Diable transmit interrupt
	TXBIE = 0;

	//rx any interrupt
	PIR3 = 0;
	PIE3 = 0b00000011; 

	// High interrupt
	IPR3 =0b00000011;

	// Programmable buffers interrupt
	BIE0  = 0xFF;	


	//loopback mode or not
	#ifdef CAN_LOOPBACK_MODE
		CANCON = 0x40;//request loopback mode
		while ((CANSTAT & 0xE0) != 0x40 );//wait for loopback mode
	#else
		CANCON = 0x00;//request normal mode
		while ((CANSTAT & 0xE0) != 0x00 );//wait for normal mode
	#endif


	// Init ticker service
	tickInit();


	// Read ID and NID if exist.
	if (EERead(NODE_ID_EE)==NODE_HAS_ID)
	{
		MY_ID=+EERead(NODE_ID_EE + 1);
	}

	// Send user program startup heatbeat
	outCp.type=ptPGM;
	outCp.pgm.class=pcCTRL;
	outCp.pgm.id=pctAPPBOOT;
	outCp.length=2;
	outCp.data[1]=(BYTE)((APP_VERSION & 0xFF00)>>8);
	outCp.data[0]=(BYTE)(APP_VERSION & 0xFF);
	while(!canSendMessage(outCp,PRIO_HIGH));
}


/*
*	Function: canISR
*
*	Input:	none
*	Output: none
*	Pre-conditions: a call to canInit()
*	Affects: interrupt registers and receive data buffers
*	Depends: ..
*/
void canISR()
{
	ClrWdt();

	if (PIR3bits.RXBnIF || PIR3bits.RXB1IF || PIR3bits.RXB0IF)
	{
		ECANCON=(ECANCON&0b00000)|(0b10000|(CANCON&0x0F)); 
		if (RXB0CONbits.RXFUL) canGetPacket(); 
	
		if (PIR3bits.RXBnIF) {PIR3bits.RXBnIF = 0; return;}
		if (PIR3bits.RXB1IF) {PIR3bits.RXB1IF = 0; return;}
		if (PIR3bits.RXB0IF) {PIR3bits.RXB0IF = 0; return;}
	}

	tickUpdate();

	if ((tickGet()-heartbeat)>TICK_SECOND*5)
	{
		// Send alive heartbeat
		outCp.type=ptPGM;
		outCp.pgm.class=pcCTRL;
		outCp.pgm.id=pctHEARTBEAT;
		outCp.length=0;
		canSendMessage(outCp,PRIO_HIGH);
			
		heartbeat = tickGet();
	}

	
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
void canGetPacket()
{
		CAN_PACKET cp;

		//RXB0SIDH 28 27 26 25 24 23 22 21
        //RXB0SIDL 20 19 18 xx xx xx 17 16
        //RXB0EIDH 15 14 13 12 11 10 09 08
        //RXB0EIDL 07 06 05 04 03 02 01 00


		cp.type = (BYTE)((RXB0SIDH&0xC0)>>6);


		if (cp.type==ptBOOT)
		{
			cp.boot.type  = ((RXB0SIDH&0x38)>>3);
			cp.boot.offset= ((RXB0SIDH&0x7)<<5)+((RXB0SIDL&0xE0)>>3)+(RXB0SIDL&0x3);
			cp.boot.rid   = RXB0EIDH;
		}
		else
		{
			cp.pgm.class= ((RXB0SIDH&0x3C)>>2);
			cp.pgm.id   = (((WORD)RXB0SIDH&0x3)<<13)+(((WORD)RXB0SIDL&0xE0)<<5)+(((WORD)RXB0SIDL&0x3)<<8)+(WORD)RXB0EIDH;
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



		// Check if bootloader request specific.
		// reset and id change
		if (cp.type==ptBOOT)
		{
			if (cp.boot.rid==MY_ID)
			{
				if (cp.boot.type==btADDR) Reset(); // Reset me.
				if (cp.boot.type==btCHANGEID)
				{
						// Change my ID and NID
						MY_ID=cp.data[0];
						EEWrite(NODE_ID_EE+1,cp.data[0]);
						EEWrite(NODE_ID_EE,NODE_HAS_ID);
						// Send ack with new id.
						outCp.type=ptBOOT;
						outCp.boot.type=btACK;
						outCp.length=0;
						while(!canSendMessage(outCp,PRIO_HIGH));
				}
			}
		}


		//Call main parser
		if (cp.type==ptPGM) canParse(cp);
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
BOOL canSendMessage(CAN_PACKET cp, CAN_PRIORITY prio)
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
	// set extended
	RXB0SIDLbits.EXID=1;
	
	// Priority
	RXB0CON = (RXB0CON & 0b11111100) | prio;
	
	// mark as redy to transmit
	_asm 
		bsf RXB0CON, 3, 0
	_endasm 
	
	return TRUE;

}
