/*
 * FOST02 temperature and humidity sensor
 *
 * Using "two-wire" interface for reading sensor values.
 *
 * Author: Linus Lundin
 * Date: 2007-08-12
 *
 */

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <drivers/sensor/ds18s20/delay.h>
#include "FOST02.h"

/*----------------------------------------------
 * Functions
 * --------------------------------------------*/


/* 
 * Initiate FOST02 for reading temperature and humidity from sensor.
 */
void FOST02Init(void)
{
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	FOST02_CLK_DIR |=(1<<FOST02_CLK); //set 1 in DDR line CLK = Out
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	FOST02_DATA_PORT &= ~(1<<FOST02_DATA); //set 0 in FOST_DATA
}
void sendReset(void)
{
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	delay_us(2);
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_DATA_DIR |=(1<<FOST02_DATA); //set 1 in DDR line Data = 0 (data line = output)
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	delay_us(2);
}
uint8_t sendCommand(uint8_t cmd)
{
	sendReset();
	uint8_t i,ret;
	for (i = 0; i<8 ; i++)
	{
		if ((cmd & 0x80) == 0x80)
			FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
		else
			FOST02_DATA_DIR |=(1<<FOST02_DATA); //set 1 in DDR line Data = 0 (data line = output)
		delay_us(2);
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		cmd = cmd << 1;
		delay_us(2);
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		delay_us(2);
	}
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		
	delay_us(2);
	if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) == 0) 
	{
		ret = 1;
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		delay_us(2);
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		while ((FOST02_DATA_PIN & (1<<FOST02_DATA)) == 0);
		delay_us(2);
	}
	else
		ret = 0;
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	delay_us(2);
	return ret;
}

uint8_t getFOST02PinStatus(void)
{
	if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) != 0) 
		return 1;
	else
		return 0; 
}
uint16_t getFOST02Data(void)
{
	uint8_t i,data1, data2;
	uint16_t data = 0;
	data=0;
	data1=0;
	data2=0; 
	for (i = 0; i<8 ; i++)
	{
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		delay_us(2);
		data1 = data1 << 1;
		if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) != 0) 
			data1 = data1 | 0x0001;
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		delay_us(2);
	}
	FOST02_DATA_DIR |=(1<<FOST02_DATA); //set 1 in DDR line Data = 0 (data line = output)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	for (i = 0; i<8 ; i++)
	{
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		delay_us(2);
		data2 = data2 << 1;
		if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) != 0) 
			data2 = data2 | 0x0001;
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		delay_us(2);
	}
	//skip Ack
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	data = (uint16_t) (data1<<8);
	data += data2;
	return data;
}

void getFOST02DataByte(uint8_t *data1in, uint8_t *data2in)
{
	uint8_t i;
	uint16_t data = 0;
	uint8_t data1 = 0;
	uint8_t data2 = 0;
	for (i = 0; i<8 ; i++)
	{
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		delay_us(2);
		data1 = data1 << 1;
		if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) != 0) 
			data1 = data1 | 0x0001;
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		delay_us(2);
	}
	FOST02_DATA_DIR |=(1<<FOST02_DATA); //set 1 in DDR line Data = 0 (data line = output)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	for (i = 0; i<8 ; i++)
	{
		FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
		delay_us(2);
		data2 = data2 << 1;
		if ( (FOST02_DATA_PIN & (1<<FOST02_DATA)) != 0) 
			data2 = data2 | 0x0001;
		FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
		delay_us(2);
	}
	//skip Ack
	FOST02_DATA_DIR &= ~(1<<FOST02_DATA); // set 0 in DDR line: Data = 1 (data line = input)
	delay_us(2);
	FOST02_CLK_PORT |= (1<<FOST02_CLK); //set 1 in FOST_CLK
	delay_us(2);
	FOST02_CLK_PORT &= ~(1<<FOST02_CLK); //set 0 in FOST_CLK
	*data1in = data1;
	*data2in = data2;
}
/*
 * Start reading and return the temperature value.
 */
void measureFOST02temperature(void) {}
void measureFOST02humidity(void) {}
uint16_t getFOST02temperature(void) {return 0;}
uint16_t getFOST02humidity(void) {return 0;}
