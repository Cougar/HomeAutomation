/*
 * Library for printing views on a 20x4 characters LCD
 *
 * Author: Erik Larsson
 * Date: 2006-12-30
 *
 */

/*-----------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <clcd20x4.h>
#include <lcd_HD44780.h>
#include <avr/interrupt.h>

/*-----------------------------------------------------
 * Functions
 * --------------------------------------------------*/

/*
 * Prints "view" to LCD
 */
void printLCDview(uint8_t view){
    uint8_t line;
    uint8_t buffer[ MAX_LENGTH ];
    lcd_clrscr();

    view = view*5;

    /* Print first 3 lines */
    for(line = 0; line < LINES-1; line++ ){
        lcd_gotoxy(0,line);
        lcd_puts( viewStrings[ view+line ] );
    }

    /* Print bottom line */
    lcd_gotoxy(0,3);
    snprintf( buffer, MAX_LENGTH, "<<%s", viewStrings[ view+3 ] );
    lcd_puts( buffer );

    lcd_gotoxy(9,3);
    lcd_puts( EDIT_MODE_SYMBOL );

    lcd_gotoxy(18,3);
    lcd_puts( ">>" );

    lcd_gotoxy(12,3);
    lcd_puts( viewStrings[ view+4 ] );

}

/*
 * Prints data to "view"
 */
void printLCDviewData(uint8_t view, uint8_t *data, uint8_t size){
    int m;
    char buffer[ MAX_LENGTH ];

    if( view==MAIN_VIEW ){
        /* No data */
    }else if( view==TEMPSENS_VIEW ){
        for(m=0;m<size/2;m++){
            if((int8_t)data[m*2]<0){
                /* Subsero? */
                snprintf(buffer, MAX_DATA_LENGTH, "-%u.%u   ", -(int8_t)data[m*2], (int8_t)(data[m*2+1]>>4)/10 );
            }else{
                snprintf(buffer, MAX_DATA_LENGTH, "%u.%u    ", data[m*2], (int8_t)(data[m*2+1]>>4)/10 );
            }
			cli();
            lcd_gotoxy(dataPos[m][0],dataPos[m][1]);
            lcd_puts(buffer);
			sei();
        }
    }else if( view==RELAYS_VIEW ){
        for(m=0;m<size;m++){
            /* Relay status, on or off */
            snprintf(buffer, MAX_DATA_LENGTH, (data[m]==0x01)?"ON   ":"OFF   ");
            lcd_gotoxy(dataPos[m][0],dataPos[m][1]);
            lcd_puts(buffer);
        }
    }else if( view==DIMMERS_VIEW ){
        // FIXME
    }else if( view==SERVO_VIEW ){
        for(m=0;m<size;m++){
            if((int8_t)data[m]<0){
                snprintf(buffer, MAX_DATA_LENGTH, "-%u  ", -(int8_t)data[m]);
            }else{
                snprintf(buffer, MAX_DATA_LENGTH, "%u   ", data[m]);
            }
            lcd_gotoxy(dataPos[m][0], dataPos[m][1]);
            lcd_puts(buffer);
        }
    }else{
        /* Do nothing */
        return;
    }
}

void printLCDedit(uint8_t view){

}
