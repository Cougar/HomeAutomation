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
 * It is possible to use up to three PWM outputs.
 * In order: OC1A, OC0A and OC0B.
 * They will be named RCS0-RCS2.
 * Define how many that will be used.
 */
#define NUMBER_OF_RCS 2

/* 
 * For width of the pulse that control the RC servo.
 * Clock pulses.
 * This is adjusted for a 8 MHz clock, to use different speeds recalculate.
 */
#define PWM1_MIN_PULSE   800 // 1000
#define PWM1_MAX_PULSE   2200 // 2000
#define PWM2_MIN_PULSE   20
#define PWM2_MAX_PULSE   50
#define PWM3_MIN_PULSE   20
#define PWM3_MAX_PULSE   50

#define STEP1   ((PWM1_MAX_PULSE - PWM1_MIN_PULSE)/256)
#define STEP2   1
#define STEP3   1

#define MAX_POSITION 0
#define MIN_POSITION 255

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void rcServoInit();
void setPosition(uint8_t abs_pos, uint8_t servo);
void alterPosition(int8_t rel_pos, uint8_t servo);
uint8_t getPosition(uint8_t servo);

#endif
