#ifndef PID_AUTOTUNE_H
#define PID_AUTOTUNE_H
#define AUTOTUNE_LIBRARY_VERSION 0.0.2

#include <inttypes.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

// verbose debug option
// requires open Serial port
#undef AUTOTUNE_DEBUG

#undef USE_SIMULATION

// defining this option implements relay bias
// this is useful to adjust the relay output values
// during the auto tuning to recover symmetric
// oscillations 
// this can compensate for load disturbance
// and equivalent signals arising from nonlinear
// or non-stationary processes 
// any improvement in the tunings seems quite modest 
// but sometimes unbalanced oscillations can be 
// persuaded to converge where they might not 
// otherwise have done so
#undef AUTOTUNE_RELAY_BIAS

// average amplitude of successive peaks must differ by no more than this proportion
// relative to half the difference between maximum and minimum of last 2 cycles
#define AUTOTUNE_PEAK_AMPLITUDE_TOLERANCE 0.05

// ratio of up/down relay step duration should differ by no more than this tolerance
// biasing the relay con give more accurate estimates of the tuning parameters but
// setting the tolerance too low will prolong the autotune procedure unnecessarily
// this parameter also sets the minimum bias in the relay as a proportion of its amplitude
#define AUTOTUNE_STEP_ASYMMETRY_TOLERANCE 0.20

// auto tune terminates if waiting too long between peaks or relay steps
// set larger value for processes with long delays or time constants
#define AUTOTUNE_MAX_WAIT_MINUTES 5

// Ziegler-Nichols type auto tune rules
// in tabular form
struct Tuning
{
  uint8_t divisor[3];
  /*
  bool PI_controller()
  {
    return pgm_read_uint8_t_near(&_divisor[2]) == 0;
  }
  
  float divisor(uint8_t index)  
  {
    return (float)pgm_read_uint8_t_near(&_divisor[index]) * 0.05;
  }
  */
};



  // constants ***********************************************************************************

  // auto tune method
  enum PID_AutoTune_Methods
  {
    ZIEGLER_NICHOLS_PI = 0,	
    ZIEGLER_NICHOLS_PID = 1,
    TYREUS_LUYBEN_PI,
    TYREUS_LUYBEN_PID,
    CIANCONE_MARLIN_PI,
    CIANCONE_MARLIN_PID,
    AMIGOF_PI,
    PESSEN_INTEGRAL_PID,
    SOME_OVERSHOOT_PID,
    NO_OVERSHOOT_PID
  };

  // peak type
  enum Peak
  {
    MINIMUM = -1,
    NOT_A_PEAK = 0,
    MAXIMUM = 1
  };

  // auto tuner state
  enum AutoTunerState
  {
    AUTOTUNER_OFF = 0, 
    STEADY_STATE_AT_BASELINE = 1,
    STEADY_STATE_AFTER_STEP_UP = 2,
    RELAY_STEP_UP = 4,
    RELAY_STEP_DOWN = 8,
    CONVERGED = 16,
    FAILED = 128
  }; 

  // tuning rule divisor
  enum PID_Constant_Devisors
  {
    KP_DIVISOR = 0,
    TI_DIVISOR = 1,
    TD_DIVISOR = 2
  };

  

  
  // commonly used methods ***********************************************************************
  void PID_ATune_Init(float* Input, float* Output);          // * Constructor.  links the Autotune to a given PID
  bool PID_ATune_Runtime(void);                       // * Similar to the PID Compute function, 
                                        //   returns true when done, otherwise returns false
  void PID_ATune_Cancel(void);                        // * Stops the AutoTune 

  void PID_ATune_SetOutputStep(float);           // * how far above and below the starting value will 
                                        //   the output step?   
  float PID_ATune_GetOutputStep(void);               // 

  void PID_ATune_SetControlType(uint8_t);            // * Determines tuning algorithm
  uint8_t PID_ATune_GetControlType(void);                // * Returns tuning algorithm

  void PID_ATune_SetLookbackSec(uint16_t);             // * how far back are we looking to identify peaks
  int PID_ATune_GetLookbackSec(void);                 //

  void PID_ATune_SetNoiseBand(float);            // * the autotune will ignore signal chatter smaller 
                                        //   than this value
  float PID_ATune_GetNoiseBand(void);                //   this should be accurately set

  float PID_ATune_GetKp(void);                       // * once autotune is complete, these functions contain the
  float PID_ATune_GetKi(void);                       //   computed tuning parameters.  
  float PID_ATune_GetKd(void);                       //

#endif
