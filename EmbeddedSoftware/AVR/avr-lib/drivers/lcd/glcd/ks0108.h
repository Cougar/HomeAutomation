#include <inttypes.h>
#include <config.h>
#include <drivers/sensor/ds18s20/delay.h>

#ifndef LCD_CONTROL_PIN_E

#define LCD_DATA_PORT_DB0	PORTB
#define LCD_DATA_DDR_DB0	DDRB
#define LCD_DATA_IN_DB0		PINB
#define LCD_DATA_PORT_DB1	PORTD
#define LCD_DATA_DDR_DB1	DDRD
#define LCD_DATA_IN_DB1		PIND
#define LCD_DATA_PORT_DB2	PORTD
#define LCD_DATA_DDR_DB2	DDRD
#define LCD_DATA_IN_DB2		PIND
#define LCD_DATA_PORT_DB3	PORTD
#define LCD_DATA_DDR_DB3	DDRD
#define LCD_DATA_IN_DB3		PIND
#define LCD_DATA_PORT_DB4	PORTD
#define LCD_DATA_DDR_DB4	DDRD
#define LCD_DATA_IN_DB4		PIND
#define LCD_DATA_PORT_DB5	PORTD
#define LCD_DATA_DDR_DB5	DDRD
#define LCD_DATA_IN_DB5		PIND
#define LCD_DATA_PORT_DB6	PORTD
#define LCD_DATA_DDR_DB6	DDRD
#define LCD_DATA_IN_DB6		PIND
#define LCD_DATA_PORT_DB7	PORTD
#define LCD_DATA_DDR_DB7	DDRD
#define LCD_DATA_IN_DB7		PIND

#define LCD_DATA_PIN_DB0	0x00
#define LCD_DATA_PIN_DB1	0x07
#define LCD_DATA_PIN_DB2	0x06
#define LCD_DATA_PIN_DB3	0x05
#define LCD_DATA_PIN_DB4	0x04
#define LCD_DATA_PIN_DB5	0x02
#define LCD_DATA_PIN_DB6	0x01
#define LCD_DATA_PIN_DB7	0x00

#define LCD_CONTROL_PORT	PORTB
#define LCD_CONTROL_DDR		DDRB
#define LCD_CONTROL_PORT_CS	PORTC
#define LCD_CONTROL_DDR_CS	DDRC

#define LCD_CONTROL_PIN_RS	0x07
#define LCD_CONTROL_PIN_RW	0x02
#define LCD_CONTROL_PIN_E	0x01
#define LCD_CONTROL_PIN_CS1	0x05
#define LCD_CONTROL_PIN_CS2	0x04

#endif


#define GLCD_COLOR_BLACK 0
#define GLCD_COLOR_WHITE 1
#define GLCD_COLOR_SET	 1
#define GLCD_COLOR_CLEAR 0

//H�r har vi n�gon slags tillst�ndsvariabel
typedef struct struct_GrLcdStateType
{
    unsigned char lcdXAddr;
    unsigned char lcdYAddr;
    uint8_t lcdYpage;
    uint8_t color;
} GrLcdStateType;


//Alla funktioner
void Disable(void);

void Delay(void);

void Enable(void);

void glcdWriteData(uint8_t data, uint8_t color);

void glcdWriteDataTransparent(uint8_t inputdata, uint8_t color);

void glcdWriteChar(char c, uint8_t color);
void glcdWriteCharTransparent(char c, uint8_t color);

void glcdPutStr(char *data, uint8_t color);
void glcdPutStrTransparent(char *data, uint8_t color);

void SetControls(uint8_t RS, uint8_t RW);

void SetData(uint8_t Data);

void glcdSetXY(uint8_t x, uint8_t y);

void glcdPowerOn(void);

void glcdClear(void);

void glcdSetColor(uint8_t color);

uint8_t glcdGetColor(void);

#define glcdDrawVertLine(x, y, length, color) {glcdFillRect(x, y, 0, length, color);}
#define glcdDrawHoriLine(x, y, length, color) {glcdFillRect(x, y, length, 0, color);}
#define glcdDrawCircle(xCenter, yCenter, radius, color) {glcdDrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);}
void glcdDrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void glcdDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void glcdDrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color);
void glcdFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void glcdInvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void glcdInvert(void);
void glcdSetDot(uint8_t x, uint8_t y, uint8_t color);

