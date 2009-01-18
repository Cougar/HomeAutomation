/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief General PID implementation for AVR.
 *
 * Discrete PID controller implementation. Set up by giving P/I/D terms
 * to Init_PID(), and uses a struct PID_DATA to store internal values.
 *
 * - File:               pid.c
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

#include "pid.h"
#include <inttypes.h>

/*! \brief Initialisation of PID controller parameters.
 *
 *  Initialise the variables used by the PID algorithm.
 *
 *  \param p_factor  Proportional term.
 *  \param i_factor  Integral term.
 *  \param d_factor  Derivate term.
 *  \param pid  Struct with PID status.
 */
void pid_Init(float p_factor, float i_factor, float d_factor, struct PID_DATA *pid)
// Set up PID controller parameters
{
  // Start values for PID controller
  pid->sumError = 0;
  pid->lastProcessValue = 0;
  // Tuning constants for PID loop
  pid->P_Factor = p_factor;
  pid->I_Factor = i_factor;
  pid->D_Factor = d_factor;
  // Limits to avoid overflow
  pid->maxError = MAX_INT / (pid->P_Factor + 1);
  pid->maxSumError = MAX_I_TERM;
}


/*! \brief PID control algorithm.
 *
 *  Calculates output from setpoint, process value and PID status.
 *
 *  \param setPoint  Desired value.
 *  \param processValue  Measured value.
 *  \param pid_st  PID status struct.
 */
float pid_Controller(float setPoint, float processValue, struct PID_DATA *pid_st, struct PID_DEBUG_DATA *pidDebugData )
{
  float error, p_term, d_term;
  float i_term, ret, temp;

  error = setPoint - processValue;

  // Calculate Pterm and limit error overflow
  if (error > pid_st->maxError){
    p_term = MAX_INT;
  }
  else if (error < -pid_st->maxError){
    p_term = -MAX_INT;
  }
  else{
    p_term = pid_st->P_Factor * error;
  }

  // Calculate Iterm and limit integral runaway

  temp = pid_st->sumError + error;

  if(temp > pid_st->maxSumError){
    i_term = pid_st->maxSumError*pid_st->I_Factor;
    pid_st->sumError = pid_st->maxSumError;
  }
  else if(temp < -pid_st->maxSumError){
    i_term = -pid_st->maxSumError*pid_st->I_Factor;
    pid_st->sumError = -pid_st->maxSumError;
  }
  else{
    pid_st->sumError = temp;
    i_term = pid_st->I_Factor * pid_st->sumError;
  }

  // Calculate Dterm
  d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);

  pid_st->lastProcessValue = processValue;

pidDebugData->I_term = i_term;
pidDebugData->D_term = d_term;
pidDebugData->P_term = p_term;



  ret = (p_term + i_term + d_term) / SCALING_FACTOR;

pidDebugData->Sum = ret;

  if(ret > MAX_INT){
    ret = MAX_INT;
  }
  else if(ret < -MAX_INT){
    ret = -MAX_INT;
  }

  return(ret);
}

/*! \brief Resets the integrator.
 *
 *  Calling this function will reset the integrator in the PID regulator.
 */
void pid_Reset_Integrator(pidData_t *pid_st)
{
  pid_st->sumError = 0;
}
