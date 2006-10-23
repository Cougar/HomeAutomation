/*********************************************************************
 *
 *                  Ticker service
 *
 *********************************************************************
 * FileName:        Tick.c
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#define TICK_INCLUDE

#include <Tick.h>
#include <Compiler.h>

#define TICK_TEMP_VALUE_1 ((INSTR_FREQ) / (TICKS_PER_SECOND * TICK_PRESCALE_VALUE))

#if TICK_TEMP_VALUE_1 > 60000
#error TICK_PER_SECOND value cannot be programmed with current CLOCK_FREQ
#error Either lower TICK_PER_SECOND or manually configure the Timer
#endif

#define TICK_TEMP_VALUE         (65535 - TICK_TEMP_VALUE_1)

#define TICK_COUNTER_HIGH       ((TICK_TEMP_VALUE >> 8) & 0xff)
#define TICK_COUNTER_LOW        (TICK_TEMP_VALUE & 0xff)

#if (TICK_PRESCALE_VALUE == 2)
    #define TIMER_PRESCALE  (0)
#elif ( TICK_PRESCALE_VALUE == 4 )
    #define TIMER_PRESCALE  (1)
#elif ( TICK_PRESCALE_VALUE == 8 )
    #define TIMER_PRESCALE  (2)
#elif ( TICK_PRESCALE_VALUE == 16 )
    #define TIMER_PRESCALE  (3)
#elif ( TICK_PRESCALE_VALUE == 32 )
    #define TIMER_PRESCALE  (4)
#elif ( TICK_PRESCALE_VALUE == 64 )
    #define TIMER_PRESCALE  (5)
#elif ( TICK_PRESCALE_VALUE == 128 )
    #define TIMER_PRESCALE  (6)
#elif ( TICK_PRESCALE_VALUE == 256 )
    #define TIMER_PRESCALE  (7)
#else
    #error Invalid TICK_PRESCALE_VALUE specified.
#endif


TICK TickCount = 0;	// 10ms/unit



/*
*	Function: TickInit
*
*	Input:	none
*	Output: Tick manager is initialized.
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/
#define PERIOD (INSTR_FREQ/256/1000)*TICK_PERIOD_MS
void tickInit(void)
{
    // Start the timer.
    TMR0L = TICK_COUNTER_LOW;
    TMR0H = TICK_COUNTER_HIGH;

    // 16-BIT, internal timer, PSA set to 1:256
    T0CON = 0b00000000 | TIMER_PRESCALE;

    // Start the timer.
    T0CONbits.TMR0ON = 1;

    INTCONbits.TMR0IF = 1;
    INTCONbits.TMR0IE = 1;
}

/*
*	Function: TickGet
*
*	Input:	none
*	Output: Current tick value is given, 1 tick represents approximately 10ms
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/
TICK tickGet(void)
{
    return TickCount;
}


/*
*	Function: tickUpdate
*
*	Input:	none
*	Output: none
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/
void tickUpdate(void)
{
    if(INTCONbits.TMR0IF)
    {
        TMR0H = TICK_COUNTER_HIGH;
        TMR0L = TICK_COUNTER_LOW;

        TickCount++;

        INTCONbits.TMR0IF = 0;
    }
}
