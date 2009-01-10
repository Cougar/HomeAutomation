#include "ks0108.h"
//#include <avr/io.h>
#include "font.h"
//#include <avr\pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/pgmspace.h>

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

#define OUTPUT	0
#define INPUT	1

volatile GrLcdStateType GrLcdState;

void SetControls(uint8_t RS, uint8_t RW){
	if(RS) {LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_RS);} else { LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_RS);};
	if(RW) {LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_RW);} else { LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_RW);};
}

void SetData(uint8_t Data){
	if(Data&0x01) {LCD_DATA_PORT_DB0 |= (1<<LCD_DATA_PIN_DB0);} else { LCD_DATA_PORT_DB0 &= ~(1<<LCD_DATA_PIN_DB0);};
	if(Data&0x02) {LCD_DATA_PORT_DB1 |= (1<<LCD_DATA_PIN_DB1);} else { LCD_DATA_PORT_DB1 &= ~(1<<LCD_DATA_PIN_DB1);};
	if(Data&0x04) {LCD_DATA_PORT_DB2 |= (1<<LCD_DATA_PIN_DB2);} else { LCD_DATA_PORT_DB2 &= ~(1<<LCD_DATA_PIN_DB2);};
	if(Data&0x08) {LCD_DATA_PORT_DB3 |= (1<<LCD_DATA_PIN_DB3);} else { LCD_DATA_PORT_DB3 &= ~(1<<LCD_DATA_PIN_DB3);};
	if(Data&0x10) {LCD_DATA_PORT_DB4 |= (1<<LCD_DATA_PIN_DB4);} else { LCD_DATA_PORT_DB4 &= ~(1<<LCD_DATA_PIN_DB4);};
	if(Data&0x20) {LCD_DATA_PORT_DB5 |= (1<<LCD_DATA_PIN_DB5);} else { LCD_DATA_PORT_DB5 &= ~(1<<LCD_DATA_PIN_DB5);};
	if(Data&0x40) {LCD_DATA_PORT_DB6 |= (1<<LCD_DATA_PIN_DB6);} else { LCD_DATA_PORT_DB6 &= ~(1<<LCD_DATA_PIN_DB6);};
	if(Data&0x80) {LCD_DATA_PORT_DB7 |= (1<<LCD_DATA_PIN_DB7);} else { LCD_DATA_PORT_DB7 &= ~(1<<LCD_DATA_PIN_DB7);};
}
uint8_t GetData(void){
	uint8_t input = 0;
	if (LCD_DATA_IN_DB0 & (1<<LCD_DATA_PIN_DB0)) {input += 1;}
	if (LCD_DATA_IN_DB1 & (1<<LCD_DATA_PIN_DB1)) {input += 2;}
	if (LCD_DATA_IN_DB2 & (1<<LCD_DATA_PIN_DB2)) {input += 4;}
	if (LCD_DATA_IN_DB3 & (1<<LCD_DATA_PIN_DB3)) {input += 8;}
	if (LCD_DATA_IN_DB4 & (1<<LCD_DATA_PIN_DB4)) {input += 16;}
	if (LCD_DATA_IN_DB5 & (1<<LCD_DATA_PIN_DB5)) {input += 32;}
	if (LCD_DATA_IN_DB6 & (1<<LCD_DATA_PIN_DB6)) {input += 64;}
	if (LCD_DATA_IN_DB7 & (1<<LCD_DATA_PIN_DB7)) {input += 128;}
	return input;
}

void Disable(){
	LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_E);
}

void Delay(){
	delay_us(1);
}

void Enable(){
	Delay();
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_E);
	Delay();
	Disable();
	Delay();
}

void SetDirection(uint8_t dir){
  if (dir == OUTPUT){
    LCD_DATA_DDR_DB0 |= (1<<LCD_DATA_PIN_DB0);
    LCD_DATA_DDR_DB1 |= (1<<LCD_DATA_PIN_DB1);
    LCD_DATA_DDR_DB2 |= (1<<LCD_DATA_PIN_DB2);
    LCD_DATA_DDR_DB3 |= (1<<LCD_DATA_PIN_DB3);
    LCD_DATA_DDR_DB4 |= (1<<LCD_DATA_PIN_DB4);
    LCD_DATA_DDR_DB5 |= (1<<LCD_DATA_PIN_DB5);
    LCD_DATA_DDR_DB6 |= (1<<LCD_DATA_PIN_DB6);
    LCD_DATA_DDR_DB7 |= (1<<LCD_DATA_PIN_DB7);
  }else if (dir == INPUT){
    LCD_DATA_DDR_DB0 &= ~(1<<LCD_DATA_PIN_DB0);
    LCD_DATA_DDR_DB1 &= ~(1<<LCD_DATA_PIN_DB1);
    LCD_DATA_DDR_DB2 &= ~(1<<LCD_DATA_PIN_DB2);
    LCD_DATA_DDR_DB3 &= ~(1<<LCD_DATA_PIN_DB3);
    LCD_DATA_DDR_DB4 &= ~(1<<LCD_DATA_PIN_DB4);
    LCD_DATA_DDR_DB5 &= ~(1<<LCD_DATA_PIN_DB5);
    LCD_DATA_DDR_DB6 &= ~(1<<LCD_DATA_PIN_DB6);
    LCD_DATA_DDR_DB7 &= ~(1<<LCD_DATA_PIN_DB7);
  }
}
void glcdSetColor(uint8_t color){
  GrLcdState.color=color;
}
uint8_t glcdGetColor(void){
  return GrLcdState.color;
}
void glcdWriteData(uint8_t data, uint8_t color){
	SetControls(1,0);
	if (color == GLCD_COLOR_CLEAR)
	  data = ~data;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  SetData(data);
	else
	  SetData(~data);
	Enable();
	GrLcdState.lcdXAddr++;

	if (GrLcdState.lcdXAddr > 63){
		glcdSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
	if (GrLcdState.lcdXAddr > 128){
		glcdSetXY(0,GrLcdState.lcdYAddr+8);
	}
}
void glcdWriteDataTransparent(uint8_t inputdata, uint8_t color){
	uint8_t data = 0;
	SetDirection(INPUT);
	SetControls(1,1);
	Enable();	//dummy read
	Delay();
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_E);
	Delay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = GetData();
	else
	  data = ~GetData();
	Disable();
	Delay();
	SetControls(1,0);
	SetDirection(OUTPUT);
	glcdSetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);
	SetControls(1,0);
	if (color == GLCD_COLOR_CLEAR) 
	  data = data&(~inputdata);
	else
	  data = data|inputdata;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  SetData(data);
	else
	  SetData(~data);
	Enable();
	GrLcdState.lcdXAddr++;

	if (GrLcdState.lcdXAddr > 63){
		glcdSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
	if (GrLcdState.lcdXAddr > 128){
		glcdSetXY(0,GrLcdState.lcdYAddr+8);
	}
}

uint8_t glcdReadData(void){
	uint8_t data = 0;
	SetDirection(INPUT);
	SetControls(1,1);
	Enable();	//dummy read
	Delay();
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_E);
	Delay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = GetData();
	else
	  data = ~GetData();
	Disable();
	Delay();
	SetControls(1,0);
	SetDirection(OUTPUT);
	glcdSetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);
	return data;
}

void glcdPowerOn(){
	LCD_CONTROL_DDR |= (1<<LCD_CONTROL_PIN_RS);
	LCD_CONTROL_DDR |= (1<<LCD_CONTROL_PIN_RW);
	LCD_CONTROL_DDR |= (1<<LCD_CONTROL_PIN_E);
	LCD_CONTROL_DDR_CS |= (1<<LCD_CONTROL_PIN_CS1);
	LCD_CONTROL_DDR_CS |= (1<<LCD_CONTROL_PIN_CS2);

	SetDirection(OUTPUT);
	GrLcdState.color = GLCD_COLOR_BLACK;
	LCD_CONTROL_PORT_CS |= (1<<LCD_CONTROL_PIN_CS2)|(1<<LCD_CONTROL_PIN_CS1);
	Disable();
	Delay();
	//set display on
	SetControls(0,0);
	SetData(0x3F);
	Enable();

	//set startaddress of the first row (set to row 0)
	SetControls(0,0);
	SetData(0xc0);
	Enable();

	glcdClear();
}

void glcdClear(){
	LCD_CONTROL_PORT_CS |= (1<<LCD_CONTROL_PIN_CS2)|(1<<LCD_CONTROL_PIN_CS1);
	Enable();
	Delay();
	Disable();

	uint8_t i;
	uint8_t j;

	for(j = 0; j < 8; j++){
		SetControls(0,0);
		SetData(0xB8 + j);
		Enable();

		SetControls(0,0);
		SetData(0x40);
		Enable();
		for (i = 0; i < 64; i++){
			SetControls(1,0);
			if (GrLcdState.color == GLCD_COLOR_BLACK)
			  SetData(0x00);
			else
			  SetData(0xff);
			Enable();
		}
	}

}

void glcdSetXY(uint8_t x, uint8_t y){
	GrLcdState.lcdXAddr = x;
	GrLcdState.lcdYAddr = y;
	GrLcdState.lcdYpage = y/8;

	//Vi b�rjar med X. Steg 1: V�lj r�tt chip:
	if (x > 63){
		LCD_CONTROL_PORT_CS |= (1<<LCD_CONTROL_PIN_CS2);
		LCD_CONTROL_PORT_CS &= ~(1<<LCD_CONTROL_PIN_CS1);
	} else {
		LCD_CONTROL_PORT_CS |= (1<<LCD_CONTROL_PIN_CS1);
		LCD_CONTROL_PORT_CS &= ~(1<<LCD_CONTROL_PIN_CS2);
	}

	//Steg 2: S�tt r�tt x-adress p� det aktiva chippet
	SetControls(0,0);
	SetData(0x40 + x%64);
	Enable();

	//Steg 3: S�tt r�tt y-adress p� det aktiva chippet
	SetData(0xB8 + GrLcdState.lcdYpage);
	Enable();
}
uint8_t glcdGetX(void){
	return GrLcdState.lcdXAddr;
}
uint8_t glcdGetY(void){
	return GrLcdState.lcdYAddr;
}


void glcdWriteChar(char c, uint8_t color)
{
	uint8_t i = 0;
	if (GrLcdState.lcdXAddr > (128-5))
	{
		glcdSetXY(0,GrLcdState.lcdYAddr + 8);
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
	if (GrLcdState.lcdXAddr > (128-5))
	{
		glcdSetXY(0,GrLcdState.lcdYAddr + 8);
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
	if(GrLcdState.color == GLCD_COLOR_BLACK)
		GrLcdState.color = GLCD_COLOR_WHITE;
	else
		GrLcdState.color = GLCD_COLOR_BLACK;
	glcdInvertRect(0,0,127,63);
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
