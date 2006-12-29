#include <stdio.h>
#include <clcd20x4.h>

/*
 * Prints "view" to LCD
 */

void printLCDview(uint8_t view){
    uint8_t line;
    uint8_t buffer[ MAX_LENGTH ];
    lcd_clrscs();

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

void printLCDviewData(){

}
