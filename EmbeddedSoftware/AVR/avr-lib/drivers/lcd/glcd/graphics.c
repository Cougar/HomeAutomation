#include "graphics.h"
#include "font.h"
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/pgmspace.h>

#ifndef GRAPHICSDRIVER
#define GRAPHICSDRIVER KS0108
#warning Please define GRAPHICSDRIVER in your config.inc
#endif

#if GRAPHICSDRIVER==KS0108
#include "ks0108.h"
#elif GRAPHICSDRIVER==DOTMATRIX
#include "dotmatrix.h"
#else
#error Non supported graphics driver
#endif


//Some parts of the code is from proycon avrlib written by Pascal Stang:
//*****************************************************************************
//
// File Name	: 'glcd.c'
// Title		: Graphic LCD API functions
// Author		: Pascal Stang - Copyright (C) 2002
// Date			: 5/30/2002
// Revised		: 5/30/2002
// Version		: 0.5
// Target MCU	: Atmel AVR
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

void glcdSetColor(uint8_t color){
#if GRAPHICSDRIVER==KS0108
  ks0108SetColor(color);
#elif GRAPHICSDRIVER==DOTMATRIX
  dotmatrixSetColor(color);
#endif
}

uint8_t glcdGetColor(void){
#if GRAPHICSDRIVER==KS0108
  return ks0108GetColor();
#elif GRAPHICSDRIVER==DOTMATRIX
  return dotmatrixGetColor();
#endif
}

void glcdWriteData(uint8_t data, uint8_t color){
#if GRAPHICSDRIVER==KS0108
	ks0108WriteData(data, color);
#elif GRAPHICSDRIVER==DOTMATRIX
	dotmatrixWriteData(data, color);
#endif
}

void glcdWriteDataTransparent(uint8_t inputdata, uint8_t color){
#if GRAPHICSDRIVER==KS0108
	ks0108WriteDataTransparent(inputdata, color);
#elif GRAPHICSDRIVER==DOTMATRIX
	dotmatrixWriteDataTransparent(inputdata, color);
#endif
}

uint8_t glcdReadData(void){
#if GRAPHICSDRIVER==KS0108
	return ks0108ReadData();
#elif GRAPHICSDRIVER==DOTMATRIX
	return dotmatrixReadData();
#endif
}

void glcdClear(){
#if GRAPHICSDRIVER==KS0108
	ks0108Clear();
#elif GRAPHICSDRIVER==DOTMATRIX
	dotmatrixClear();
#endif
}

void glcdSetXY(uint8_t x, uint8_t y){
#if GRAPHICSDRIVER==KS0108
	ks0108SetXY(x, y);
#elif GRAPHICSDRIVER==DOTMATRIX
	dotmatrixSetXY(x, y);
#endif
}

uint8_t glcdGetX(void){
#if GRAPHICSDRIVER==KS0108
	return ks0108GetX();
#elif GRAPHICSDRIVER==DOTMATRIX
	return dotmatrixGetX();
#endif
}

uint8_t glcdGetY(void){
#if GRAPHICSDRIVER==KS0108
	return ks0108GetY();
#elif GRAPHICSDRIVER==DOTMATRIX
	return dotmatrixGetY();
#endif
}

void glcdInit(void)
{
#if GRAPHICSDRIVER==KS0108
	ks0108Init();
#elif GRAPHICSDRIVER==DOTMATRIX
	dotmatrixInit();
#endif
}

void glcdWriteChar(char c, uint8_t color)
{
	uint8_t i = 0;
	if (glcdGetX() > (KS0108_WIDTH-5))
	{
		glcdSetXY(0, glcdGetY() + 8);
	}

	for(i=0; i<5; i++)
	{
		glcdWriteData(pgm_read_byte(&Font5x7[((c - 0x20) * 5) + i]), color); //Borde snyggas till med pekare
	}
	glcdWriteData(0x00, color); //space between this and the next char
}

void glcdPutStr(char *data, uint8_t color){
	while (*data){
		glcdWriteChar(*data, color);
		data++;
	}
}
void glcdWriteCharTransparent(char c, uint8_t color)
{
	uint8_t i = 0;
	if (glcdGetX() > (KS0108_WIDTH-5))
	{
		glcdSetXY(0, glcdGetY() + 8);
	}

	for(i=0; i<5; i++)
	{
		glcdWriteDataTransparent(pgm_read_byte(&Font5x7[((c - 0x20) * 5) + i]), color); //Borde snyggas till med pekare
	}
	glcdWriteDataTransparent(0x00, color); //space between this and the next char
}

void glcdPutStrTransparent(char *data, uint8_t color){
	while (*data){
		glcdWriteCharTransparent(*data, color);
		data++;
	}
}

void glcdDrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {
	uint8_t length, i, y, yAlt, xTmp, yTmp;
	int16_t m;
	
	//
	// vertical line
	//
	if(x1 == x2) {
		// x1|y1 must be the upper point
		if(y1 > y2) {
			yTmp = y1;
			y1 = y2;
			y2 = yTmp;
		}
		glcdDrawVertLine(x1, y1, y2-y1, color);
	
	//
	// horizontal line
	//
	} else if(y1 == y2) {	
		// x1|y1 must be the left point
		if(x1 > x2) {
			xTmp = x1;
			x1 = x2;
			x2 = xTmp;
		}
		glcdDrawHoriLine(x1, y1, x2-x1, color);
	
	//
	// schiefe line :)
	//
	} else {
		// angle >= 45�
		if((y2-y1) >= (x2-x1) || (y1-y2) >= (x2-x1)) {
			// x1 must be smaller than x2
			if(x1 > x2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
		
			length = x2-x1;		// not really the length :)
			m = ((y2-y1)*200)/length;
			yAlt = y1;
			
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+y1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				glcdDrawLine(x1+i, yAlt, x1+i, y, color);
				
				if(length <= (y2-y1) && y1 < y2)
					yAlt = y+1;
				else if(length <= (y1-y2) && y1 > y2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		
		// angle < 45�
		} else {
			// y1 must be smaller than y2
			if(y1 > y2) {
				xTmp = x1;
				yTmp = y1;
				x1 = x2;
				y1 = y2;
				x2 = xTmp;
				y2 = yTmp;
			}
			
			length = y2-y1;
			m = ((x2-x1)*200)/length;
			yAlt = x1;
			
			for(i=0; i<=length; i++) {
				y = ((m*i)/200)+x1;
				
				if((m*i)%200 >= 100)
					y++;
				else if((m*i)%200 <= -100)
					y--;
				
				glcdDrawLine(yAlt, y1+i, y, y1+i, color);
				if(length <= (x2-x1) && x1 < x2)
					yAlt = y+1;
				else if(length <= (x1-x2) && x1 > x2)
					yAlt = y-1;
				else
					yAlt = y;
			}
		}
	}
}

void glcdDrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	glcdDrawHoriLine(x, y, width, color);				// top
	glcdDrawHoriLine(x, y+height, width, color);		// bottom
	glcdDrawVertLine(x, y, height, color);			// left
	glcdDrawVertLine(x+width, y, height, color);		// right
}


void glcdFillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color) {
	uint8_t mask, pageOffset, h, i; //, data;
	height++;
	
	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;
	
	glcdSetXY(x, y);
	for(i=0; i<=width; i++) {
		glcdWriteDataTransparent(mask, color);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		glcdSetXY(x, y);
		
		for(i=0; i<=width; i++) {
			glcdWriteData(0xff, color);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		glcdSetXY(x, y+8);
		
		for(i=0; i<=width; i++) {
			glcdWriteDataTransparent(mask, color);
		}
	}
}

void glcdInvertRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
	uint8_t mask, pageOffset, h, i, data, tmpData;
	height++;
	
	pageOffset = y%8;
	y -= pageOffset;
	mask = 0xFF;
	if(height < 8-pageOffset) {
		mask >>= (8-height);
		h = height;
	} else {
		h = 8-pageOffset;
	}
	mask <<= pageOffset;
	
	glcdSetXY(x, y);
	for(i=0; i<=width; i++) {
		data = glcdReadData(); //glcdReadData();
		tmpData = ~data;
		data = (tmpData & mask) | (data & ~mask);
		glcdWriteData(data, GLCD_COLOR_SET);
	}
	
	while(h+8 <= height) {
		h += 8;
		y += 8;
		glcdSetXY(x, y);
		
		for(i=0; i<=width; i++) {
			data = glcdReadData();
			glcdWriteData(~data, GLCD_COLOR_SET);
		}
	}
	
	if(h < height) {
		mask = ~(0xFF << (height-h));
		glcdSetXY(x, y+8);
		
		for(i=0; i<=width; i++) {
			data = glcdReadData(); //glcdReadData();
			tmpData = ~data;
			data = (tmpData & mask) | (data & ~mask);
			glcdWriteData(data, GLCD_COLOR_SET);
		}
	}
}

void glcdInvert(void) {
	if(glcdGetColor() == GLCD_COLOR_BLACK)
		glcdSetColor(GLCD_COLOR_WHITE);
	else
		glcdSetColor(GLCD_COLOR_BLACK);
	glcdInvertRect(0,0,KS0108_WIDTH-1,KS0108_HIGHT-1);
}

void glcdSetDot(uint8_t x, uint8_t y, uint8_t color) {
	glcdSetXY(x, y-y%8);					// read data from display memory
	glcdWriteDataTransparent(0x01 << (y%8), GLCD_COLOR_SET);
}

void glcdDrawRoundRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t radius, uint8_t color) {
  	int16_t tSwitch, x1 = 0, y1 = radius;
  	tSwitch = 3 - 2 * radius;
	
	while (x1 <= y1) {
	    glcdSetDot(x+radius - x1, y+radius - y1, color);
	    glcdSetDot(x+radius - y1, y+radius - x1, color);

	    glcdSetDot(x+width-radius + x1, y+radius - y1, color);
	    glcdSetDot(x+width-radius + y1, y+radius - x1, color);
	    
	    glcdSetDot(x+width-radius + x1, y+height-radius + y1, color);
	    glcdSetDot(x+width-radius + y1, y+height-radius + x1, color);

	    glcdSetDot(x+radius - x1, y+height-radius + y1, color);
	    glcdSetDot(x+radius - y1, y+height-radius + x1, color);

	    if (tSwitch < 0) {
	    	tSwitch += (4 * x1 + 6);
	    } else {
	    	tSwitch += (4 * (x1 - y1) + 10);
	    	y1--;
	    }
	    x1++;
	}
	  	
	glcdDrawHoriLine(x+radius, y, width-(2*radius), color);			// top
	glcdDrawHoriLine(x+radius, y+height, width-(2*radius), color);	// bottom
	glcdDrawVertLine(x, y+radius, height-(2*radius), color);			// left
	glcdDrawVertLine(x+width, y+radius, height-(2*radius), color);	// right
}
