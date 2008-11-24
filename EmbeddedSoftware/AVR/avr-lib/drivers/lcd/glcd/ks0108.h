#include <inttypes.h>
#include <config.h>


#ifndef LCD_CONTROL_PIN_E

#define LCD_DATA_PORT_DB0	PORTB
#define LCD_DATA_DDR_DB0	DDRB
#define LCD_DATA_PORT_DB1	PORTD
#define LCD_DATA_DDR_DB1	DDRD
#define LCD_DATA_PORT_DB2	PORTD
#define LCD_DATA_DDR_DB2	DDRD
#define LCD_DATA_PORT_DB3	PORTD
#define LCD_DATA_DDR_DB3	DDRD
#define LCD_DATA_PORT_DB4	PORTD
#define LCD_DATA_DDR_DB4	DDRD
#define LCD_DATA_PORT_DB5	PORTD
#define LCD_DATA_DDR_DB5	DDRD
#define LCD_DATA_PORT_DB6	PORTD
#define LCD_DATA_DDR_DB6	DDRD
#define LCD_DATA_PORT_DB7	PORTD
#define LCD_DATA_DDR_DB7	DDRD


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

//H�r har vi n�gon slags tillst�ndsvariabel
typedef struct struct_GrLcdStateType
{
    unsigned char lcdXAddr;
    unsigned char lcdYAddr;
} GrLcdStateType;


//Alla funktioner
void Disable(void);

void Delay(void);

void Enable(void);

void glcdWriteData(uint8_t data);

//extern void glcdWriteChar(char c);

//extern void glcdPutStr(const char *s);

void glcdWriteChar(char c);

void glcdPutStr(char *data);

void SetControls(uint8_t RS, uint8_t RW);

void SetData(uint8_t Data);

void glcdSetXY(uint8_t x, uint8_t y);

void glcdPowerOn(void);

void glcdClear(void);
