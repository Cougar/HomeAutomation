#ifndef DTMF_H_
#define DTMF_H_

#include <inttypes.h>

/**
 * @brief Return values.
 */
typedef enum {
	DTMF_Ret_No_data,		 	/**< No complete number yet. */
	DTMF_Ret_Data_avail, 		/**< Data is available. */
	DTMF_Ret_Overflow, 			/**< Buffer was overflown. */
} DTMF_Ret_t;

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
/**
 * @brief Initialize DTMF receiver
 */
void DTMFin_Init(void);

/**
 * @brief Poll driver for status
 * 
 * 
 * 
 * 
 */
DTMF_Ret_t DTMFin_Pop(uint8_t *buffer, uint8_t *len);


#endif /*DTMF_H_*/
