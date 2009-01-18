/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for pid.c.
 *
 * - File:               pid.h
 * - Compiler:           IAR EWAAVR 4.11A
 * - Supported devices:  All AVR devices can be used.
 * - AppNote:            AVR221 - Discrete PID controller
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name$
 * $Revision: 456 $
 * $RCSfile$
 * $Date: 2006-02-16 12:46:13 +0100 (to, 16 feb 2006) $
 *****************************************************************************/

#ifndef PID_H
#define PID_H

#include <inttypes.h>
#define SCALING_FACTOR  128

/*! \brief PID Status
 *
 * Setpoints and data used by the PID control algorithm
 */
typedef struct PID_DATA{
  //! Last process value, used to find derivative of process value.
  float lastProcessValue;
  //! Summation of errors, used for integrate calculations
  float sumError;
  //! The Proportional tuning constant, multiplied with SCALING_FACTOR
  float P_Factor;
  //! The Integral tuning constant, multiplied with SCALING_FACTOR
  float I_Factor;
  //! The Derivative tuning constant, multiplied with SCALING_FACTOR
  float D_Factor;
  //! Maximum allowed error, avoid overflow
  float maxError;
  //! Maximum allowed sumerror, avoid overflow
  float maxSumError;
} pidData_t;


typedef struct PID_DEBUG_DATA{
  //! Last process value, used to find derivative of process value.
  float I_term;
  //! Summation of errors, used for integrate calculations
  float P_term;
  //! The Proportional tuning constant, multiplied with SCALING_FACTOR
  float D_term;
  //! The Integral tuning constant, multiplied with SCALING_FACTOR
  float Sum;
  //! The Derivative tuning constant, multiplied with SCALING_FACTOR
} pidDebugData_t;

/*! \brief Maximum values
 *
 * Needed to avoid sign/overflow problems
 */
// Maximum value of variables
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      2.0

// Boolean values
#define FALSE           0
#define TRUE            1

void pid_Init(float p_factor, float i_factor, float d_factor, struct PID_DATA *pid);
float pid_Controller(float setPoint, float processValue, struct PID_DATA *pid_st, struct PID_DEBUG_DATA *pidDebugData );
void pid_Reset_Integrator(pidData_t *pid_st);

#endif
