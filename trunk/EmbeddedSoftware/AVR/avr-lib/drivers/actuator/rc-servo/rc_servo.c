/*
 * PWM output to control a RC servo
 *
 * It is possible to use OC0A, OC0B and OC1A. The others are not aviable because
 * of the pins is used to interface the mcp2515.
 *
 * Author: Erik Larsson
 * Date: 2007-01-04
 *
 * TODO anpassa för fler hastigheter än 8 MHz
 */

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <rc_servo.h>


/*----------------------------------------------
 * Functions
 * --------------------------------------------*/

/* 
 * Initiate PWM output.
 */
void rcServoInit(){
// set up counter 1 (16 bit) to act as a dual channel PWM generator
// we want OC1A and B to be set on reaching BOTTOM, clear on reaching
// compare match, use ICR1 as TOP and have a prescale of 8.

#if NUMBER_OF_RCS >= 1
	/* Use OC1A */
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11); /* Timer uses main system clock with 1/8 prescale */
	ICR1 = 20000; /* Used for TOP, makes for 50 Hz PWM */
	OCR1A = PWM1_MIN_PULSE; /* Servo at min position */
	DDRB |= (1<<DDB1); /* Enable pin as output */
#endif
#if NUMBER_OF_RCS >= 2
	/* Use OC0A */
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); /* Set OC0A at TOP, Mode 7 */
	TCCR0B |= (1<<CS02); /* Prescaler 1/256 */
	OCR0A = PWM2_MIN_PULSE;
	DDRD |= (1<<DDD6);
#endif
#if NUMBER_OF_RCS >= 3
	/* Use OC0B */
	TCCR0A |= (1<<COM0B1);
	OCR0B = PWM3_MIN_PULSE;
	DDRD |= (1<<DDD5);
#endif

}

/*
 * Set the servo to an absolute position.
 * Interval 0 to 255.
 */
void setPosition(uint8_t abs_pos, uint8_t servo){
	uint16_t temp_pos;

	switch( servo ){
#if NUMBER_OF_RCS > 0
		case 1:
			temp_pos = PWM1_MIN_PULSE + abs_pos*STEP1;
			if(temp_pos>PWM1_MAX_PULSE){
				OCR1A = PWM1_MAX_PULSE;
			}else{
				OCR1A = temp_pos;
			}
			break;
#endif
#if NUMBER_OF_RCS > 1
		case 2:
			temp_pos = PWM2_MIN_PULSE + abs_pos*STEP2;
			if(temp_pos>PWM2_MAX_PULSE){
				OCR0A = PWM2_MAX_PULSE;
			}else{
				OCR0A = temp_pos;
			}
			break;
#endif
#if NUMBER_OF_RCS > 2
		case 3:
			temp_pos = PWM3_MIN_PULSE + abs_pos*STEP3;
			if(temp_pos>PWM3_MAX_PULSE){
				OCR0B = PWM3_MAX_PULSE;
			}else{
				OCR0B = temp_pos;
			}
			break;
#endif
    }


}

/*
 * Alter the position rel_pos steps.
 * Interval -128 to 127.
 */
 // FIXME inte ändrat här än
void alterPosition(int8_t rel_pos, uint8_t servo){
	int8_t new_pos, current_pos;

	current_pos = getPosition( servo );
	new_pos = current_pos + rel_pos;

	if(rel_pos<0 && current_pos<new_pos ){
		new_pos = MIN_POSITION;
	}else if(rel_pos>0 && current_pos>new_pos ){
		new_pos = MAX_POSITION;
	}

	setPosition( new_pos, servo );
}

/*
 * Get current position of the servo.
 */
uint8_t getPosition(uint8_t servo){
	switch( servo ){
		case 1: return ((OCR1A - PWM1_MIN_PULSE)/STEP1); break;
		case 2: return (int8_t)((OCR0A*8 - PWM2_MIN_PULSE*8)/STEP2); break;
		case 3: return (int8_t)((OCR0B*8 - PWM3_CENTER_PULSE*8)/STEP3); break;
	}
}

/*
http://members.shaw.ca/climber/avrtimers.html

 To change the servo position all you have to do is assign a value to OCR1A or B.
 With an 8 MHz clock the normal range you use is 1000 to 2000.
 I especially like it at 8 MHz because the timing for normal servo control
 is pulse widths of (usually) 1.0 to 2.0 milliseconds. Just multiply that
 by 1000 and you get the value to assign to OCR1A or B.

If you want to use a different system clock then all you have to do is 
adjust the value you assign ICR1 and the useable range to assign OCR1A 
and B to control the servos is 5% to 10% of the value you use for ICR1. 
For example, for a 1 MHz clock (like the default RC clock from the factory) 
use 2500 for ICR1 and 125 - 250 for the range of values to give OCR1A and B. 
*/
