/** 
 * @defgroup mt8870 Mitel DTMF-receiver Library
 * @code #include <drivers/DTMF/mt8870/mt8870.h> @endcode
 * 
 * @brief Driver for MT8870 DTMF receiver
 * 
 * Driver handle complete CallerID. Init and call start.
 * When the poll function return MT8870_Ret_Finished
 * data can be found in buffer set with the start function.
 * The length pointer supplied with function Poll is used to
 * find out the length of data. 
 * 
 * @author	Anders Runeson
 * @date	2007-08-24
 */

/**@{*/

#ifndef MT8870_H_
#define MT8870_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/* Should be defined in user config.inc */
#if 0
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

#endif

/**
 * @brief Return values.
 */
typedef enum {
	MT8870_Ret_Not_Finished, 	/**< No complete number yet. */
	MT8870_Ret_Finished, 		/**< Data is available. */
	MT8870_Ret_Overflow, 		/**< Buffer was overflown. */
} MT8870_Ret_t;

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
/**
 * @brief Initialize MT8870 CallerID receiver
 */
void DTMFin_Init(void);

/**
 * @brief Poll driver for status
 * 
 * Function returns MT8870_Ret_Finished if a complete
 * number is stored in buffer supplied with the Start
 * function.
 */
MT8870_Ret_t DTMFin_Poll(uint8_t *len);

/**
 * @brief Start the CallerID  
 */
void DTMFin_Start(uint8_t *buffer);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
void DTMFin_timer_callback(uint8_t timer);

/**@}*/
#endif /*MT8870_H_*/
