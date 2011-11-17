#include <inttypes.h>
#include <config.h>
#include <drivers/mcu/gpio.h>
#include <drivers/sensor/ds18s20/delay.h>

#ifndef LCD_CONTROL_E

#define LCD_DATA_DB0	EXP_D
#define LCD_DATA_DB1	EXP_E
#define LCD_DATA_DB2	EXP_F
#define LCD_DATA_DB3	EXP_G
#define LCD_DATA_DB4	EXP_H
#define LCD_DATA_DB5	EXP_I
#define LCD_DATA_DB6	EXP_J
#define LCD_DATA_DB7	EXP_K

#define LCD_CONTROL_RS	EXP_A
#define LCD_CONTROL_RW	EXP_B
#define LCD_CONTROL_E	EXP_C
#define LCD_CONTROL_CS1	EXP_N
#define LCD_CONTROL_CS2	EXP_O

#endif


#define GLCD_COLOR_BLACK 0
#define GLCD_COLOR_WHITE 1
#define GLCD_COLOR_SET	 0
#define GLCD_COLOR_CLEAR 1

//H�r har vi n�gon slags tillst�ndsvariabel
typedef struct struct_GrLcdStateType
{
    unsigned char lcdXAddr;
    unsigned char lcdYAddr;
    uint8_t lcdYpage;
    uint8_t color;
} GrLcdStateType;


//Alla funktioner
void ks0108Disable(void);

void ks0108Delay(void);

void ks0108Enable(void);

void ks0108WriteData(uint8_t data, uint8_t color);

void ks0108WriteDataTransparent(uint8_t inputdata, uint8_t color);

void ks0108SetControls(uint8_t RS, uint8_t RW);

void ks0108SetData(uint8_t Data);

uint8_t ks0108ReadData(void);

void ks0108SetXY(uint8_t x, uint8_t y);

uint8_t ks0108GetX(void);

uint8_t ks0108GetY(void);

void ks0108Init(void);

void ks0108Clear(void);

void ks0108SetColor(uint8_t color);

uint8_t ks0108GetColor(void);

//void glcdWriteChar(char c, uint8_t color);
//void glcdPutStrTransparent(char *data, uint8_t color);
//void glcdWriteCharTransparent(char c, uint8_t color);
//void glcdPutStr(char *data, uint8_t color);

//#define glcdDrawVertLine(x, y, length, color) {glcdFillRect(x, y, 0, length, color);}
//#define glcdDrawHoriLine(x, y, length, color) {glcdFillRect(x, y, length, 0, color);}
//#define glcdDrawCircle(xCenter, yCenter, radius, color) {glcdDrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);}
//void glcdDrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
//void glcdDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
//void glcdDrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color);
//void glcdFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
//void glcdInvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
//void glcdInvert(void);
//void glcdSetDot(uint8_t x, uint8_t y, uint8_t color);

