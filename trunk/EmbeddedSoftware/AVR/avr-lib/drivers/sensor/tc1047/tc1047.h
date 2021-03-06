#ifndef TC1047_H_
#define TC1047_H_


/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*-----------------------------------------------
 * Defines
 * ---------------------------------------------*/


/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
uint8_t adcTemperatureInit(void);
uint16_t getTC1047temperature(uint8_t sensor);

#endif
