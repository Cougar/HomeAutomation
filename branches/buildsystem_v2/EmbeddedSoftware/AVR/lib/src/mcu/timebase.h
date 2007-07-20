#ifndef timebase_h_
#define timebase_h_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
/* Enable timer2 if you want to use as many PWM outputs as possible */
#define TIMER2

#define TIMEBASE_PRESCALE (64)
#define TIMEBASE_HITS_PER_1MS (F_CPU/TIMEBASE_PRESCALE/1000)

#if TIMEBASE_HITS_PER_1MS > 255
#undef TIMEBASE_PRESCALE
#define TIMEBASE_PRESCALE (256)
#undef TIMEBASE_HITS_PER_1MS
#define TIMEBASE_HITS_PER_1MS (F_CPU/TIMEBASE_PRESCALE/1000)	// behöver denna definieras om ifall prescalern definierats om?
#endif

#define TIMEBASE_RELOAD ((uint8_t)(0xff-TIMEBASE_HITS_PER_1MS+1))


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void Timebase_Init(void);
uint32_t Timebase_CurrentTime(void);
uint32_t Timebase_PassedTimeMillis(uint32_t t0);


#endif
