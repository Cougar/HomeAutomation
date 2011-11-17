#include "dotmatrix.h"
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
#ifndef KS0108_WIDTH
#error KS0108_WIDTH, KS0108_HIGHT must be defined
#endif

#define OUTPUT	0
#define INPUT	1

volatile GrLcdStateType GrLcdState;

void dotmatrixSetControls(uint8_t RS, uint8_t RW){
//	if(RS) {gpio_set_pin(LCD_CONTROL_RS);} else {gpio_clr_pin(LCD_CONTROL_RS);}
//	if(RW) {gpio_set_pin(LCD_CONTROL_RW);} else {gpio_clr_pin(LCD_CONTROL_RW);}
}

void dotmatrixSetData(uint8_t Data){
/*	if(Data&0x01) {gpio_set_pin(LCD_DATA_DB0);} else {gpio_clr_pin(LCD_DATA_DB0);}
	if(Data&0x02) {gpio_set_pin(LCD_DATA_DB1);} else {gpio_clr_pin(LCD_DATA_DB1);}
	if(Data&0x04) {gpio_set_pin(LCD_DATA_DB2);} else {gpio_clr_pin(LCD_DATA_DB2);}
	if(Data&0x08) {gpio_set_pin(LCD_DATA_DB3);} else {gpio_clr_pin(LCD_DATA_DB3);}
	if(Data&0x10) {gpio_set_pin(LCD_DATA_DB4);} else {gpio_clr_pin(LCD_DATA_DB4);}
	if(Data&0x20) {gpio_set_pin(LCD_DATA_DB5);} else {gpio_clr_pin(LCD_DATA_DB5);}
	if(Data&0x40) {gpio_set_pin(LCD_DATA_DB6);} else {gpio_clr_pin(LCD_DATA_DB6);}
	if(Data&0x80) {gpio_set_pin(LCD_DATA_DB7);} else {gpio_clr_pin(LCD_DATA_DB7);}*/
}
uint8_t dotmatrixGetData(void){
	uint8_t input = 0;
/*	if (gpio_get_state(LCD_DATA_DB0)) {input += 1;}
	if (gpio_get_state(LCD_DATA_DB1)) {input += 2;}
	if (gpio_get_state(LCD_DATA_DB2)) {input += 4;}
	if (gpio_get_state(LCD_DATA_DB3)) {input += 8;}
	if (gpio_get_state(LCD_DATA_DB4)) {input += 16;}
	if (gpio_get_state(LCD_DATA_DB5)) {input += 32;}
	if (gpio_get_state(LCD_DATA_DB6)) {input += 64;}
	if (gpio_get_state(LCD_DATA_DB7)) {input += 128;}*/
	return input;
}

void dotmatrixDisable(){
//	gpio_clr_pin(LCD_CONTROL_E);
}

void dotmatrixDelay(){
//	delay_us(2);
}

void dotmatrixEnable(){
/*	dotmatrixDelay();
	gpio_set_pin(LCD_CONTROL_E);
	dotmatrixDelay();
	dotmatrixDisable();
	dotmatrixDelay();*/
}

void dotmatrixSetDirection(uint8_t dir){
/*  if (dir == OUTPUT){
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
  }*/
}
void dotmatrixSetColor(uint8_t color){
  GrLcdState.color=color;
}
uint8_t dotmatrixGetColor(void){
  return GrLcdState.color;
}
void dotmatrixWriteData(uint8_t data, uint8_t color){
/*	dotmatrixSetControls(1,0);
	if (color == GLCD_COLOR_CLEAR)
	  data = ~data;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  dotmatrixSetData(data);
	else
	  dotmatrixSetData(~data);
	dotmatrixEnable();
	GrLcdState.lcdXAddr++;

	if (GrLcdState.lcdXAddr == 64 ){
		dotmatrixSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	} 
#if KS0108_WIDTH > 128
else if (GrLcdState.lcdXAddr == 128 ){
		dotmatrixSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
#endif
	if (GrLcdState.lcdXAddr > KS0108_WIDTH ){
		dotmatrixSetXY(0,GrLcdState.lcdYAddr+8);
	}*/
}
void dotmatrixWriteDataTransparent(uint8_t inputdata, uint8_t color){
/*	uint8_t data = 0;
	dotmatrixSetDirection(INPUT);
	dotmatrixSetControls(1,1);
	dotmatrixEnable();	//dummy read
	dotmatrixDelay();
	gpio_set_pin(LCD_CONTROL_E);
	dotmatrixDelay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = dotmatrixGetData();
	else
	  data = ~dotmatrixGetData();
	dotmatrixDisable();
	dotmatrixDelay();
	dotmatrixSetControls(1,0);
	dotmatrixSetDirection(OUTPUT);
	dotmatrixSetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);
	dotmatrixSetControls(1,0);
	if (color == GLCD_COLOR_CLEAR) 
	  data = data&(~inputdata);
	else
	  data = data|inputdata;
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  dotmatrixSetData(data);
	else
	  dotmatrixSetData(~data);
	dotmatrixEnable();
	GrLcdState.lcdXAddr++;


	if (GrLcdState.lcdXAddr == 64 ){
		dotmatrixSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	} 
#if KS0108_WIDTH > 128
	else if (GrLcdState.lcdXAddr == 128 ){
		dotmatrixSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
#endif
	if (GrLcdState.lcdXAddr > KS0108_WIDTH ){
		dotmatrixSetXY(0,GrLcdState.lcdYAddr+8);
	}*/
}

uint8_t dotmatrixReadData(void){
	uint8_t data = 0;
/*	dotmatrixSetDirection(INPUT);
	dotmatrixSetControls(1,1);
	dotmatrixEnable();	//dummy read
	dotmatrixDelay();
	gpio_set_pin(LCD_CONTROL_E);
	dotmatrixDelay();
	if (GrLcdState.color == GLCD_COLOR_BLACK)
	  data = dotmatrixGetData();
	else
	  data = ~dotmatrixGetData();
	dotmatrixDisable();
	dotmatrixDelay();
	dotmatrixSetControls(1,0);
	dotmatrixSetDirection(OUTPUT);
	dotmatrixSetXY(GrLcdState.lcdXAddr, GrLcdState.lcdYAddr);*/
	return data;
}

void dotmatrixInit(){
/*	gpio_set_out(LCD_CONTROL_RS);
	gpio_set_out(LCD_CONTROL_RW);
	gpio_set_out(LCD_CONTROL_E);
	gpio_set_out(LCD_CONTROL_CS1);
	gpio_set_out(LCD_CONTROL_CS2);

	dotmatrixSetDirection(OUTPUT);
	GrLcdState.color = GLCD_COLOR_WHITE;
#if KS0108_INVERT_CS == 1
	gpio_clr_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#else
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_set_pin(LCD_CONTROL_CS2);
#endif
	dotmatrixDisable();
	dotmatrixDelay();
	//set display on
	dotmatrixSetControls(0,0);
	dotmatrixSetData(0x3F);
	dotmatrixEnable();

	//set startaddress of the first row (set to row 0)
	dotmatrixSetControls(0,0);
	dotmatrixSetData(0xc0);
	dotmatrixEnable();
	dotmatrixClear();*/
}

void dotmatrixClear(){
/*
#if KS0108_INVERT_CS == 1
	gpio_clr_pin(LCD_CONTROL_CS1);
	gpio_clr_pin(LCD_CONTROL_CS2);
#else
	gpio_set_pin(LCD_CONTROL_CS1);
	gpio_set_pin(LCD_CONTROL_CS2);
#endif
	dotmatrixEnable();
	dotmatrixDelay();
	dotmatrixDisable();

	uint8_t i;
	uint8_t j;

	for(j = 0; j < 8; j++){
		dotmatrixSetControls(0,0);
		dotmatrixSetData(0xB8 + j);
		dotmatrixEnable();

		dotmatrixSetControls(0,0);
		dotmatrixSetData(0x40);
		dotmatrixEnable();
		for (i = 0; i < 64; i++){
			dotmatrixSetControls(1,0);
			if (GrLcdState.color == GLCD_COLOR_BLACK)
			  dotmatrixSetData(0x00);
			else
			  dotmatrixSetData(0xff);
			dotmatrixEnable();
		}
	}
*/
}

void dotmatrixSetXY(uint8_t x, uint8_t y){
/*	GrLcdState.lcdXAddr = x;
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
	dotmatrixSetControls(0,0);
	dotmatrixSetData(0x40 + x%64);
	dotmatrixEnable();

	//Steg 3: S�tt r�tt y-adress p� det aktiva chippet
	dotmatrixSetData(0xB8 + GrLcdState.lcdYpage);
	dotmatrixEnable();*/
}
uint8_t dotmatrixGetX(void){
	return GrLcdState.lcdXAddr;
}
uint8_t dotmatrixGetY(void){
	return GrLcdState.lcdYAddr;
}

