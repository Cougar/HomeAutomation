#include "act_lcd_eg2401.h"
#include "act_lcd_eg2401_tileset.h"

#define LCD_LP   (1 <<  9)
#define LCD_FR   (1 << 10)
#define LCD_YSCL (1 << 13)
#define LCD_DIN  (1 <<  0)
#define LCD_XSCL (1 <<  1)
#define LCD_XECL (1 <<  2)
#define LCD_PXLS (0x00F0)

#define MASK_ALL (LCD_XSCL | LCD_XECL | LCD_LP | LCD_YSCL | LCD_DIN | LCD_FR | LCD_PXLS)

#define DDR_LOW   DDRD
#define PORT_LOW  PORTD
#define DDR_HIGH  DDRC
#define PORT_HIGH PORTC

uint8_t lcd_framebuffer[256] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};


uint8_t act_lcd_eg2401_getPxls( uint8_t x, uint8_t y );
void act_lcd_eg2401_setPortL( uint16_t data );
void act_lcd_eg2401_setPort( uint16_t data );

void act_lcd_eg2401_Init(void)
{
    // Init contrastPWM
    TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
    TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11); /* Timer uses main system clock with 1/8 prescale */
    ICR1 = 4;
    OCR1A = 1; /* Servo at min position */
    DDRB |= (1<<DDB1); /* Enable pin as output */



    // Setup ports
    DDR_LOW  |= (MASK_ALL&0x00FF);
    DDR_HIGH |= (MASK_ALL&0xFF00)>>8;
}

void act_lcd_eg2401_Process(void)
{
    uint8_t x, y; // x counting segments of 4 pxl
    uint8_t pxldata;

    static uint16_t fr = 0;
    fr ^= LCD_FR;

    act_lcd_eg2401_setPort( fr );
    for( y=0; y<64; y++ ) {
        for( x=0; x<7; x++ ) {
            pxldata = act_lcd_eg2401_getPxls( x, y );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) );
            pxldata <<= 4;
            act_lcd_eg2401_setPortL( pxldata | LCD_XSCL );
            act_lcd_eg2401_setPortL( pxldata );
        }
        pxldata = act_lcd_eg2401_getPxls( x, y );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) );

        pxldata <<= 4;
        act_lcd_eg2401_setPortL( pxldata | LCD_XSCL | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata );

        for( x=8; x<15; x++ ) {
            pxldata = act_lcd_eg2401_getPxls( x, y );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) );
            pxldata <<= 4;
            act_lcd_eg2401_setPortL( pxldata | LCD_XSCL );
            act_lcd_eg2401_setPortL( pxldata );
        }
        pxldata = act_lcd_eg2401_getPxls( x, y );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) );

        pxldata <<= 4;
        act_lcd_eg2401_setPortL( pxldata | LCD_XSCL | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata );

        for( x=16; x<23; x++ ) {
            pxldata = act_lcd_eg2401_getPxls( x, y );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) );
            pxldata <<= 4;
            act_lcd_eg2401_setPortL( pxldata | LCD_XSCL );
            act_lcd_eg2401_setPortL( pxldata );
        }
        pxldata = act_lcd_eg2401_getPxls( x, y );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) );

        pxldata <<= 4;
        act_lcd_eg2401_setPortL( pxldata | LCD_XSCL | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata | LCD_XECL );
        act_lcd_eg2401_setPortL( pxldata );

        for( x=24; x<31; x++ ) {
            pxldata = act_lcd_eg2401_getPxls( x, y );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
            act_lcd_eg2401_setPortL( (pxldata&0xF0) );
            pxldata <<= 4;
            act_lcd_eg2401_setPortL( pxldata | LCD_XSCL );
            act_lcd_eg2401_setPortL( pxldata );
        }
        pxldata = act_lcd_eg2401_getPxls( x, y );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) | LCD_XSCL );
        act_lcd_eg2401_setPortL( (pxldata&0xF0) );

        pxldata <<= 4;
        if( y==0 ) {
            act_lcd_eg2401_setPort( fr | pxldata | LCD_XSCL | LCD_XECL          | LCD_YSCL | LCD_DIN );
            act_lcd_eg2401_setPort( fr | pxldata | LCD_XECL            | LCD_LP | LCD_YSCL | LCD_DIN );
            act_lcd_eg2401_setPort( fr | pxldata                       | LCD_LP            | LCD_DIN );
        } else {
            act_lcd_eg2401_setPort( fr | pxldata | LCD_XSCL | LCD_XECL          | LCD_YSCL );
            act_lcd_eg2401_setPort( fr | pxldata | LCD_XECL            | LCD_LP | LCD_YSCL );
            act_lcd_eg2401_setPort( fr | pxldata                       | LCD_LP );
        }
        act_lcd_eg2401_setPort( fr | pxldata );
    }
/*    act_lcd_eg2401_setPort( fr | pxldata | LCD_YSCL );
    act_lcd_eg2401_setPort( fr | pxldata );
FIXME: clock away from last row... */

}

void act_lcd_eg2401_setPortL( uint16_t data ) {
    PORT_LOW  = (  data&MASK_ALL     &0xFF) | (PORT_LOW & (~MASK_ALL)    &0xFF);
}

void act_lcd_eg2401_setPort( uint16_t data ) {
    PORT_LOW  = (  data&MASK_ALL     &0xFF) | (PORT_LOW & (~MASK_ALL)    &0xFF);
    PORT_HIGH = (((data&MASK_ALL)>>8)&0xFF) | (PORT_HIGH&((~MASK_ALL)>>8)&0xFF);
}

uint8_t act_lcd_eg2401_getPxls( uint8_t x, uint8_t y ) {
    uint16_t i = lcd_framebuffer[ x | ((y<<2)&0xe0) ];
    i <<= 3;
    i |= y&0x07;
    return pgm_read_byte( act_lcd_eg2401_tileset + i );
}

void act_lcd_eg2401_HandleMessage(StdCan_Msg_t *rxMsg) {}
void act_lcd_eg2401_List(uint8_t ModuleSequenceNumber) {}
