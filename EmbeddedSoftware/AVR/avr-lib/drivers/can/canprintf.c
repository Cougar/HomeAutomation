
#include "canprintf.h"

#if CAN_PRINTF==1
/*
* This is included in the default module somehow, and the user of a module
* only has to enable CAN_PRINTF in config.inc
*
*/
#define SEND_BUFFER_SIZE 8
static StdCan_Msg_t printfTxMsg;
static uint8_t bufpoint;
static FILE canstdout = FDEV_SETUP_STREAM(CanPutChar, NULL, _FDEV_SETUP_WRITE);

/*----------------------------------------------------------------------------
* Initfunction for printf-CAN
* Call this function from your initiation routine
*
*--------------------------------------------------------------------------*/
void CanPrintf_Init(void) 
{
	StdCan_Set_class(printfTxMsg.Header, CAN_MODULE_CLASS_TST);
	StdCan_Set_direction(printfTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	printfTxMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	printfTxMsg.Header.ModuleId = 0;
	printfTxMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_ASCII;
	bufpoint = 0;
	stdout = &canstdout;    //set the output stream
}

/*----------------------------------------------------------------------------
* Putchar for CAN
* Implements a small buffer, sends packet over CAN if buffer is full or
* \n-char is sent.
* No timeout to force send, so a string that should be completely printed should
* be terminated by \n
*--------------------------------------------------------------------------*/
int CanPutChar(char c, FILE* stream) 
{
	printfTxMsg.Data[bufpoint] = c;
	bufpoint++;
 
	if (c == '\n' || bufpoint >= SEND_BUFFER_SIZE) {
		printfTxMsg.Length = bufpoint;
		StdCan_Put(&printfTxMsg);
		bufpoint = 0;
		
		for (uint16_t i = 1; i > 0; i++) {
			asm("nop");
		}
	}
	return 0;
}
#endif //CAN_PRINTF
