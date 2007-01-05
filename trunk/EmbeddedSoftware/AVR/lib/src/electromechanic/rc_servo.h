#ifndef RC_SERVO_H_
#define RC_SERVO_H_

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

/*-----------------------------------------------
 * Defines
 * --------------------------------------------*/

/*
 * If using both OC1A and OC1B set DOUBLE_PWM
 * On NodeGeneric it's only possible to use OC1A
 */
#define DOUBLE_PWM 0

/* 
 * For width of the pulse that control the RC servo.
 * Clock pulses.
 * This is adjusted for a 8 MHz clock, to use different speeds recalculate.
 */
#define PWM_MAX_PULSE   1000
#define PWM_MIN_PULSE   2000
#define PWM_CENTER_PULSE ((PWM_MAX_PULSE - PWM_MIN_PULSE)/2 + PWM_MIN_PULSE)
#define STEP            ((PWM_MAX_PULSE - PWM_MIN_PULSE)/256)

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void rcServoInit();
void setPosition(int8_t abs_pos);
void alterPosition(int8_t rel_pos);
int8_t getPosition();

#endif
