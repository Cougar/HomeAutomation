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
	CANCON=0b10000000;
	while(!CANSTATbits.OPMODE2);
	
	ECANCON=0;

	BRGCON1=0b01001001;
	BRGCON2=0b10001001;
	BRGCON3=0b00000001;

	// Set Mask 0 to not mask any filter bit
	RXM0SIDH = 0xFF;
	RXM0SIDL = 0xE3;
	RXM0EIDH = 0xFF;
	RXM0EIDL = 0xFF;

	MSEL0=MSEL1=MSEL2=MSEL3=0;


	CIOCON=0b00100000;


	PIE3=0b00000011;
	IPR3=0b00000011;
	
	CANCON=0b01000000;
	while(CANSTATbits.OPMODE2);
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
	if (PIR3bits.RXB0IF==1 && PIE3bits.RXB0IE==1)
	{
		CAN_MESSAGE cm;

		uartPutrs("IF1");
		
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

		RXB0CONbits.RXFUL=0;
		PIR3bits.RXB0IF=0;
	}	 
	if (PIR3bits.RXB1IF==1 && PIE3bits.RXB1IE==1)
	{
		CAN_MESSAGE cm;

		uartPutrs("IF2");

		// Set extended mode or not.
		cm.extended=RXB1SIDLbits.EXID;

		// Clear
		cm.ident[0]=cm.ident[1]=cm.ident[2]=cm.ident[3]=0;


		if (cm.extended==TRUE)
		{
			//Read extended ident.
			cm.ident[0]=RXB1EIDL;
			cm.ident[1]=RXB1EIDH;
			cm.ident[2]=(RXB1SIDL & 0b00000011)+((RXB1SIDL & 0b11100000)>3)+((RXB1SIDH & 0b00000111)<5);
			cm.ident[3]=RXB1SIDH>3;
		}
		else
		{
			//Read standard ident.
			cm.ident[0]=((RXB1SIDL & 0b11100000)>5)+(RXB1SIDH>2);
			cm.ident[1]=RXB1SIDH>5;
		}

		// Data length
		cm.data_length=RXB1DLC;

		// Data one bye one, for speed
		cm.data[0]=RXB1D0;	cm.data[1]=RXB1D1;
		cm.data[2]=RXB1D2; 	cm.data[3]=RXB1D3;
		cm.data[4]=RXB1D4; 	cm.data[5]=RXB1D5;
		cm.data[6]=RXB1D6; 	cm.data[7]=RXB1D7;


		//Call main parser
		canParse(cm);

		RXB1CONbits.RXFUL=0;
		PIR3bits.RXB1IF=0;
	}

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
	
	// Find the first empty transmitter. 
	if ( TXB0CONbits.TXREQ == 0 ) 
	{ 
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

		while(TXB0CONbits.TXREQ);


	} 
	// None of the transmit buffers were empty. 
	else {uartPutrs("No free tx."); return FALSE;} 
	

	

		return TRUE;

}


#endif //USE_CAN
