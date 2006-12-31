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

    }else if( view==TEMPSENS_VIEW ){

    }else if( view==RELAYS_VIEW ){

    }else if( view==DIMMERS_VIEW ){

    }else{
        /* Do nothing */
        return;
    }




    for( m=0; m<size; m++ ){
        lcd_gotoxy( dataPos[m][0],dataPos[m][1] );


        snprintf( buffer, MAX_LENGTH, data[ m ] );
        lcd_puts(buffer);
        //TODO fixa rätt enheter för olika views
    }
}
