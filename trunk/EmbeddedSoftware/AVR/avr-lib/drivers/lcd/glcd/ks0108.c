#include "ks0108.h"
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
#ifndef GRAPHICS_WIDTH
#error GRAPHICS_WIDTH, GRAPHICS_HEIGHT must be defined
#endif

#define OUTPUT	0
#define INPUT	1

volatile GrLcdStateType GrLcdState;

void ks0108SetControls(uint8_t RS, uint8_t RW){
	if(RS) {gpio_set_pin(LCD_CONTROL_RS);} else {gpio_clr_pin(LCD_CONTROL_RS);}
	if(RW) {gpio_set_pin(LCD_CONTROL_RW);} else {gpio_clr_pin(LCD_CONTROL_RW);}
}

void ks0108SetData(uint8_t Data){
	if(Data&0x01) {gpio_set_pin(LCD_DATA_DB0);} else {gpio_clr_pin(LCD_DATA_DB0);}
	if(Data&0x02) {gpio_set_pin(LCD_DATA_DB1);} else {gpio_clr_pin(LCD_DATA_DB1);}
	if(Data&0x04) {gpio_set_pin(LCD_DATA_DB2);} else {gpio_clr_pin(LCD_DATA_DB2);}
	if(Data&0x08) {gpio_set_pin(LCD_DATA_DB3);} else {gpio_clr_pin(LCD_DATA_DB3);}
	if(Data&0x10) {gpio_set_pin(LCD_DATA_DB4);} else {gpio_clr_pin(LCD_DATA_DB4);}
	if(Data&0x20) {gpio_set_pin(LCD_DATA_DB5);} else {gpio_clr_pin(LCD_DATA_DB5);}
	if(Data&0x40) {gpio_set_pin(LCD_DATA_DB6);} else {gpio_clr_pin(LCD_DATA_DB6);}
	if(Data&0x80) {gpio_set_pin(LCD_DATA_DB7);} else {gpio_clr_pin(LCD_DATA_DB7);}
}
uint8_t ks0108GetData(void){
	uint8_t input = 0;
	if (gpio_get_state(LCD_DATA_DB0)) {input += 1;}
	if (gpio_get_state(LCD_DATA_DB1)) {input += 2;}
	if (gpio_get_state(LCD_DATA_DB2)) {input += 4;}
	if (gpio_get_state(LCD_DATA_DB3)) {input += 8;}
	if (gpio_get_state(LCD_DATA_DB4)) {input += 16;}
	if (gpio_get_state(LCD_DATA_DB5)) {input += 32;}
	if (gpio_get_state(LCD_DATA_DB6)) {input += 64;}
	if (gpio_get_state(LCD_DATA_DB7)) {input += 128;}
	return input;
}

void ks0108Disable(){
	gpio_clr_pin(LCD_CONTROL_E);
}

void ks0108Delay(){
	delay_us(2);
}

void ks0108Enable(){
	ks0108Delay();
	gpio_set_pin(LCD_CONTROL_E);
	ks0108Delay();
	ks0108Disable();
	ks0108Delay();
}

void ks0108SetDirection(uint8_t dir){
  if (dir == OUTPUT){
    gpio_set_out(LCD_DATA_DB0);
    gpio_set_out(LCD_DATA_DB1);
    gpio_set_out(LCD_DATA_DB2);
    gpio_set_out(LCD_DATA_DB3);
    gpio_set_out(LCD_DATA_DB4);
    gpio_set_out(LCD_DATA_DB5);
    gpio_set_out(LCD_DATA_DB6);
    gpio_set_out(LCD_DATA_DB7);
  }else if (dir == INPUT){
    gpio_set_in(LCD_DATA_DB0);
    gpio_set_in(LCD_DATA_DB1);
    gpio_set_in(LCD_DATA_DB2);
    gpio_set_in(LCD_DATA_DB3);
    gpio_set_in(LCD_DATA_DB4);
    gpio_set_in(LCD_DATA_DB5);
    gpio_set_in(LCD_DATA_DB6);
    gpio_set_in(LCD_DATA_DB7);
  }
}
void ks0108SetColor(uint8_t color){
  GrLcdState.color=color;
}
uint8_t ks0108GetColor(void){
  return GrLcdState.color;
}
void ks0108WriteData(uint8_t data, uint8_t color){
	ks0108SetControls(1,0);
	if (color == GLCD_COLOR_CLEAR)
	  data = ~data;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  ks0108SetData(data);
	else
	  ks0108SetData(~data);
	ks0108Enable();
	GrLcdState.lcdXAddr++;

	if (GrLcdState.lcdXAddr == 64 ){
		ks0108SetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	} 
#if GRAPHICS_WIDTH > 128
else if (GrLcdState.lcdXAddr == 128 ){
		ks0108SetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
#endif
	if (GrLcdState.lcdXAddr > GRAPHICS_WIDTH ){
		ks0108SetXY(0,GrLcdState.lcdYAddr+8);
	}
}
void ks0108WriteDataTransparent(uint8_t inputdata, uint8_t color){
	uint8_t data = 0;
	ks0108SetDirection(INPUT);
	ks0108SetControls(1,1);
	ks0108Enable();	//dummy read
	ks0108Delay();
	gpio_set_pin(LCD_CONTROL_E);
	ks0108Delay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = ks0108GetData();
	else
	  data = ~ks0108GetData();
	ks0108Disable();
	ks0108Delay();
	ks0108SetControls(1,0);
	ks0108SetDirection(OUTPUT);
	ks0108SetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);
	ks0108SetControls(1,0);
	if (color == GLCD_COLOR_CLEAR) 
	  data = data&(~inputdata);
	else
	  data = data|inputdata;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  ks0108SetData(data);
	else
	  ks0108SetData(~data);
	ks0108Enable();
	GrLcdState.lcdXAddr++;


	if (GrLcdState.lcdXAddr == 64 ){
		ks0108SetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	} 
#if GRAPHICS_WIDTH > 128
	else if (GrLcdState.lcdXAddr == 128 ){
		ks0108SetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
#endif
	if (GrLcdState.lcdXAddr > GRAPHICS_WIDTH ){
		ks0108SetXY(0,GrLcdState.lcdYAddr+8);
	}
}

uint8_t ks0108ReadData(void){
	uint8_t data = 0;
	ks0108SetDirection(INPUT);
	ks0108SetControls(1,1);
	ks0108Enable();	//dummy read
	ks0108Delay();
	gpio_set_pin(LCD_CONTROL_E);
	ks0108Delay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = ks0108GetData();
	else
	  data = ~ks0108GetData();
	ks0108Disable();
	ks0108Delay();
	ks0108SetControls(1,0);
	ks0108SetDirection(OUTPUT);
	ks0108SetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);
	return data;
}

void ks0108Init(){
	gpio_set_out(LCD_CONTROL_RS);
	gpio_set_out(LCD_CONTROL_RW);
	gpio_set_out(LCD_CONTROL_E);
	gpio_set_out(LCD_CONTROL_CS1);
	gpio_set_out(LCD_CONTROL_CS2);

	ks0108SetDirection(OUTPUT);
	GrLcdState.color = GLCD_COLOR_WHITE;
#if KS0108_INVERT_CS == 1
	gpio_clr_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#else
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_set_pin(LCD_CONTROL_CS2);
#endif
	ks0108Disable();
	ks0108Delay();
	//set display on
	ks0108SetControls(0,0);
	ks0108SetData(0x3F);
	ks0108Enable();

	//set startaddress of the first row (set to row 0)
	ks0108SetControls(0,0);
	ks0108SetData(0xc0);
	ks0108Enable();
	ks0108Clear();
}

void ks0108Clear(){
#if KS0108_INVERT_CS == 1
	gpio_clr_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#else
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_set_pin(LCD_CONTROL_CS2);
#endif
	ks0108Enable();
	ks0108Delay();
	ks0108Disable();

	uint8_t i;
	uint8_t j;

	for(j = 0; j < 8; j++){
		ks0108SetControls(0,0);
		ks0108SetData(0xB8 + j);
		ks0108Enable();

		ks0108SetControls(0,0);
		ks0108SetData(0x40);
		ks0108Enable();
		for (i = 0; i < 64; i++){
			ks0108SetControls(1,0);
			if (GrLcdState.color == GLCD_COLOR_BLACK)
			  ks0108SetData(0x00);
			else
			  ks0108SetData(0xff);
			ks0108Enable();
		}
	}

}

void ks0108SetXY(uint8_t x, uint8_t y){
	GrLcdState.lcdXAddr = x;
	GrLcdState.lcdYAddr = y;
	GrLcdState.lcdYpage = y/8;

	//Vi b�rjar med X. Steg 1: V�lj r�tt chip:
	if (x > 63 && x <= 127){
#if KS0108_INVERT_CS == 1
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#else
	gpio_set_pin(LCD_CONTROL_CS2);
	gpio_clr_pin(LCD_CONTROL_CS1);
#endif
	} else if (x > 127){
#if KS0108_INVERT_CS == 1
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_set_pin(LCD_CONTROL_CS2);
#else
	gpio_clr_pin(LCD_CONTROL_CS2);
	gpio_clr_pin(LCD_CONTROL_CS1);
#endif
	} else {
#if KS0108_INVERT_CS == 1
	gpio_set_pin(LCD_CONTROL_CS2);
	gpio_clr_pin(LCD_CONTROL_CS1);
#else
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#endif
	}

	//Steg 2: S�tt r�tt x-adress p� det aktiva chippet
	ks0108SetControls(0,0);
	ks0108SetData(0x40 + x%64);
	ks0108Enable();

	//Steg 3: S�tt r�tt y-adress p� det aktiva chippet
	ks0108SetData(0xB8 + GrLcdState.lcdYpage);
	ks0108Enable();
}
uint8_t ks0108GetX(void){
	return GrLcdState.lcdXAddr;
}
uint8_t ks0108GetY(void){
	return GrLcdState.lcdYAddr;
}

