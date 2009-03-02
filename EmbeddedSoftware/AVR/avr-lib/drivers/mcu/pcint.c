/**
 * ...
 * 
 * @author	Linus Lundin
 * 
 * @date	2009-03-01
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <drivers/mcu/pcint.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/

struct {
	uint8_t pcintBit;
	//uint8_t lastStatus;
	pcintCallback_t callback;
} pcints[PCINT_NUM_PCINTS];

uint32_t pinLastStatus;

/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *---------------------------------------------------------------------------*/

/*********************************************************************//**
Function: ISR(PCINT0_vect)
Purpose:  Executed when pin change on PCINT0.
Input:    -
Returns:  -
**************************************************************************/
ISR(PCINT0_vect) {
	uint8_t p;

	uint32_t pin = ((uint32_t)PIND<<16)|((uint32_t)PINC<<8)|(PINB);
	/* Check which pin that have had a change and call the callback.*/
	//printf("I\n");
	for (p = 0; p < PCINT_NUM_PCINTS; p++)
	{
		if (pinLastStatus != ((pin & (1 << pcints[p].pcintBit)) != 0)) {
				pcints[p].callback(p);
		}
		
		/*if (pcints[p].pcintBit < 8) {
			if (pcints[p].lastStatus != ((PINB & (1 << pcints[p].pcintBit)) != 0)) {
				pcints[p].lastStatus = ((PINB & (1 << pcints[p].pcintBit)) != 0);
				pcints[p].callback(p);
			}
		} else if (pcints[p].pcintBit < 16) {
			if (pcints[p].lastStatus != ((PINC & (1 << pcints[p].pcintBit%8)) != 0)) {
				pcints[p].lastStatus = ((PINC & (1 << pcints[p].pcintBit%8)) != 0);
				pcints[p].callback(p);
			}
		} else if (pcints[p].pcintBit < 24) {
			if (pcints[p].lastStatus != ((PIND & (1 << pcints[p].pcintBit%8)) != 0)) {
				pcints[p].lastStatus = ((PIND & (1 << pcints[p].pcintBit%8)) != 0);
				pcints[p].callback(p);
			}
		}*/
	}
	
	pinLastStatus = pin;
} /* ISR(PCINT0_vect) */


/*********************************************************************//**
Function: ISR(PCINT1_vect)
Purpose:  Executed when pin change on PCINT1.
Input:    -
Returns:  -
**************************************************************************/
#if ((__GNUC__ == 4  && __GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ > 1)||(__GNUC__ == 4  && __GNUC_MINOR__ > 2)||__GNUC__ > 4)
ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
#else
ISR_ALIAS(PCINT1_vect, PCINT0_vect);
#endif
/*********************************************************************//**
Function: ISR(PCINT0_vect)
Purpose:  Executed when pin change on PCINT2.
Input:    -
Returns:  -
**************************************************************************/
#if ((__GNUC__ == 4  && __GNUC_MINOR__ == 2 && __GNUC_PATCHLEVEL__ > 1)||(__GNUC__ == 4  && __GNUC_MINOR__ > 2)||__GNUC__ > 4)
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
#else
ISR_ALIAS(PCINT2_vect, PCINT0_vect);
#endif
/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

void Pcint_SetCallback(uint8_t pcint_id, uint8_t pcintBit, pcintCallback_t callback)
{
	//printf("Test\n");
	if (pcint_id<PCINT_NUM_PCINTS)
	{
		uint8_t sreg = SREG;
		cli();

		PCIFR=(1<<PCIF0|1<<PCIF1|1<<PCIF2);	/* clear any pending interrupt before enabling interrupts */
		PCICR|=(1<<PCIE0|1<<PCIE1|1<<PCIE2);	/* enable interrupt for PCINT0-2 */
#if 0
		PCIFR=(1<<PCIF1);	/* clear any pending interrupt before enabling interrupts */
		PCICR|=(1<<PCIE1);	/* enable interrupt for PCINT1 */
		PCIFR=(1<<PCIF2);	/* clear any pending interrupt before enabling interrupts */
		PCICR|=(1<<PCIE2);	/* enable interrupt for PCINT2 */
#endif
		pcints[pcint_id].pcintBit = pcintBit;
		//pcints[pcint_id].lastStatus = 0;
		if (callback != 0) {
			if (pcints[pcint_id].pcintBit < 8) {
				//pcints[pcint_id].lastStatus = ((PINB & (1 << pcints[pcint_id].pcintBit%8)) != 0);
				PCMSK0 |= (1<<pcints[pcint_id].pcintBit%8);
//printf("PCMSK %u\n", PCMSK0);
			} else if (pcints[pcint_id].pcintBit < 16) {
				//pcints[pcint_id].lastStatus = ((PINC & (1 << pcints[pcint_id].pcintBit%8)) != 0);
				PCMSK1 |= (1<<pcints[pcint_id].pcintBit%8);
//printf("PCMSK1 %u\n", PCMSK1);
			} else if (pcints[pcint_id].pcintBit < 24) {
				//pcints[pcint_id].lastStatus = ((PIND & (1 << pcints[pcint_id].pcintBit%8)) != 0);
				PCMSK2 |= (1<<pcints[pcint_id].pcintBit%8);
//printf("PCMSK2 %u\n", PCMSK2);
			}
		} else { //turn off interupt
			if (pcints[pcint_id].pcintBit < 8) {
				PCMSK0 &= ~(1<<(pcints[pcint_id].pcintBit%8));
			} else if (pcints[pcint_id].pcintBit < 16) {
				PCMSK1 &= ~(1<<(pcints[pcint_id].pcintBit%8));
			} else if (pcints[pcint_id].pcintBit < 24) {
				PCMSK2 &= ~(1<<(pcints[pcint_id].pcintBit%8));
			}
		}

		uint32_t pinLastStatus = ((uint32_t)PIND<<16)|((uint32_t)PINC<<8)|(PINB);

		pcints[pcint_id].callback = callback;
		SREG = sreg;
	}
}

/*---------------------------------------------------------------------------*/

