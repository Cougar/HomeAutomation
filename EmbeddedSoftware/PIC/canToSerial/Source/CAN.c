/*********************************************************************
 *
 *                  CAN driver
 *
 *********************************************************************
 * FileName:        CAN.c
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#include "../Include/CANdefs.h"
#include "../Include/stackTasks.h"
#include "../Include/CAN.h"
#include "../Include/uart.h"

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

		// Set extended mode or not.
		cm.extended=(RXB0SIDLbits.EXID==0?FALSE:TRUE);

		// Clear
		cm.ident=0;


		if (cm.extended==TRUE)
		{
			//Read extended ident.

			//xx xx xx 28 27 26 25 24   23 22 21 20 19 18 17 16   15 14 13 12 11 10 09 08   07 06 05 04 03 02 01 00

        	//RXB0SIDH 28 27 26 25 24 23 22 21
        	//RXB0SIDL 20 19 18 xx xx xx 17 16
        	//RXB0EIDH 15 14 13 12 11 10 09 08
        	//RXB0EIDL 07 06 05 04 03 02 01 00


        	cm.ident = (((DWORD)RXB0SIDH)<<21) + (((DWORD)(RXB0SIDL & 0xE0))<<13) + (((DWORD)(RXB0SIDL & 0x03))<<16) + (((DWORD)RXB0EIDH)<<8) + (BYTE)RXB0EIDL;

		}
		else
		{
			//Read standard ident.

			//xx xx xx xx xx 10 09 08   07 06 05 04 03 02 01 00

        	//RXB0SIDH 10 09 08 07 06 05 04 03
        	//RXB0SIDL 02 01 00 xx xx xx xx xx

			cm.ident = (((DWORD)RXB0SIDH)<<3) + (((DWORD)RXB0SIDL)>>5);
		}

		// Data length
		cm.data_length=RXB0DLC;

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
BOOL canSendMessage(CAN_MESSAGE cm)
{
	
	if ( TXB0CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00011; } 
	if ( TXB1CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00100; } 
	if ( TXB2CONbits.TXREQ == 0 )  { ECANCON=(ECANCON&0b00000)|0b00101; } 
	// None of the transmit buffers were empty. 
	else { return FALSE;} 


		if (cm.extended==TRUE)
		{
			//Write extended ident.
	
			//xx xx xx 28 27 26 25 24   23 22 21 20 19 18 17 16   15 14 13 12 11 10 09 08   07 06 05 04 03 02 01 00

        	//RXB0SIDH 28 27 26 25 24 23 22 21
        	//RXB0SIDL 20 19 18 xx xx xx 17 16
        	//RXB0EIDH 15 14 13 12 11 10 09 08
        	//RXB0EIDL 07 06 05 04 03 02 01 00


        	RXB0SIDH =  (BYTE)((cm.ident>>21));
        	RXB0SIDL = ((BYTE)(cm.ident>>16) & 0x03) + ((BYTE)(cm.ident>>13) & 0xE0);
        	RXB0EIDH =  (BYTE)(cm.ident>>8);
        	RXB0EIDL =  (BYTE)cm.ident;

		}
		else
		{
			//Write standard ident.
		
			//xx xx xx xx xx 10 09 08   07 06 05 04 03 02 01 00

        	//RXB0SIDH 10 09 08 07 06 05 04 03
        	//RXB0SIDL 02 01 00 xx xx xx xx xx

       		RXB0SIDH=  (BYTE)(cm.ident>>3);
        	RXB0SIDL= ((BYTE)(cm.ident<<5) & 0xE0);

		}

		// Data length
		if (cm.data_length>8) RXB0DLC=8; else RXB0DLC=cm.data_length;

		// Data one bye one, for speed
		RXB0D0=cm.data[0];	RXB0D1=cm.data[1];
		RXB0D2=cm.data[2]; 	RXB0D3=cm.data[3];
		RXB0D4=cm.data[4]; 	RXB0D5=cm.data[5];
		RXB0D6=cm.data[6]; 	RXB0D7=cm.data[7];

		// set extended or not
		RXB0SIDLbits.EXID=(cm.extended==TRUE?1:0);


		// mark as redy to transmit
		_asm 
			bsf RXB0CON, 3, 0
		_endasm 

		return TRUE;

}



#endif //USE_CAN
