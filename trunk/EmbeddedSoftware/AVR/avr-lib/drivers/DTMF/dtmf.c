/**
 * DTMF wrapper driver.
 * 
 * @date	2008-06-29
 * 
 * @author	Anders Runeson
 *   
 */

#include <config.h>
#include <drivers/DTMF/dtmf.h>


#ifdef MT8870
#include <drivers/DTMF/mt8870/mt8870.h>
#else
#error No DTMF driver selected
#endif

void DTMFin_Init(void) {
#ifdef MT8870
	mt8870_Init();
#endif
}

DTMF_Ret_t DTMFin_Pop(uint8_t *buffer, uint8_t *len) {
#ifdef MT8870
	return mt8870_Pop(buffer, len);
#endif 
}
