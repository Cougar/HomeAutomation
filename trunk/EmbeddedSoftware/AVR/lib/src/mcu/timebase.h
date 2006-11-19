#ifndef timebase_h_
#define timebase_h_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define TIMEBASE_PRESCALE (64)
#define TIMEBASE_HITS_PER_1MS (F_OSC/TIMEBASE_PRESCALE/1000)
#define TIMEBASE_RELOAD ((uint8_t)(0xff-TIMEBASE_HITS_PER_1MS+1))


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void Timebase_Init(void);
uint32_t Timebase_CurrentTime(void);
uint32_t Timebase_PassedTimeMillis(uint32_t t0);


#endif
