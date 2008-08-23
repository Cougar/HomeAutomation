#ifndef CANPRINTF_H_
#define CANPRINTF_H_

#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/can/modules/GlobalCMD.h>

#ifndef CAN_PRINTF
#define CAN_PRINTF 0
#endif

void CanPrintf_Init(void);
int CanPutChar(char c, FILE* stream);

#endif /*CANPRINTF_H_*/
