#include <inttypes.h>


#define LCD_DATA_PORT_DB0	PORTC
#define LCD_DATA_PORT_DB1	PORTC
#define LCD_DATA_PORT_DB2	PORTC
#define LCD_DATA_PORT_DB3	PORTC
#define LCD_DATA_PORT_DB4	PORTC
#define LCD_DATA_PORT_DB5	PORTC
#define LCD_DATA_PORT_DB6	PORTB
#define LCD_DATA_PORT_DB7	PORTD

#define LCD_DATA_PIN_DB0	0x00
#define LCD_DATA_PIN_DB1	0x01
#define LCD_DATA_PIN_DB2	0x02
#define LCD_DATA_PIN_DB3	0x03
#define LCD_DATA_PIN_DB4	0x04
#define LCD_DATA_PIN_DB5	0x05
#define LCD_DATA_PIN_DB6	0x00
#define LCD_DATA_PIN_DB7	0x00

#define LCD_CONTROL_PORT	PORTD

#define LCD_CONTROL_PIN_RS	0x02
#define LCD_CONTROL_PIN_RW	0x01
#define LCD_CONTROL_PIN_E	0x04
#define LCD_CONTROL_PIN_CS1	0x05
#define LCD_CONTROL_PIN_CS2	0x06


//Här har vi någon slags tillståndsvariabel
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

void glcdWriteChar(unsigned char c);

void glcdPutStr(unsigned char *data);

void SetControls(uint8_t RS, uint8_t RW);

void SetData(uint8_t Data);

void glcdSetXY(uint8_t x, uint8_t y);

void glcdPowerOn(void);
