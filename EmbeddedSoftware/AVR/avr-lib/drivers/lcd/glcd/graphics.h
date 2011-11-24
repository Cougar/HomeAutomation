#include <inttypes.h>
#include <config.h>

#define GLCD_COLOR_BLACK 0
#define GLCD_COLOR_WHITE 1
#define GLCD_COLOR_SET	 0
#define GLCD_COLOR_CLEAR 1

#define KS0108 0
#define DOTMATRIX 1

//Alla funktioner
void glcdSetColor(uint8_t color);

uint8_t glcdGetColor(void);

void glcdWriteData(uint8_t data, uint8_t color);

void glcdWriteDataTransparent(uint8_t inputdata, uint8_t color);

void glcdClear(void);

void glcdSetXY(uint8_t x, uint8_t y);

uint8_t glcdGetX(void);

uint8_t glcdGetY(void);

void glcdInit(void);

void glcdRefresh(void);


void glcdPutStr(char *data, uint8_t color);
void glcdWriteCharTransparent(char c, uint8_t color);
void glcdPutStrTransparent(char *data, uint8_t color);
void glcdWriteChar(char c, uint8_t color);

#define glcdDrawVertLine(x, y, length, color) {glcdFillRect(x, y, 0, length, color);}
#define glcdDrawHoriLine(x, y, length, color) {glcdFillRect(x, y, length, 0, color);}
#define glcdDrawCircle(xCenter, yCenter, radius, color) {glcdDrawRoundRect(xCenter-radius, yCenter-radius, 2*radius, 2*radius, radius, color);}

void glcdDrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void glcdDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void glcdFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);
void glcdInvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void glcdInvert(void);
void glcdSetDot(uint8_t x, uint8_t y, uint8_t color);
void glcdDrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color);

