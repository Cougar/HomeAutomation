/*********************************************************************
 *
 *                  LCD routines (HD44780)
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#include <lcd.h>

#ifdef USE_LCD

static void lcdWrite(BYTE c);

#define	LCD_STROBE	((LCD_EN_IO = 1),(LCD_EN_IO = 0))

/*
	OBS OBS OBS OBS!!

	DELAYS ARE FOR 40Mhz (10Mhz PLL4)

	Definitions needed:

	LCD_RS_TRIS and LCD_RS_IO
	LCD_EN_TRIS and LCD_EN_IO
	LCD_DATA_4_TRIS and LCD_DATA_4_IO
	LCD_DATA_5_TRIS and LCD_DATA_5_IO
	LCD_DATA_6_TRIS and LCD_DATA_6_IO
	LCD_DATA_7_TRIS and LCD_DATA_7_IO
	
	

*/

/*
*	Function: lcdWrite
*
*	Input:	Byte to write.
*	Output: none
*	Pre-conditions: none
*	Affects: Write to the dislay
*	Depends: none.
*/
void lcdWrite(BYTE c)
{    
    LCD_DATA_4_IO=((c)&0b10000)>>4;
	LCD_DATA_5_IO=((c)&0b100000)>>5;
	LCD_DATA_6_IO=((c)&0b1000000)>>6;
	LCD_DATA_7_IO=((c)&0b10000000)>>7;
	LCD_STROBE;
	LCD_DATA_4_IO=((c)&0b1);
	LCD_DATA_5_IO=((c)&0b10)>>1;
	LCD_DATA_6_IO=((c)&0b100)>>2;
	LCD_DATA_7_IO=((c)&0b1000)>>3;
	LCD_STROBE;
	Delay100TCYx(8);	// 80uS
}


/*
*	Function: lcdClear
*
*	Input:	none.
*	Output: none
*	Pre-conditions: none
*	Affects: Clear the display
*	Depends: none.
*/
void lcdClear(void)
{
	LCD_RS_IO = 0;
	lcdWrite(0x1);   // Clear the Display
	Delay10KTCYx(2);		// 2ms
	lcdWrite(0x02);  // Home the display
	Delay10KTCYx(2);		// 2ms
}

/*
*	Function: lcdPuts
*
*	Input:	string to write.
*	Output: none
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void lcdPuts(char * s)
{
	LCD_RS_IO = 1;	// write characters
	while(*s) lcdWrite(*s++);
}

/*
*	Function: lcdPutrs
*
*	Input:	string to write.
*	Output: none
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void lcdPutrs(const rom char *s)
{
	LCD_RS_IO = 1;	// write characters
	while(*s) lcdWrite(*s++);
}

/*
*	Function: lcdPutc
*
*	Input:	char to write.
*	Output: none
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void lcdPutc(char c)
{
	LCD_RS_IO=1;    // write character
	lcdWrite(c);
}



/*
*	Function: lcdGoto
*
*	Input:	Position to goto
*	Output: none
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void lcdGoto(BYTE pos)
{
	LCD_RS_IO = 0;
	lcdWrite(0x80+pos);           // sets the DDRAM Address
}

	
/*
*	Function: lcdInit
*
*	Input:	none.
*	Output: none
*	Pre-conditions: none
*	Affects: none.
*	Depends: Setup to use 4 bit mode
*/
void lcdInit(void)
{
	// Setup IO (all output)
	LCD_RS_TRIS = 0;
	LCD_EN_TRIS = 0;
	LCD_DATA_4_TRIS = 0;
	LCD_DATA_5_TRIS = 0;
	LCD_DATA_6_TRIS = 0;
	LCD_DATA_7_TRIS = 0;

	LCD_RS_IO = 0;	// write control bytes
	Delay10KTCYx(15);	// power on delay, 15ms
	lcdWrite(0x3);	// attention!
	Delay10KTCYx(5);		// 5ms
	LCD_STROBE;
	Delay100TCYx(10);	// 100us
	LCD_STROBE;
	Delay10KTCYx(5);		// 5ms
	lcdWrite(0x2);	// set 4 bit mode
	Delay100TCYx(4);	// 40 us
	lcdWrite(0x28);	// 4 bit mode, 1/16 duty, 5x8 font
	lcdWrite(0x08);	// display off
	lcdWrite(0x0C);	// display on, unblink curson off
	lcdWrite(0x06);	// entry mode
}






#endif

