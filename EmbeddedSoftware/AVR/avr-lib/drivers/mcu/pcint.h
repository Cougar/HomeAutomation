/** 
 * @defgroup pcint PCINT Library
 * @code #include <drivers/mcu/pcint.h> @endcode
 * 
 * @brief Functions for pcint usage. 
 *
 * ...
 *
 * @author	Linus Lundin
 * @date	2009-03-01
 */

/**@{*/

#ifndef PCINT_H_
#define PCINT_H_

#include <config.h>
#ifndef PCINT_NUM_PCINTS
#	error PCINT_NUM_PCINTS is not defined! Edit config.inc.
#endif

#if PCINT_NUM_PCINTS > 0

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * @brief Definition for ...
 * 
 * When passed as the type argument to ...
 */

/*-----------------------------------------------------------------------------
 * Public Types
 *---------------------------------------------------------------------------*/

/**
 * @brief Type of the callback function pointer
 */
typedef void (*pcintCallback_t)(uint8_t, uint8_t);

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/

/**
 * @brief Initializes a pin-change interupt and activate callbacks. 
 * 
 * @param pcint_id
 * 		The PCINT id (0<=pcint_id<PCINT_NUM_PCINTS).
 * @param pcintBit
 * 		...
 * @param callback
 * 		Pointer to optional callback function on the form 
 * 		void callback(uint8_t pcint_id). The callback function is called with the
 * 		pcint number as the argument whenever the pcint is thrown. It is called
 * 		from the interrupt handler and must be very short. Null if not used.
 */ 
void Pcint_SetCallback(uint8_t pcint_id, uint8_t pcintBit, pcintCallback_t callback);


/**
 * @brief Initializes a pin-change interupt and activate callbacks. 
 * 
 * @param pcint_id
 * 		The PCINT id (0<=pcint_id<PCINT_NUM_PCINTS).
 * @param allInTheMiddle
 * 		Use the standard pin definitions in GPIO
 * @param callback
 * 		Pointer to optional callback function on the form 
 * 		void callback(uint8_t pcint_id). The callback function is called with the
 * 		pcint number as the argument whenever the pcint is thrown. It is called
 * 		from the interrupt handler and must be very short. Null if not used.
 */ 
static inline __attribute__ ((always_inline)) void Pcint_SetCallbackPin(uint8_t pcint_id, volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint, pcintCallback_t callback) {Pcint_SetCallback(pcint_id, pcint, *callback);}

#endif	/*PCINT_NUM_PCINTS > 0*/
/**@}*/
#endif /*PCINT_H_*/
