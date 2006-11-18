/*********************************************************************
 *
 *                  IR reciver module
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#include <IRec.h>

#include <CAN.h>
#include <funcdefs.h>

#ifdef USE_IREC

static BYTE toggle = 0;
static BYTE addr = 0;
static BYTE data = 0;
static BYTE bitIndex = 0;
static IR_TYPE type = IR_NONE;

/*
*	Function: irecInit
*
*	Input:	none
*	Output: none
*	Pre-conditions: none
*	Affects: none
*	Depends: TIMER 3 is free
*/
void irecInit()
{

	// Setup timer 3 to be source for CCP1
	T3CON = 0b11111001;
	
	// configure timer 3 interrupt (for overflow)
	PIR2bits.TMR3IF = 0;
	PIE2bits.TMR3IE = 1;
	IPR2bits.TMR3IP = 1;

	// Setup CCP1 to use capture on falling edge
	CCP1CON=0x04;

	// Enable CCP1 interrupt and set priority to high.
	PIR1bits.CCP1IF=0;
	PIE1bits.CCP1IE=1;
	IPR1bits.CCP1IP=1;

}

/*
*	Function: irecISR
*
*	Input:	none
*	Output: none
*	Pre-conditions: a call to irecInit()
*	Affects: ...
*	Depends: ...
*/
void irecISR()
{
	static WORD timediff = 0;
	static BOOL started = FALSE;
	static BYTE lastbit = 1;
	static BOOL second = FALSE;

	// If IR bit
	if(PIR1bits.CCP1IF && PIE1bits.CCP1IE)
	{
		// Read timediff
		timediff  = TMR3L;
		timediff += (((WORD)TMR3H)<<8);

		// Clear timer.
		TMR3H = 0;
		TMR3L = 0;

		// Wait on inverse edge
		CCP1CONbits.CCP1M0=~CCP1CONbits.CCP1M0;

		// Disable interrupt
		PIR1bits.CCP1IF=0;

		// If found RC5 och RC5X startbit...
		if (started==TRUE && (type==IR_RC5 || type==IR_RC5X))
		{
			// Parse message
			
			if ((t2-250)<timediff && timediff<(t2+250) && second==FALSE)
			{
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=5) addr+=(lastbit<<(5-bitIndex));
				if (bitIndex>=6 && bitIndex<=11) data+=(lastbit<<(11-bitIndex));
				bitIndex++;
	
				second = TRUE;
				return;
			}
	
			if ((t4-250)<timediff && timediff<(t4+250) && second==FALSE)
			{
				lastbit=(lastbit?0:1);
	
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=5) addr+=(lastbit<<(5-bitIndex));
				if (bitIndex>=6 && bitIndex<=11) data+=(lastbit<<(11-bitIndex));
				bitIndex++;
	
				second = TRUE;
				return;
			}
	
			if ((t2-250)<timediff && timediff<(t2+250) && second==TRUE)
			{
				second = FALSE;
				return;
			}
	
			if ((t4-250)<timediff && timediff<(t4+250) && second==TRUE)
			{
				lastbit=(lastbit?0:1);
	
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=5) addr+=(lastbit<<(5-bitIndex));
				if (bitIndex>=6 && bitIndex<=11) data+=(lastbit<<(11-bitIndex));
				bitIndex++;
	
				return;
			}

		} // if started and type == RC5 or RC5X


		// If RC6...
		if (started==TRUE && type==IR_RC6)
		{

			if ((t1-250)<timediff && timediff<(t1+250) && second==FALSE)
			{
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=8) addr+=(lastbit<<(8-bitIndex));
				if (bitIndex>=9 && bitIndex<=16) data+=(lastbit<<(16-bitIndex));
				bitIndex++;
	
				second = TRUE;
				return;
			}
	
			if ((t2-250)<timediff && timediff<(t2+250) && second==FALSE)
			{
				lastbit=(lastbit?0:1);
	
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=8) addr+=(lastbit<<(8-bitIndex));
				if (bitIndex>=9 && bitIndex<=16) data+=(lastbit<<(16-bitIndex));
				bitIndex++;
	
				second = TRUE;
				return;
			}
	
			if ((t1-250)<timediff && timediff<(t1+250) && second==TRUE)
			{
				second = FALSE;
				return;
			}
	
			if ((t2-250)<timediff && timediff<(t2+250) && second==TRUE)
			{
				lastbit=(lastbit?0:1);
	
				if (bitIndex==0) toggle=lastbit;
				if (bitIndex>=1 && bitIndex<=8) addr+=(lastbit<<(8-bitIndex));
				if (bitIndex>=9 && bitIndex<=16) data+=(lastbit<<(16-bitIndex));
				bitIndex++;
	
				return;
			}

		} // if started and type == RC6



		bitIndex++;


		if (started==FALSE && type==IR_NONE && bitIndex==2 && (t2-250)<timediff && timediff<(t2+250))
		{
			type=IR_RC5;
		}
		else if (started==FALSE && type==IR_NONE && bitIndex==2 && (t4-250)<timediff && timediff<(t4+250))
		{
			started = TRUE;
			bitIndex = 0;
			lastbit = 1;
			type=IR_RC5X;
		}
		else if (started==FALSE && type==IR_NONE && bitIndex==2 && (t6-250)<timediff && timediff<(t6+250))
		{
			type=IR_RC6;
		}
		else if (started==FALSE && type==IR_RC6 && bitIndex==9)
		{	
			started = TRUE;
			bitIndex = 0;
			lastbit = 0;
			type=IR_RC6;
			second = TRUE;
		}
		else if (started==FALSE && type==IR_RC5 && bitIndex==3 && (t2-250)<timediff && timediff<(t2+250))
		{
			started = TRUE;
			bitIndex = 0;
			lastbit = 1;
			type=IR_RC5;
		}
		else if(bitIndex>2 && type!=IR_RC6)
		{
			type=IR_NONE;
		}
		else if(bitIndex>9)
		{
			type=IR_NONE;
		}
		
	}




	// If timer 3 overflow
	if(PIR2bits.TMR3IF && PIE2bits.TMR3IE)
	{	

		PIR2bits.TMR3IF=0;

		if (type!=IR_NONE)
		{
			irecParse(type,toggle,addr,data);
		}

		started  = FALSE;
		bitIndex = 0;
		addr = 0;
		toggle = 0;
		data = 0;
		type = IR_NONE;

		// wait in falling edge.
		CCP1CONbits.CCP1M0=0;
		
	}

}



#endif
