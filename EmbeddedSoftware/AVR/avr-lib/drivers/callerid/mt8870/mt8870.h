#ifndef MT8870_H_
#define MT8870_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/*
 * Signals connected from MT8870 is
 * StD, Q1, Q2, Q3 och Q4
 */
#define MT_StD_PIN      PIND
#define MT_StD_DDR      DDRD
#define MT_StD_BIT      PD2

#define MT_Q1_PIN       PINC
#define MT_Q1_DDR       DDRC
#define MT_Q1_BIT       PC0

#define MT_Q2_PIN       PINC
#define MT_Q2_DDR       DDRC
#define MT_Q2_BIT       PC1

#define MT_Q3_PIN       PINC
#define MT_Q3_DDR       DDRC
#define MT_Q3_BIT       PC2

#define MT_Q4_PIN       PINC
#define MT_Q4_DDR       DDRC
#define MT_Q4_BIT       PC3

/* Time to elapse to know when number sequence is over */
#define DIAL_TIMEOUT	1800

/* Maximum number of tones to store for the number */
#define MAX_TONES		16

/* Returnvalues */
#define RET_NOT_FINISHED	0
#define RET_FINISHED		1
#define RET_OVERFLOW		2

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void DTMFin_Init(void);
uint8_t DTMFin_Poll(uint8_t *len);
void DTMFin_Start(uint8_t *buffer);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
void DTMFin_timer_callback(uint8_t timer);

#endif /*MT8870_H_*/
