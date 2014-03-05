/**********************************************************************************************
 * C PID Library - Version 1.0.1
 * modified by Linus Lundin <linus.lundin@gmail.com (ported c to be used with AVR-GCC)
 * originally by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#include "PID_v1.h"
#include <drivers/timer/timer.h>
void PID_Initialize(PidType* pid);
void PID_Limit(PidType* pid, FloatType *var);

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up 
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
void PID_init(PidType* pid, FloatType* Input, FloatType* Output, FloatType* Setpoint, FloatType Kp, FloatType Ki, FloatType Kd,
    PidDirectionType ControllerDirection) {


  PID_SetOutputLimits(pid, 0, 0xffff);

  //default Controller Sample Time is 0.1 seconds
  pid->SampleTime = 100;

  PID_SetControllerDirection(pid, ControllerDirection);
  PID_SetTunings(pid, Kp, Ki, Kd);

  pid->lastTime = Timer_GetTicks() - pid->SampleTime;
  pid->mode = PID_Mode_Manual;
  pid->myInput = Input;
  pid->myOutput = Output;
  pid->mySetpoint = Setpoint;
  pid->MaxKd = 10.0;
}

/* Compute() **********************************************************************
 *   This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/ 
bool PID_Compute(PidType* pid)
{
  if (pid->mode == PID_Mode_Manual) 
  {
    return false;
  }
  unsigned long now = Timer_GetTicks();
  unsigned long timeChange = (now - pid->lastTime);
  if (timeChange >= pid->SampleTime)
  {
    // Compute all the working error variables
    FloatType input = *pid->myInput;

    // compute pid_t Output
    FloatType error = *pid->mySetpoint - input;
    FloatType dInput = (input - pid->lastInput);

    pid->PTerm = pid->kp * error; 
    pid->ITerm += ( pid->ki * error );
    PID_Limit(pid, &pid->ITerm);
    pid->DTerm = - pid->kd * dInput / ( pid->MaxKd < MAX_KD_MIN ? 1.0 : 1.0 + pid->kd / pid->MaxKd );

    // compute PID Output
    FloatType output = pid->PTerm + pid->ITerm + pid->DTerm;
    PID_Limit(pid, &output);
    *pid->myOutput = output;

    // remember some variables for next time
    pid->lastInput = input;
    pid->lastTime = now;
    return true;
  }
  else 
  {
    return false;
  }
}

/* Limit(...)******************************************************************
 * bound supplied variable to (outMin, outMax)
 ******************************************************************************/
void PID_Limit(PidType* pid, FloatType *var)
{
  if (*var < pid->outMin)
  {
    *var = pid->outMin;
  }
  else if (*var > pid->outMax)
  {
    *var = pid->outMax;
  }
}

/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted. 
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/
void PID_SetTunings(PidType* pid, FloatType Kp, FloatType Ki, FloatType Kd)
{
  if ((Kp < 0.0) || (Ki < 0.0) || (Kd < 0.0)) 
  {
    return;
  }

  pid->dispKp = Kp;
  pid->dispKi = Ki;
  pid->dispKd = Kd;

  FloatType SampleTimeInSec = ((FloatType) pid->SampleTime) / 1000.0;
  pid->kp = Kp;
  pid->ki = Ki * SampleTimeInSec;
  pid->kd = Kd / SampleTimeInSec;

  if (pid->controllerDirection == PID_Direction_Reverse) {
    pid->kp = (0.0 - pid->kp);
    pid->ki = (0.0 - pid->ki);
    pid->kd = (0.0 - pid->kd);
  }
}

/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed	
 ******************************************************************************/
void PID_SetSampleTime(PidType* pid, uint32_t NewSampleTime)
{
  if (NewSampleTime > 0) {
    FloatType ratio = (FloatType) NewSampleTime / (FloatType) pid->SampleTime;
    pid->ki *= ratio;
    pid->kd /= ratio;
    pid->SampleTime = (uint32_t) NewSampleTime;
  }
}

/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void PID_SetOutputLimits(PidType* pid, FloatType newMin, FloatType newMax) {
  if (newMin >= newMax) {
    return;  
  }
  pid->outMin = newMin;
  pid->outMax = newMax;

  if (pid->mode == PID_Mode_Automatic)
  {
    PID_Limit(pid, pid->myOutput);
    PID_Limit(pid, &pid->ITerm);
  }
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/
void PID_SetMode(PidType* pid, PidModeType newMode)
{
  if (newMode != pid->mode)
  {
    // just changed mode
    PID_Initialize(pid);
    pid->mode = newMode;
  }
}

/* Initialize()****************************************************************
 *  does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/
void PID_Initialize(PidType* pid)
{
  pid->ITerm = *pid->myOutput;
  pid->lastInput = *pid->myInput;
  PID_Limit(pid, &pid->ITerm);
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads 
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID_SetControllerDirection(PidType* pid, PidDirectionType newDirection)
{
  if ((pid->mode == PID_Mode_Automatic) && (newDirection != pid->controllerDirection))
  {
    pid->kp = (0.0 - pid->kp);
    pid->ki = (0.0 - pid->ki);
    pid->kd = (0.0 - pid->kd);
  }   
  pid->controllerDirection = newDirection;
}

/* pid_set_max_Kd(...)*********************************************************
 * set maximum derivative gain
 * Default 0 means no maximum.
 ******************************************************************************/
void PID_SetMaxKd(PidType* pid, FloatType newMaxKd )
{
  if ( newMaxKd < MAX_KD_MIN ) 
  {
    // no limit to Kd
    newMaxKd = 0.0;	
  }
  pid->MaxKd = newMaxKd;
}	

/* Status Functions************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display 
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
FloatType PID_GetKp(PidType* pid) {
  return pid->dispKp;
}
FloatType PID_GetKi(PidType* pid) {
  return pid->dispKi;
}
FloatType PID_GetKd(PidType* pid) {
  return pid->dispKd;
}
FloatType PID_GetMin(PidType* pid) {
  return pid->outMin;
}
FloatType PID_GetMax(PidType* pid) {
  return pid->outMax;
}

PidModeType PID_GetMode(PidType* pid) {
  return pid->mode;
}

PidDirectionType PID_GetDirection(PidType* pid) {
  return pid->controllerDirection;
}

FloatType PID_GetPTerm(PidType* pid)
{ 
  return  pid->PTerm; 
}

FloatType PID_GetITerm(PidType* pid)
{ 
  return  pid->ITerm; 
}

FloatType PID_GetDTerm(PidType* pid)
{ 
  return  pid->DTerm; 
}

