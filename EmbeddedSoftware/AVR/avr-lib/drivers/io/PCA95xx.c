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

	/* Disable interrupts while tampering with the global vars */
//	uint8_t sreg = SREG;
//	cli();

	/* Store address */
	Pca95xx_address = (PCA95xx_I2C_DEV_ADDR|(address&7));
	
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
		Pca95xx_outputs = (messageBuf[2]<<8)|messageBuf[1];
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
		Pca95xx_direction = (messageBuf[2]<<8)|messageBuf[1];
	}
	
	/* Enable interrupt */
#if PCA95XX_NUM_CALLBACKS > 0
	PCA_INT_ENABLE();
#endif
//	SREG = sreg;
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
	uint8_t sreg = SREG;
	cli();
	
	/* Set masked ones in outputs to global var */
	Pca95xx_outputs |= (outputs&mask);
	/* Clear masked zeros in outputs to global var */
	Pca95xx_outputs &= (outputs|(~mask));
	/* Set output registers */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_OUTPUT0;
	messageBuf[2] = outputs&0xff;
	messageBuf[3] = (outputs>>8)&0xff;
	TWI_Start_Read_Write( messageBuf, 4 );
	while ( TWI_Transceiver_Busy() );

	SREG = sreg;
}

/*---------------------------------------------------------------------------*/

void Pca95xx_SetDirection(uint16_t direction, uint16_t mask)
{
	unsigned char messageBuf[5];

	/* Disable interrupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();

	/* Set masked ones in direction to global var */
	Pca95xx_direction |= (direction&mask);
	/* Clear masked zeros in direction to global var */
	Pca95xx_direction &= (direction|(~mask));
	/* Set direction registers */
	messageBuf[0] = (Pca95xx_address<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = PCA95xx_REG_CONF0;
	messageBuf[2] = direction&0xff;
	messageBuf[3] = (direction>>8)&0xff;
	TWI_Start_Read_Write( messageBuf, 4 );
	while ( TWI_Transceiver_Busy() );
	SREG = sreg;
}
