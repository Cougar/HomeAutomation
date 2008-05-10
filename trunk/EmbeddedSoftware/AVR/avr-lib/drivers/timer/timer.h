/** 
 * @defgroup timer Timer Library
 * @code #include <drivers/timer/timer.h> @endcode
 * 
 * @brief Functions for simple and advanced timer usage. 
 *
 * Timer software module. Utilizes a hardware timer to provide a number of
 * software timers to the application. Each timer can be set to either one-shot
 * or free-running mode. Timer events can be defined by polling or callbacks. 
 *
 * @author	Andreas Fritiofson
 * @date	2007-07-13
 */

/**@{*/

#ifndef TIMER_H_
#define TIMER_H_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * @brief Definition for timer type TimerTypeOneShot
 * 
 * When passed as the type argument to timerSetTimeout, the 
 * timer will trigger only once and then disable itself.
 */
#define TimerTypeOneShot		0

/**
 * @brief Definition for timer type TimerTypeFreeRunning
 * 
 * When passed as the type argument to timerSetTimeout, 
 * the timer will reload itself at every expiration and trigger periodically.
 */
#define TimerTypeFreeRunning	1


/*-----------------------------------------------------------------------------
 * Public Types
 *---------------------------------------------------------------------------*/

/**
 * @brief Type of the callback function pointer
 */
typedef void (*timerCallback_t)(uint8_t);

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/

/**
 * @brief Initialize timer engine
 * 
 * Initializes the timer engine. The hardware timer interrupt source will be
 * enabled, but global interrupts need to be enabled by the application.
 */
void Timer_Init(void);

/**
 * @brief Get current timer value
 * 
 * @retval
 * 		The current time in ticks. The tick count wraps around after 2^32
 * 		ticks without any notification.
 */
uint32_t Timer_GetTicks(void);

/**
 * @brief Initializes a software timer and starts it. 
 * 
 * @param timer
 * 		The timer (0<=timer<TIMER_NUM_TIMERS).
 * @param timeout
 * 		The timeout in ticks. Passing zero disables a running timer. A timeout
 * 		value of X ticks gives an actual timeout of between (X-1) and X ticks,
 * 		not including the time it takes for the application to check the
 * 		expiration status of the timer.
 * @param type
 * 		TimerTypeOneShot or TimerTypeFreeRunning.
 * @param callback
 * 		Pointer to optional callback function on the form 
 * 		void callback(uint8_t timer). The callback function is called with the
 * 		timer number as the	argument whenever the timer expires. It is called
 * 		from the interrupt handler and must be very short. Null if not used.
 */ 
void Timer_SetTimeout(uint8_t timer, uint16_t timeout, uint8_t type, timerCallback_t callback);

/**
 * @brief Check if timer expired. 
 * 
 * Checks if a timer has expired, should be used for polling a timer event if callback 
 * is not used. The timer will only signal its expiration at
 * the first call to this function after a timeout period, even if multiple
 * timeout periods have elapsed since the previous call.
 * 
 * @param timer
 * 		The timer (0<=timer<TIMER_NUM_TIMERS).
 * @retval
 * 		Non-zero if timer has expired since the previous check.
 */ 
uint8_t Timer_Expired(uint8_t timer);

/**@}*/
#endif /*TIMER_H_*/
