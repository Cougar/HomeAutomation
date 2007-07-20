/*********************************************************************
*
*                  Helper Functions for Microchip TCP/IP Stack
*
*********************************************************************
* FileName:			Helpers.C
* Dependencies:		Compiler.h
*					Helpers.h
* Processor:		PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
* Complier:			Microchip C18 v3.02 or higher
*					Microchip C30 v2.01 or higher
* Company:			Microchip Technology, Inc.
*
* Software License Agreement
*
* This software is owned by Microchip Technology Inc. ("Microchip") 
* and is supplied to you for use exclusively as described in the 
* associated software agreement.  This software is protected by 
* software and other intellectual property laws.  Any use in 
* violation of the software license may subject the user to criminal 
* sanctions as well as civil liability.  Copyright 2006 Microchip
* Technology Inc.  All rights reserved.
*
* This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
* WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
* TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
* INFRINGEMENT.  Microchip shall in no event be liable for special, 
* incidental, or consequential damages.
*
*
* Author               Date    Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Nilesh Rajbharti     5/17/01 Original        (Rev 1.0)
* Nilesh Rajbharti     2/9/02  Cleanup
* Nilesh Rajbharti     6/25/02 Rewritten CalcIPChecksum() to avoid
*                              multi-byte shift operation.
* Howard Schlunder		2/9/05 Added hexatob(), btohexa_high(), and
*							   btohexa_low()
********************************************************************/

#include "..\Include\Compiler.h"
#include "..\Include\Helpers.h"
#include "..\Include\MAC.h"



#if defined(__C30__) || defined(HI_TECH_C)
/*********************************************************************
* Function:		void itoa(unsigned int Value, char *Buffer)
*
* PreCondition:	None
*
* Input:		Value: Unsigned integer to be converted
*				Buffer: Pointer to a location to write the string
*
* Output:		*Buffer: Receives the resulting string
*
* Side Effects:	None
*
* Overview:		The function converts an unsigned integer (16 bits) 
*				into a null terminated decimal string.
*
* Note:			None
********************************************************************/
void itoa(unsigned int Value, char *Buffer)
{
	unsigned char i;
	unsigned int Digit;
	unsigned int Divisor;
	enum {FALSE = 0, TRUE} Printed = FALSE;

	if(Value)
	{
		for(i = 0, Divisor = 10000; i < 5; i++)
		{
			Digit = Value/Divisor;
			if(Digit || Printed)
			{
				*Buffer++ = '0' + Digit;
				Value -= Digit*Divisor;
				Printed = TRUE;
			}
			Divisor /= 10;
		}
	}
	else
	{
		*Buffer++ = '0';
	}

	*Buffer = '\0';
}			    
#endif


/*********************************************************************
* Function:        BYTE hexatob(WORD_VAL AsciiChars)
*
* PreCondition:    None
*
* Input:           Two ascii bytes; each ranged '0'-'9', 'A'-'F', or
*						'a'-'f'
*
* Output:          The resulting packed byte: 0x00-0xFF
*
* Side Effects:    None
*
* Overview:        None
*
* Note:			None
********************************************************************/
BYTE hexatob(WORD_VAL AsciiChars)
{
	// Convert lowercase to uppercase
	if(AsciiChars.v[1] > 'F')
		AsciiChars.v[1] -= 'a'-'A';
	if(AsciiChars.v[0] > 'F')
		AsciiChars.v[0] -= 'a'-'A';

	// Convert 0-9, A-F to 0x0-0xF
	if(AsciiChars.v[1] > '9')
		AsciiChars.v[1] -= 'A' - 10;
	else
		AsciiChars.v[1] -= '0';

	if(AsciiChars.v[0] > '9')
		AsciiChars.v[0] -= 'A' - 10;
	else
		AsciiChars.v[0] -= '0';

	// Concatenate
	return (AsciiChars.v[1]<<4) |  AsciiChars.v[0];
}

/*********************************************************************
* Function:        BYTE btohexa_high(BYTE b)
*
* PreCondition:    None
*
* Input:           One byte ranged 0x00-0xFF
*
* Output:          An ascii byte (always uppercase) between '0'-'9' 
*					or 'A'-'F' that corresponds to the upper 4 bits of
*					the input byte.
*					ex: b = 0xAE, btohexa_high() returns 'A'
*
* Side Effects:    None
*
* Overview:        None
*
* Note:			None
********************************************************************/
BYTE btohexa_high(BYTE b)
{
	b >>= 4;
	return (b>0x9) ? b+'A'-10:b+'0';
}

/*********************************************************************
* Function:        BYTE btohexa_low(BYTE b)
*
* PreCondition:    None
*
* Input:           One byte ranged 0x00-0xFF
*
* Output:          An ascii byte (always uppercase) between '0'-'9' 
*					or 'A'-'F' that corresponds to the lower 4 bits of
*					the input byte.
*					ex: b = 0xAE, btohexa_low() returns 'E'
*
* Side Effects:    None
*
* Overview:        None
*
* Note:			None
********************************************************************/
BYTE btohexa_low(BYTE b)
{
	b &= 0x0F;
	return (b>9) ? b+'A'-10:b+'0';
}


WORD swaps(WORD v)
{
	WORD_VAL t;
	BYTE b;

	t.Val   = v;
	b       = t.v[1];
	t.v[1]  = t.v[0];
	t.v[0]  = b;

	return t.Val;
}


DWORD swapl(DWORD v)
{
	BYTE b;
	DWORD myV;
	DWORD_VAL *myP;

	myV     = v;
	myP     = (DWORD_VAL*)&myV;

	b       = myP->v[3];
	myP->v[3] = myP->v[0];
	myP->v[0] = b;

	b       = myP->v[2];
	myP->v[2] = myP->v[1];
	myP->v[1] = b;

	return myV;

}


WORD CalcIPChecksum(BYTE* buffer, WORD count)
{
	WORD i;
	WORD *val;

	union
	{
		DWORD Val;
		struct
		{
			WORD_VAL LSB;
			WORD_VAL MSB;
		} words;
	} tempSum, sum;

	sum.Val = 0;

	i = count >> 1;
	val = (WORD *)buffer;

	while( i-- )
		sum.Val += *val++;

	if ( count & 1 )
		sum.Val += *(BYTE *)val;

	tempSum.Val = sum.Val;

	while( (i = tempSum.words.MSB.Val) != 0u )
	{
		sum.words.MSB.Val = 0;
		sum.Val = (DWORD)sum.words.LSB.Val + (DWORD)i;
		tempSum.Val = sum.Val;
	}

	return (~sum.words.LSB.Val);
}


/*********************************************************************
* Function:        WORD CalcIPBufferChecksum(WORD len)
*
* PreCondition:    TCPInit() is already called     AND
*                  MAC buffer pointer set to starting of buffer
*
* Input:           len     - Total number of bytes to calculate
*                          checksum for.
*
* Output:          16-bit checksum as defined by rfc 793.
*
* Side Effects:    None
*
* Overview:        This function performs checksum calculation in
*                  MAC buffer itself.
*
* Note:            None
********************************************************************/
#if defined(NON_MCHP_MAC)
WORD CalcIPBufferChecksum(WORD len)
{
	BOOL lbMSB;
	WORD_VAL checkSum;
	BYTE Checkbyte;

	lbMSB = TRUE;
	checkSum.Val = 0;

	while( len-- )
	{
		Checkbyte = MACGet();

		if ( !lbMSB )
		{
			if ( (checkSum.v[0] = Checkbyte+checkSum.v[0]) < Checkbyte)
			{
				if ( ++checkSum.v[1] == 0 )
					checkSum.v[0]++;
			}
		}
		else
		{
			if ( (checkSum.v[1] = Checkbyte+checkSum.v[1]) < Checkbyte)
			{
				if ( ++checkSum.v[0] == 0 )
					checkSum.v[1]++;
			}
		}

		lbMSB = !lbMSB;
	}

	checkSum.v[1] = ~checkSum.v[1];
	checkSum.v[0] = ~checkSum.v[0];
	return checkSum.Val;
}
#endif

/*********************************************************************
* Function:		char *strupr(char *s)
*
* PreCondition:	None
*
* Input:		s: Pointer to a null terminated string to convert.
*
* Output:		char* return: Pointer to the initial string
*				*s: String is updated in to have all upper case 
*					characters
*
* Side Effects:	None
*
* Overview:		The function sequentially converts all lower case 
*				characters in the input s string to upper case 
*				characters.  Non a-z characters are skipped.
*
* Note:			None
********************************************************************/
#if defined(__C30__) || defined(HI_TECH_C)
char *strupr(char *s)
{
	char c;
	char *t;

	t = s;
	while( (c = *t) )
	{
		if(c >= 'a' && c <= 'z')
		{
			*t -= ('a' - 'A');
		}
		t++;
	}
	return s;
}
#endif


void dwordToHex(DWORD c,char *str)
{
	BYTE b;
	str[0]='0';
	str[1]='x';

	b = c>>24;
	str[2]=btohexa_high(b);
	str[3]=btohexa_low(b);

	b = c>>16;
	str[4]=btohexa_high(b);
	str[5]=btohexa_low(b);

	b = c>>8;
	str[6]=btohexa_high(b);
	str[7]=btohexa_low(b);

	b = c;
	str[8]=btohexa_high(b);
	str[9]=btohexa_low(b);
	
	str[10]='\0';
}

void timestampToString(DWORD t, char *str)
{
        DWORD days=t/86400;
        DWORD day_sec=t%86400+7200;

        int year = days/365+1970;

        int dy = days%365; //left in year
        int month=0;
        int day=0;


        int daysOfMonth = 31;
        int i;

		int hh;
		int mm;
		int ss;

        int skottar = (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));

        dy-=9; // corr!!

        for(i = 1; i <= 12; ++i)
        {
                if(dy <= daysOfMonth) { day=dy; break; }
                else { dy -= daysOfMonth; }

                if(daysOfMonth==31) daysOfMonth=30; else daysOfMonth=31;
                if (i==1) daysOfMonth=(skottar?29:28);
        }

        month=i;

        hh = day_sec/3600;
        mm = (day_sec%3600)/60;
        ss = (day_sec%3600)%60;


		str[0]=(year/1000)+48;
		str[1]=(year-(str[0]-48)*1000)/100+48;
		str[2]=(year-(str[0]-48)*1000-(str[1]-48)*100)/10+48;
		str[3]=(year-(str[0]-48)*1000-(str[1]-48)*100-(str[2]-48)*10)+48;
		str[4]='-';
		str[5]=month/10+48;
		str[6]=(month-(str[5]-48)*10)+48;
		str[7]='-';
		str[8]=day/10+48;
		str[9]=(day-(str[8]-48)*10)+48;
		str[10]=' ';
		str[11]=hh/10+48;
		str[12]=(hh-(str[11]-48)*10)+48;	
		str[13]=':';
		str[14]=mm/10+48;
		str[15]=(mm-(str[14]-48)*10)+48;
		str[16]=':';
		str[17]=ss/10+48;
		str[18]=(ss-(str[17]-48)*10)+48;
		str[19]='\0';
}
