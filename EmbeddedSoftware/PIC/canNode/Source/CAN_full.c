#include "../Include/CANdefs.h"
#include "../Include/stackTasks.h"
#include "../Include/CAN.h"
#include "../Include/uart.h"


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

	// 1. Ensure that the ECAN module is in Configuration mode.
	CANCON=0;

	CANCONbits.REQOP2=1; // Request configruation mode
	while(!CANSTATbits.OPMODE2); // Wait for configuration mode

	//2. Select ECAN operational mode.
	#ifdef CAN_ENCHANCED_MODE
		// Use Mode 1: Enchanged Legacy mode
		// ECANCON.MDSEL1:MDSEL0 = 1
		ECANCONbits.MDSEL1 = 0;
		ECANCONbits.MDSEL0 = 1;
	#else
		// Use Mode 0: Legacy mode
		// ECANCON.MDSEL1:MDSEL0 = 0
		ECANCONbits.MDSEL1 = 0;
		ECANCONbits.MDSEL0 = 0;
	#endif


	// BUFFERS
	
	// Make all programmable buffers receive buffers
	BSEL0 = 0x00;
	
	// Setup the receive buffers
	RXB0CON = 0;
	RXB1CON = 0;
	B0CON = 0;
	B1CON = 0;
	B2CON = 0;
	B3CON = 0;
	B4CON = 0;
	B5CON = 0;



	//3. Set up the baud rate registers.
	// Times for different time segments:
	// Sync_Seq + Prop_Seq + Phase_Seg1 + Phase_Seg2 = 8
	// Sync_Seq = 2, Prop_Seq = 2, Phase_Seg1 = 2, Phase_Seg2 = 2
	// BRGCON1.SJW1:SJW0 = 1
	// BRGCON1.BRP5:BRP0 = CAN_BRP
	// BRGCON2.SEG2PHTS = 0 //Maximum PHEG1
	// BRGCON2.SEG1PH2:SEG1PH0 = 1
	// BRGCON2.PRSEG2:PRSEG0 = 1
	// BRGCON3.WAKDIS = 1/0  1=Dsiable CAN bus wake up
	// BRGCON3.WAKFIL = 1/0  1=Use filter on wake up
	// BRGCON3.SEG2PH2:SEG2PH0 = 1
	
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
	
	
	//4. Set up the filter and mask registers.
	
	// Disable all filters
	RXFCON0 = 0;
	RXFCON1 = 0;


	// No data byte filtering
	SDFLC = 0;
	
	// Setup filter/buffer association, two filters per buffer
	RXFBCON0 = 0x00;
	RXFBCON1 = 0x11;
	RXFBCON2 = 0x22;
	RXFBCON3 = 0x33;
	RXFBCON4 = 0x44;
	RXFBCON5 = 0x55;
	RXFBCON6 = 0x66;
	RXFBCON7 = 0x77;

	// Setup filter/mask association to Mask 0
	MSEL0 = MSEL1 = MSEL2 = MSEL3 = 0;
	
	// Set Mask 0 to not mask any filter bit
	RXM0SIDH = 0xFF;
	RXM0SIDL = 0xE3;
	RXM0EIDH = 0xFF;
	RXM0EIDL = 0xFF;	
	
	// Set I/O control CANTX pin will drive VDD when recessive
	CIOCON = 0x20;

	// Set interrupts	
	// Diable transmit interrupt
	TXBIE = 0;

	//rx any interrupt
	PIR3 = 0;
	PIE3 = 0b00000011; 

	// High interrupt
	IPR3 =0b00000011;

	// Programmable buffers interrupt
	BIE0  = 0xFF;	

	//5. Set the ECAN module to normal mode or any other mode required by the application logic.
	#ifdef CAN_LOOPBACK_MODE
		// Use Loopback mode
		CANCON=0b01000000;
	#else
		// Use Normal mode
		CANCON=0b00000000;
	#endif

	while(CANSTATbits.OPMODE2); // Wait for configuration mode
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
	BYTE defECANCON;


	if (PIR3bits.RXBnIF==1 && PIE3bits.RXBnIE==1 || PIR3bits.RXB1IF==1 || PIR3bits.RXB0IF==1)
	{
		uartPutrs("canISR!");


		// Save default ECANCON 
		#ifdef CAN_ENCHANCED_MODE
			defECANCON=(ECANCON&0b00000)|0b10000;
		#else
			defECANCON=(CANCON&0b000)|0b000;
		#endif

			 if (RXB0CONbits.RXFUL==1) 	
			{ 			
				#ifdef CAN_ENCHANCED_MODE
					ECANCON = defECANCON|0b10000; 
				#else
					CANCON = defECANCON|0b000; 
				#endif

				canGetPacket(); 
				RXB0CONbits.RXFUL=0; 
			}
		else if (RXB1CONbits.RXFUL==1) 	
		{ 

			#ifdef CAN_ENCHANCED_MODE
				ECANCON = defECANCON|0b10001; 
			#else
				CANCON = defECANCON|0b101; 
			#endif	
	
			canGetPacket(); 
			RXB1CONbits.RXFUL=0; 
		}
		#ifdef CAN_ENCHANCED_MODE
			else if (B0CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10010; canGetPacket(); B0CONbits.RXFUL=0; }
			else if (B1CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10011; canGetPacket(); B1CONbits.RXFUL=0; }
			else if (B2CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10100; canGetPacket(); B2CONbits.RXFUL=0; }
			else if (B3CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10101; canGetPacket(); B3CONbits.RXFUL=0; }
			else if (B4CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10110; canGetPacket(); B4CONbits.RXFUL=0; }
			else if (B5CONbits.RXFUL==1)	{ ECANCON = defECANCON|0b10111; canGetPacket(); B5CONbits.RXFUL=0; }
		#endif
		else {PIR3bits.RXBnIF=0; PIR3bits.RXB1IF=0; PIR3bits.RXB0IF=0; return;}

		// Restore mapping
		#ifdef CAN_ENCHANCED_MODE
			ECANCON = defECANCON;
		#else
			CANCON = defECANCON;
		#endif	

		PIR3bits.RXBnIF=0;
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
		cm.extended=RXB0SIDLbits.EXID;

		// Clear
		cm.ident[0]=cm.ident[1]=cm.ident[2]=cm.ident[3]=0;


		if (cm.extended==TRUE)
		{
			//Read extended ident.
			cm.ident[0]=RXB0EIDL;
			cm.ident[1]=RXB0EIDH;
			cm.ident[2]=(RXB0SIDL & 0b00000011)+((RXB0SIDL & 0b11100000)>3)+((RXB0SIDH & 0b00000111)<5);
			cm.ident[3]=RXB0SIDH>3;
		}
		else
		{
			//Read standard ident.
			cm.ident[0]=((RXB0SIDL & 0b11100000)>5)+(RXB0SIDH>2);
			cm.ident[1]=RXB0SIDH>5;
		}

		// Data length
		cm.data_length=RXB0DLC;

		// Data one bye one, for speed
		cm.data[0]=RXB0D0;	cm.data[1]=RXB0D1;
		cm.data[2]=RXB0D2; 	cm.data[3]=RXB0D3;
		cm.data[4]=RXB0D4; 	cm.data[5]=RXB0D5;
		cm.data[6]=RXB0D6; 	cm.data[7]=RXB0D7;


		//Call main parser
		canParse(cm);

		//RXBnCON.RXFUL to know if there actually is a message.
		//RXB0 och RXB1 (0 <= n <= 1)
		//RXBnCON.FILHIT  do determine filter hit.

		//EXID =  1:extended frame
		//RXBnSIDH	= high standard ident (bit10->bit3 if standard, bit28->bit21 if ext)
		//RXBnSIDL(7:5)	= low standard ident (bit2->bit0 if standard, bit20->bit18 if ext)
		//RXBnSIDL(1:0)	= low standard ident (bit17->bit16 if ext)
		//RXBnEIDH		= high extended ident (bit15->bit8)
		//RXBnEIDL		= low extended ident (bit7->bit0)
		//RXBnDLC = Data length
		//RXBnDm = Data bytes. (0 <= m <= 7)

		// Same for Bn...  (1 <= n <= 5)

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
	BYTE defECANCON;
	
	// Save default ECANCON 
	#ifdef CAN_ENCHANCED_MODE
		defECANCON=(ECANCON&0b00000)|0b10000;
	#else
		defECANCON=(CANCON&0b000)|0b100;
	#endif

	// Find the first empty transmitter. 
	if ( TXB0CONbits.TXREQ == 0 ) 
	{ 
		// TxBuffer0 is empty. Set EWIN bits to point to TXB0 
		#ifdef CAN_ENCHANCED_MODE
			ECANCON=(defECANCON&0b00000)|0b00011;
		#else
			//CANCON=(defECANCON&0b000)|0b100;
		#endif
	} 
	else if ( TXB1CONbits.TXREQ == 0 ) 
	{ 
		// TxBuffer1 is empty. Set EWIN bits to point to TXB1 
		#ifdef CAN_ENCHANCED_MODE
			ECANCON=(defECANCON&0b00000)|0b00100;
		#else
			//CANCON=(defECANCON&0b000)|0b011;
		#endif
	} 
	else if ( TXB2CONbits.TXREQ == 0 ) 
	{ 
		// TxBuffer2 is empty. Set EWIN bits to point to TXB2 
		#ifdef CAN_ENCHANCED_MODE
			ECANCON=(defECANCON&0b00000)|0b00101;
		#else
			//CANCON=(defECANCON&0b000)|0b010;
		#endif
	} 
	// None of the transmit buffers were empty. 
	else { return FALSE;} 



		uartPutrs("Sending.."); 

		// Set extended mode or not.
		TXB0SIDLbits.EXIDE=cm.extended;

		if (cm.extended==TRUE)
		{
			//Write extended ident.
			TXB0SIDH=(cm.ident[3]<3)+(cm.ident[2]>5);
			TXB0SIDL=(TXB0SIDL & 0b00011100)+((cm.ident[2] & 0b0001100)<3)+(cm.ident[2] & 0b00000011);
			TXB0EIDH=cm.ident[1];
			TXB0EIDL=cm.ident[0];
		}
		else
		{
			//Write standard ident.
			TXB0SIDH=(cm.ident[1]<5)+(cm.ident[0]>3);
			TXB0SIDL=(TXB0SIDL & 0b00011111)+(cm.ident[0]<5);
		}

		// Data length
		if (cm.data_length>8) TXB0DLC=8; else TXB0DLC=cm.data_length;

		// Data one bye one, for speed
		TXB0D0=cm.data[0];	TXB0D1=cm.data[1];
		TXB0D2=cm.data[2]; 	TXB0D3=cm.data[3];
		TXB0D4=cm.data[4]; 	TXB0D5=cm.data[5];
		TXB0D6=cm.data[6]; 	TXB0D7=cm.data[7];

		// mark as redy to transmit
		TXB0CONbits.TXREQ=1;

		uartPutrs("While");
		while(TXB0CONbits.TXREQ);
		uartPutrs("after");


/*
		// Set extended mode or not.
		//RXB0SIDLbits.EXID=cm.extended;
		if (cm.extended==TRUE)
		{
			_asm 
				bsf RXB0SIDL, 3, 0
			_endasm 
		}
		else
		{
			_asm 
				bcf RXB0SIDL, 3, 0
			_endasm 
		}


		if (cm.extended==TRUE)
		{
			//Write extended ident.
			RXB0SIDH=(cm.ident[3]<3)+(cm.ident[2]>5);
			RXB0SIDL=(RXB0SIDL & 0b00011100)+((cm.ident[2] & 0b0001100)<3)+(cm.ident[2] & 0b00000011);
			RXB0EIDH=cm.ident[1];
			RXB0EIDL=cm.ident[0];
		}
		else
		{
			//Write standard ident.
			RXB0SIDH=(cm.ident[1]<5)+(cm.ident[0]>3);
			RXB0SIDL=(RXB0SIDL & 0b00011111)+(cm.ident[0]<5);
		}

		// Data length
		if (cm.data_length>8) RXB0DLC=8; else RXB0DLC=cm.data_length;

		// Data one bye one, for speed
		RXB0D0=cm.data[0];	RXB0D1=cm.data[1];
		RXB0D2=cm.data[2]; 	RXB0D3=cm.data[3];
		RXB0D4=cm.data[4]; 	RXB0D5=cm.data[5];
		RXB0D6=cm.data[6]; 	RXB0D7=cm.data[7];

		// mark as redy to transmit
		_asm 
			bsf RXB0CON, 3, 0
		_endasm 

		// restore mapping
		#ifdef CAN_ENCHANCED_MODE
			ECANCON = defECANCON;
		#else
			CANCON = defECANCON;
		#endif
*/
		
		

		
		return TRUE;

}


#endif //USE_CAN
