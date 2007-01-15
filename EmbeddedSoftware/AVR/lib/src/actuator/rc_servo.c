/*
 * PWM output to control a RC servo
 *
 * It is possible to use both OCR1A and OCR1B, but they uses the same value.
 * If it's desired to control A and B individually some rewriting is necessary.
 *
 * Author: Erik Larsson
 * Date: 2007-01-04
 *
 * TODO anpassa för fler hastigheter än 8 MHz
 * testa libbet, det fungerar ännu bara i eqlazers huvud
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
#if DOUBLE_PWM
    TCCR1A |= (1<<COM1A1) // set OC1A/B at TOP
        | (1<<COM1B1) // clear OC1A/B when match
        | (1<<WGM11); // mode 14 (fast PWM, clear TCNT1 on match ICR1)
    TCCR1B |= (1<<WGM13)
        | (1<<WGM12)
        | (1<<CS11); // timer uses main system clock with 1/8 prescale
    ICR1 = 20000; // used for TOP, makes for 50 hz PWM
    OCR1A = 1500; // servo at center
    OCR1B = 1500; // servo at center
    DDRB |= (1<<PB1)
        | (1<<PB2);  // have to set up pins as outputs
#else
    TCCR1A |= (1<<COM1A1) // set OC1A/B at TOP
        // clear OC1A/B when match
        | (1<<WGM11); // mode 14 (fast PWM, clear TCNT1 on match ICR1)
    TCCR1B |= (1<<WGM13)
        | (1<<WGM12)
        | (1<<CS11); // timer uses main system clock with 1/8 prescale
    ICR1 = 20000; // used for TOP, makes for 50 hz PWM
    OCR1A = 1500; // servo at center
    DDRB |= (1<<PB1);  // have to set up pins as outputs
#endif
}

/*
 * Set the servo to an absolute position.
 * Interval -128 to 127.
 */
void setPosition(int8_t abs_pos){
    uint16_t temp_pos;
    temp_pos = (uint16_t)PWM_CENTER_PULSE + abs_pos*STEP;
#if DOUBLE_PWM
    OCR1A = temp_pos;
    OCR1B = temp_pos;
#else
    OCR1A = temp_pos;
#endif
}

/*
 * Alter the position rel_pos steps.
 * Interval -128 to 127.
 */
void alterPosition(int8_t rel_pos){
    int8_t new_pos, current_pos;

    current_pos = getPosition();
    new_pos = current_pos + rel_pos;

    if(rel_pos<0 && current_pos<0 && new_pos>0 ){
        new_pos = MIN_POSITION;
    }else if(rel_pos>0 && current_pos>0 && new_pos<0 ){
        new_pos = MAX_POSITION;
    }

    setPosition( new_pos );
}

/*
 * Get current position of the servo.
 */
int8_t getPosition(){

    return (int8_t)((int16_t)(OCR1A - PWM_CENTER_PULSE)/STEP);
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
