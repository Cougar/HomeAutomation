/**
 * PCA95xx device driver. Provides possibility for several independent users of the
 * device.
 * 
 * @author	Anders Runeson
 * 
 * @date	2010-04-02
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <drivers/io/PCA95xx.h>


/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
volatile uint8_t Pca95xx_address;
volatile uint16_t Pca95xx_direction;
volatile uint16_t Pca95xx_outputs;

#if PCA95XX_NUM_CALLBACKS > 0
volatile pca95xxCallback_t[PCA95XX_NUM_CALLBACKS] Pca95xx_callback;
#endif


/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *---------------------------------------------------------------------------*/
#if PCA95XX_NUM_CALLBACKS > 0
ISR(PCA_INT_VECTOR)
{
	uint8_t t;
	/* Read input registers from device */
	//TODO: communication with device
	uint16_t inputs = ;
	
	/* Call all callbacks with the input as argument */
	for (t = 0; t < PCA95XX_NUM_CALLBACKS; t++)
	{
		Pca95xx_callback[t](inputs);
	}
}
#endif

/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

void Pca95xx_Init(uint8_t address)
{
	unsigned char messageBuf[5];
	
	/* Store address */
	Pca95xx_address = (PCA95xx_I2C_DEV_ADDR|(address&7));
	//printf("i0x%04X\n", Pca95xx_address);
	
	/* Set up interrupt */
#if PCA95XX_NUM_CALLBACKS > 0
#if PCA_INT_VECTOR==INT0_vect
	PORTD|=(1<<PORT2);	/* setup pullup */
#else
	PORTD|=(1<<PORT3);	/* setup pullup */
#endif
#endif
	/* Set up ports? */
	TWI_Master_Initialise();

	/* Read output registers from device to global var */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_OUTPUT0;
	TWI_Start_Read_Write( messageBuf, 2 );
	while ( TWI_Transceiver_Busy() );
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	TWI_Start_Read_Write( messageBuf, 3 );
	while ( TWI_Transceiver_Busy() );
	if (TWI_Read_Data_From_Buffer(messageBuf, 3) == TRUE)
	{
		/* Disable interrupts while tampering with the global vars */
		uint8_t sreg = SREG;
		cli();
		Pca95xx_outputs = (messageBuf[2]<<8)|messageBuf[1];
		SREG = sreg;
		//printf("o0x%04X\n", Pca95xx_outputs);
	}

	/* Read direction registers from device to global var */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_CONF0;
	TWI_Start_Read_Write( messageBuf, 2 );
	while ( TWI_Transceiver_Busy() );
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	TWI_Start_Read_Write( messageBuf, 3 );
	while ( TWI_Transceiver_Busy() );
	if (TWI_Read_Data_From_Buffer(messageBuf, 3) == TRUE)
	{
		/* Disable interrupts while tampering with the global vars */
		uint8_t sreg = SREG;
		cli();
		Pca95xx_direction = (messageBuf[2]<<8)|messageBuf[1];
		SREG = sreg;
		//printf("d0x%04X\n", Pca95xx_direction);
	}

	/* Enable interrupt */
#if PCA95XX_NUM_CALLBACKS > 0
	PCA_INT_ENABLE();
#endif
}

/*---------------------------------------------------------------------------*/

#if PCA95XX_NUM_CALLBACKS > 0
void Pca95xx_SetCallback(uint8_t index, pca95xxCallback_t callback)
{
	/* Disable interrupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();

	if (index < PCA95XX_NUM_CALLBACKS)
	{
		/* Store the callback */
		Pca95xx_callback[index] = callback;
	}

	SREG = sreg;
}
#endif

/*---------------------------------------------------------------------------*/

uint16_t Pca95xx_GetInputs(void)
{
	unsigned char messageBuf[5];

	/* Disable device interrupts while communicating with the device */
#if PCA95XX_NUM_CALLBACKS > 0
	PCA_INT_DISABLE();
#endif
	uint16_t inputs = 0;

	/* Read input registers from device */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_INPUT0;
	TWI_Start_Read_Write( messageBuf, 2 );
	while ( TWI_Transceiver_Busy() );
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	TWI_Start_Read_Write( messageBuf, 3 );
	while ( TWI_Transceiver_Busy() );
	if (TWI_Read_Data_From_Buffer(messageBuf, 3) == TRUE)
	{
		inputs = (messageBuf[2]<<8)|messageBuf[1];
		//printf("i0x%04X\n", inputs);
	}
	
#if PCA95XX_NUM_CALLBACKS > 0
	PCA_INT_ENABLE();
#endif
	return inputs;
}

/*---------------------------------------------------------------------------*/

void Pca95xx_SetOutputs(uint16_t outputs, uint16_t mask)
{
	unsigned char messageBuf[5];

	/* Disable interrupts while tampering with the global vars */
//	uint8_t sreg = SREG;
//	cli();

	/* Set masked ones in outputs to global var */
	Pca95xx_outputs |= (outputs&mask);
	/* Clear masked zeros in outputs to global var */
	Pca95xx_outputs &= (outputs|(~mask));
	/* Set output registers */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_OUTPUT0;
	messageBuf[2] = Pca95xx_outputs&0xff;
	messageBuf[3] = (Pca95xx_outputs>>8)&0xff;
	TWI_Start_Read_Write( messageBuf, 4 );
	while ( TWI_Transceiver_Busy() );

//	SREG = sreg;
}

/*---------------------------------------------------------------------------*/

void Pca95xx_SetDirection(uint16_t direction, uint16_t mask)
{
	unsigned char messageBuf[5];

	/* Disable interrupts while tampering with the global vars */
//	uint8_t sreg = SREG;
//	cli();

	/* Set masked ones in direction to global var */
	Pca95xx_direction |= (direction&mask);
	/* Clear masked zeros in direction to global var */
	Pca95xx_direction &= (direction|(~mask));
	/* Set direction registers */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_CONF0;
	messageBuf[2] = Pca95xx_direction&0xff;
	messageBuf[3] = (Pca95xx_direction>>8)&0xff;
	TWI_Start_Read_Write( messageBuf, 4 );
	while ( TWI_Transceiver_Busy() );
//	SREG = sreg;
}



/* GPIO.H interface */

/* Port output functions */
void Pca95xx_set_pin(uint8_t nr)
{
	Pca95xx_SetOutputs(1<<nr, 1<<nr);
}

void Pca95xx_clr_pin(uint8_t nr)
{
	Pca95xx_SetOutputs(0<<nr, 1<<nr);
}

void Pca95xx_set_statement(uint8_t statement, uint8_t nr)
{
	Pca95xx_SetOutputs(statement<<nr, 1<<nr);
}

void Pca95xx_toggle_pin(uint8_t nr)
{
	Pca95xx_SetOutputs(Pca95xx_outputs^(1<<nr), 1<<nr);
}

uint8_t Pca95xx_get_output_state(uint8_t nr)
{
	return ((Pca95xx_outputs>>nr)&0x1);
}


/* Port input functions */
uint8_t Pca95xx_get_state(uint8_t nr)
{
	return ((Pca95xx_GetInputs()>>nr)&0x1);
}


/* Direction functions */
void Pca95xx_set_in(uint8_t nr)
{
	Pca95xx_SetDirection(PCA95xx_CONF_INPUT<<nr, 1<<nr);
}

void Pca95xx_set_out(uint8_t nr)
{
	Pca95xx_SetDirection(PCA95xx_CONF_OUTPUT<<nr, 1<<nr);
}

uint8_t Pca95xx_get_direction(uint8_t nr)
{
	return ((Pca95xx_direction>>nr)&0x1);
}

