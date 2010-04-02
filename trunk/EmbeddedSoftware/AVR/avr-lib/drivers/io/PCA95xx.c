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
	/* Disable intterupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();

	/* Store address */
	Pca95xx_address = address;
	
	/* Set up interrupt */
	
	/* Set up ports? */
	
	/* Read direction registers from device to global var */
	//TODO: communication with device
	//Pca95xx_direction = ;
	/* Read output registers from device to global var */
	//TODO: communication with device
	//Pca95xx_outputs = ;
	
	/* Enable interrupt */
	PCA_INT_ENABLE();
	sei();
}

/*---------------------------------------------------------------------------*/

#if PCA95XX_NUM_CALLBACKS > 0
void Pca95xx_SetCallback(uint8_t index, pca95xxCallback_t callback)
{
	/* Disable intterupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();

	if (index < PCA95XX_NUM_CALLBACKS)
	{
		/* Store the callback */
		Pca95xx_callback[index] = callback;
	}

	sei();
}
#endif

/*---------------------------------------------------------------------------*/

uint16_t Pca95xx_GetInputs(void)
{
	/* Disable device interupts while communicating with the device */
	PCA_INT_DISABLE();

	/* Read input registers from device */
	//TODO: communication with device
	uint16_t inputs = ;
	
	PCA_INT_ENABLE();
	return inputs;
}

/*---------------------------------------------------------------------------*/

void Pca95xx_SetOutputs(uint16_t outputs, uint16_t mask)
{
	/* Disable intterupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();
	
	/* Set masked ones in outputs to global var */
	Pca95xx_outputs |= (outputs&mask);
	/* Clear masked zeros in outputs to global var */
	Pca95xx_outputs &= (outputs|(~mask));
	/* Set output registers */
	//TODO: communication with device
	sei();
}

/*---------------------------------------------------------------------------*/

void Pca95xx_SetDirection(uint16_t direction, uint16_t mask)
{
	/* Disable intterupts while tampering with the global vars */
	uint8_t sreg = SREG;
	cli();

	/* Set masked ones in direction to global var */
	Pca95xx_direction |= (direction&mask);
	/* Clear masked zeros in direction to global var */
	Pca95xx_direction &= (direction|(~mask));
	/* Set direction registers */
	//TODO: communication with device
	sei();
}
