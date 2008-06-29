/** 
 * @defgroup mt8870 Mitel MT8870 DTMF-receiver Library
 * @code #include <drivers/DTMF/mt8870/mt8870.h> @endcode
 * 
 * @brief Driver for MT8870 DTMF receiver
 * 
 * Driver handle only reception of dtmf codes. 
 * When the pop function return DTMF_Ret_Data_avail
 * data can be found in the buffer.
 * The length pointer supplied with function Pop is used to
 * find out the length of data. 
 * 
 * @author	Anders Runeson
 * @date	2007-08-24
 */

/**@{*/

#ifndef MT8870_H_
#define MT8870_H_

#include <drivers/DTMF/dtmf.h>
/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
/**
 * @brief Initialize MT8870 CallerID receiver
 */
void mt8870_Init(void);

/**
 * @brief Poll driver for status
 * 
 * Function returns MT8870_Ret_Finished if a complete
 * number is stored in buffer supplied with the Start
 * function.
 */
DTMF_Ret_t mt8870_Pop(uint8_t *buffer, uint8_t *len);

/**@}*/
#endif /*MT8870_H_*/
