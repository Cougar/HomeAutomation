#ifndef RC_SERVO_H_
#define RC_SERVO_H_

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*-----------------------------------------------
 * Defines
 * --------------------------------------------*/

/* 
 * For width of the pulse that control the RC servo.
 * Milliseconds.
 */
#define PWM_MAX_PULSE 100
#define PWM_MIN_PULSE 200

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void rcServoInit();
void setPosition(int8_t abs_pos);
void alterPosition(int8_t rel_pos);
int8_t getPosition();

#endif
