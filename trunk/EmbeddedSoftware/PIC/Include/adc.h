/*********************************************************************
 *
 *   Header file for ADC routines.
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/
#ifndef ADC_H
#define ADC_H

#include <stackTasks.h>
#include <typedefs.h>
#include <delays.h>
#include <compiler.h>

#define TEMP_SAMPLES 50 // Number of samples

void adcInit(BOOL useExtRef,BYTE portCfg);
void adcISR(void);
BOOL adcConvert(BYTE channel);
void temperatureRead(char *tenth, BYTE *decimal);
int adcRead(void);
BOOL adcDone(void);

#endif
