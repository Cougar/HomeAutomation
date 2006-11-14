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

static void canGetPacket(void);

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


	if (PIR3bits.RXBnIF || PIR3bits.RXB1IF || PIR3bits.RXB0IF)
	{
					

		// Get which buffer that is ful.
			 if (RXB0CONbits.RXFUL) { ECANCON=(ECANCON&0b00000)|0b10000; canGetPacket(); }
		else if (RXB1CONbits.RXFUL) { ECANCON=(ECANCON&0b00000)|0b10001; canGetPacket(); }
		else if (B0CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10010; canGetPacket(); }
		else if (B1CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10011; canGetPacket(); }
		else if (B2CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10100; canGetPacket(); }
		else if (B3CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10101; canGetPacket(); }
		else if (B4CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10110; canGetPacket(); }
		else if (B5CONbits.RXFUL) 	{ ECANCON=(ECANCON&0b00000)|0b10111; canGetPacket(); }
	
		if (PIR3bits.RXBnIF) {PIR3bits.RXBnIF = 0;}
		if (PIR3bits.RXB1IF) {PIR3bits.RXB1IF = 0;}
		if (PIR3bits.RXB0IF) {PIR3bits.RXB0IF = 0;}
	
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
		CAN_MESSAGE cm;

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

		// Data one bye one, for speed
		cm.data[0]=RXB0D0;	cm.data[1]=RXB0D1;
		cm.data[2]=RXB0D2; 	cm.data[3]=RXB0D3;
		cm.data[4]=RXB0D4; 	cm.data[5]=RXB0D5;
		cm.data[6]=RXB0D6; 	cm.data[7]=RXB0D7;

		// Clear ful status
		RXB0CONbits.RXFUL=0;

		//Call main parser
		canParse(cm);
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
BOOL canSendMessage(CAN_MESSAGE cm,CAN_PRIORITY prio)
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
		

		// NOT Remote request
		_asm 
			bcf RXB0DLC, 6, 0
		_endasm 
		

		// Data one bye one, for speed
		RXB0D0=cm.data[0];	RXB0D1=cm.data[1];
		RXB0D2=cm.data[2]; 	RXB0D3=cm.data[3];
		RXB0D4=cm.data[4]; 	RXB0D5=cm.data[5];
		RXB0D6=cm.data[6]; 	RXB0D7=cm.data[7];

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



#endif //USE_CAN
