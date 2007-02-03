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
static BYTE MY_NID = DEFAULT_NID;
static CAN_MESSAGE outCm;

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
		MY_ID=(((WORD)EERead(NODE_ID_EE + 1))<<8)+EERead(NODE_ID_EE + 2);
		MY_NID=EERead(NODE_ID_EE + 3);
	}

	// Send user program startup heatbeat
	outCm.funct 					= FUNCT_BOOTLOADER;
	outCm.funcc 					= FUNCC_BOOT_HEARTBEAT;
	outCm.data_length 				= 1;
	outCm.data[BOOT_DATA_HEARTBEAT_INDEX] = HEARTBEAT_USER_STARTUP;
	canSendMessage(outCm,PRIO_HIGH);

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
		outCm.funct 					= FUNCT_BOOTLOADER;
		outCm.funcc 					= FUNCC_BOOT_HEARTBEAT;
		outCm.data_length 				= 1;
		outCm.data[BOOT_DATA_HEARTBEAT_INDEX] = HEARTBEAT_ALIVE;
		canSendMessage(outCm,PRIO_HIGH);
			
		heartbeat = tickGet();
	}

	ClrWdt();
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



		// Check if FUNCT_BOOTLOADER specific.
		// reset and id change
		if (cm.funct==FUNCT_BOOTLOADER)
		{
			if (cm.nid==MY_NID && ((((unsigned int)cm.data[BOOT_DATA_RID_HIGH_INDEX])<<8)+cm.data[BOOT_DATA_RID_LOW_INDEX])==MY_ID) 
			{
				if (cm.funcc==FUNCC_BOOT_INIT) Reset(); // Reset me.
				if (cm.funcc==FUNCC_BOOT_SET_ID)
				{
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
						outCm.data_length 				= 8;
						outCm.data[BOOT_DATA_ERR_INDEX]	= ACK_ERR_NO_ERROR;
						while(!canSendMessage(outCm,PRIO_HIGH));
				}
			}
		}


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

		// Append my id and nid
		cm.nid	= MY_NID;
		cm.sid	= MY_ID;


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
