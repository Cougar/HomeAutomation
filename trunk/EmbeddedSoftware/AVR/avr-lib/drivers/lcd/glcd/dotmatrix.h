#include <inttypes.h>
#include <config.h>
#include <drivers/mcu/gpio.h>
#include <drivers/sensor/ds18s20/delay.h>

#define GLCD_COLOR_BLACK 0
#define GLCD_COLOR_WHITE 1
#define GLCD_COLOR_SET	 0
#define GLCD_COLOR_CLEAR 1

/* State variable */
typedef struct struct_GrLcdStateType
{
    unsigned char lcdXAddr;
    unsigned char lcdYAddr;
    uint8_t lcdYpage;
    uint8_t color;
} GrLcdStateType;


/* Internal functions */

void dotmatrixDisable(void);

void dotmatrixDelay(void);

void dotmatrixEnable(void);

void dotmatrixSetControls(uint8_t RS, uint8_t RW);

void dotmatrixSetData(uint8_t Data);

/* Public functions */

void dotmatrixWriteData(uint8_t data, uint8_t color);

void dotmatrixWriteDataTransparent(uint8_t inputdata, uint8_t color);

uint8_t dotmatrixReadData(void);

void dotmatrixSetXY(uint8_t x, uint8_t y);

uint8_t dotmatrixGetX(void);

uint8_t dotmatrixGetY(void);

void dotmatrixRefresh(void);

void dotmatrixInit(void);

void dotmatrixClear(void);

void dotmatrixSetColor(uint8_t color);

uint8_t dotmatrixGetColor(void);


