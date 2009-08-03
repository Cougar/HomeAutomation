/*! \file i2ceeprom.c \brief Interface for standard I2C EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'i2ceeprom.c'
// Title		: Interface for standard I2C EEPROM memories
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.04.23
// Revised		: 2003.04.23
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include <drivers/mcu/i2c.h>
#include <drivers/misc/i2ceeprom.h>

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// functions
void i2ceepromInit(void)
{
	i2cInit();
	// although there is no code here
	// don't forget to initialize the I2C interface itself
}

uint8_t i2ceepromReadByte(uint8_t i2cAddr, uint16_t memAddr)
{
	uint8_t packet[2];
	// prepare address
	packet[0] = (memAddr>>8);
	packet[1] = (memAddr&0x00FF);
	// send memory address we wish to access to the memory chip
	i2cMasterSend(i2cAddr, 2, packet);
	// retrieve the data at this memory address
	i2cMasterReceive(i2cAddr, 1, packet);
	// return data
	return packet[0];
}

void i2ceepromWriteByte(uint8_t i2cAddr, uint16_t memAddr, uint8_t data)
{
	uint8_t packet[3];
	// prepare address + data
	packet[0] = (memAddr>>8);
	packet[1] = (memAddr&0x00FF);
	packet[2] = data;
	// send memory address we wish to access to the memory chip
	// along with the data we wish to write
	i2cMasterSend(i2cAddr, 3, packet);
}
