#include <inttypes.h>
#include <config.h>
#include <drivers/mcu/gpio.h>
#include <drivers/sensor/ds18s20/delay.h>

#define GLCD_COLOR_BLACK 0
#define GLCD_COLOR_WHITE 1
#define GLCD_COLOR_SET	 0
#define GLCD_COLOR_CLEAR 1

#define dotmatrixROW_IO1 EXP_P
#define dotmatrixROW_IO2 EXP_B
#define dotmatrixROW_IO3 EXP_C
#define dotmatrixROW_IO4 EXP_D
#define dotmatrixROW_IO5 EXP_E
#define dotmatrixROW_IO6 EXP_F
#define dotmatrixROW_IO7 EXP_A
#define dotmatrixROW_IO8 EXP_O

#define dotmatrixSIZEX 32
#define dotmatrixSIZEY 8

#define dotmatrixLATCHCLOCK_IO EXP_I

#define dotmatrixINITIAL_ROW 0x0

#define USART_SPI_CS_DDR	DDRC
#define USART_SPI_CS		PC0
#define USART_SPI_XCK_DDR	DDRD
#define USART_SPI_XCK		PD4
#define waitspi() while(!(UCSR0A&(1<<RXC0)))

/* State variable */
typedef struct struct_GrLcdStateType
{
    unsigned char lcdXAddr;
    unsigned char lcdYAddr;
    uint8_t lcdYpage;
    uint8_t color;
} GrLcdStateType;


/* Internal functions */

void dotmatrixSPIWrite(uint8_t data);

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


