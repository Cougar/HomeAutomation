#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <drivers/can/stdcan.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timer.h>

#include "../modules/sns_ds18x20/sns_ds18x20.h"

int main(void)
{
	StdCan_Msg_t rxMsg; // Message storage

	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();

	StdCan_Init();
	
	Timer_SetTimeout(APP_HEARTBEAT_TIMER, STDCAN_HEARTBEAT_PERIOD, TimerTypeFreeRunning, StdCan_SendHeartbeat);
	
	sns_ds18x20_Init();

	while (1) {
	
		sns_ds18x20_Process();
	
		if (StdCan_Get(&rxMsg) == StdCan_Ret_OK) {
			if ((uint8_t)StdCan_Ret_class(rxMsg) == CAN_CLASS_MODULE_NMT) {
				
				if (rxMsg.Header.ModuleType == CAN_CMD_MODULE_NMT_LIST) {
					sns_ds18x20_List(1);
				}
			}
			else {
			
				sns_ds18x20_HandleMessage(&rxMsg);
			}
		}
	}
	
	return 0;
}
