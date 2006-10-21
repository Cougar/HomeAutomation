#ifndef timebase_h_
#define timebase_h_

#include <inttypes.h>

// timebase on timer0 serves a 10ms tick
// for 8MHz System and Prescaler 64
#define TIMEBASE_PRESCALE (64)
#define TIMEBASE_HITS_PER_1MS ( F_OSC/TIMEBASE_PRESCALE/1000 )
#define TIMEBASE_RELOAD ((uint8_t)(0xff-TIMEBASE_HITS_PER_1MS+1))

volatile uint16_t gMilliSecTick;

void timebase_init(void);
uint16_t timebase_actTime(void);
uint16_t timebase_passedTimeMS(uint16_t t0);

#endif
