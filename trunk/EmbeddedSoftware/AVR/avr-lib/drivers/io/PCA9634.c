
/*-----------------------------------------------
 * Inclusions
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <config.h>
#include "PCA9634.h"
#include <drivers/mcu/TWI_Master.h>

/*-----------------------------------------------
 * Function Implementations
 * ---------------------------------------------*/

uint8_t pca9634_address;
uint8_t pca9634_LEDOUT0=0;
uint8_t pca9634_LEDOUT1=0;

/*---------------------------------------------------------------------------*/

void pca9634_init(uint8_t address)
{
	unsigned char msgBuf[5];
	TWI_Master_Initialise();
	pca9634_address = address;
	/* init, MODE1.SLEEP shall be set to normal mode */
	uint8_t data = PCA9634_MODE1_SLEEP|PCA9634_MODE1_SUB1|PCA9634_MODE1_SUB2|PCA9634_MODE1_SUB3|PCA9634_MODE1_ALL;

	msgBuf[0] = (pca9634_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	msgBuf[1] = PCA9634_REG_MODE1;
	msgBuf[2] = data;
	TWI_Start_Read_Write( msgBuf, 3 );
	while ( TWI_Transceiver_Busy() );
}

/*---------------------------------------------------------------------------*/

void pca9634_setDuty(uint8_t id, uint8_t step)
{
	/* set register PWM[id] to step */
	if (id < 8)
	{
		unsigned char msgBuf[5];
		msgBuf[0] = (pca9634_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		msgBuf[1] = PCA9634_REG_PWM0+id;
		msgBuf[2] = step;
		TWI_Start_Read_Write( msgBuf, 3 );
		while ( TWI_Transceiver_Busy() );
	}
}

/*---------------------------------------------------------------------------*/

void pca9634_setOpMode(uint8_t id, pca9634_OpMode_t mode)
{
	if (id < 8)
	{
		unsigned char msgBuf[5];
		/*  */
		uint8_t reg;
		uint8_t shift = (id&0b11)<<1;
		uint8_t data;
		if (id < 4)
		{
			reg = PCA9634_REG_LEDOUT0;
			pca9634_LEDOUT0 = pca9634_LEDOUT0|(mode<<shift);
			data = pca9634_LEDOUT0;
		}
		else if (id < 8)
		{
			reg = PCA9634_REG_LEDOUT1;
			pca9634_LEDOUT1 = pca9634_LEDOUT1|(mode<<shift);
			data = pca9634_LEDOUT1;
		}
	
		msgBuf[0] = (pca9634_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		msgBuf[1] = reg;
		msgBuf[2] = data;
		TWI_Start_Read_Write( msgBuf, 3 );
		while ( TWI_Transceiver_Busy() );
	}
}

