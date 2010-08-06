/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "act_protectedOutput.h"


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define	OUTPUT0_ON		(act_protectedOutput_OUTPUT0_PIN_ACTIVE_STATE)
#define	OUTPUT0_OFF		(1 - OUTPUT0_ON)
#define	OUTPUT0_PIN		act_protectedOutput_OUTPUT0_PIN

#define	OUTPUT1_ON		(act_protectedOutput_OUTPUT1_PIN_ACTIVE_STATE)
#define	OUTPUT1_OFF		(1 - OUTPUT1_ON)
#define	OUTPUT1_PIN		act_protectedOutput_OUTPUT1_PIN

#define	OUTPUT2_ON		(act_protectedOutput_OUTPUT2_PIN_ACTIVE_STATE)
#define	OUTPUT2_OFF		(1 - OUTPUT2_ON)
#define	OUTPUT2_PIN		act_protectedOutput_OUTPUT2_PIN

#define	OUTPUT3_ON		(act_protectedOutput_OUTPUT3_PIN_ACTIVE_STATE)
#define	OUTPUT3_OFF		(1 - OUTPUT3_ON)
#define	OUTPUT3_PIN		act_protectedOutput_OUTPUT3_PIN

#define	DIAG_ASSERTED	(act_protectedOutput_DIAG_PIN_ASSERTED_STATE)
#define	DIAG_NORMAL		(1 - DIAG_ASSERTED)
#define	DIAG_PIN		act_protectedOutput_DIAG_PIN


/*-----------------------------------------------------------------------------
 * Types
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/

static uint8_t outputStateTarget[act_protectedOutput_NR_OUTPUT_PINS];

static uint8_t diagState = DIAG_NORMAL;

// is there a recent status change that we should report via CAN?
static uint8_t diagReportPending = 0;


/*-----------------------------------------------------------------------------
 * Internal Function Prototypes
 *---------------------------------------------------------------------------*/
static void updateOutput(void);
static void readDiagPin(void);
static void pcIntCallback(uint8_t id, uint8_t status);


/*-----------------------------------------------------------------------------
 * Function Implementations
 *---------------------------------------------------------------------------*/

static void updateOutput() {

	#if act_protectedOutput_NR_OUTPUT_PINS >= 1
	if (outputStateTarget[0] == 0) {
		gpio_set_statement(OUTPUT0_OFF, OUTPUT0_PIN);
	}
	else if (outputStateTarget[0] == 1) {
		if (diagState == DIAG_NORMAL) {
			gpio_set_statement(OUTPUT0_ON, OUTPUT0_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(OUTPUT0_OFF, OUTPUT0_PIN);
		}
	}
	#endif

	#if act_protectedOutput_NR_OUTPUT_PINS >= 2
	if (outputStateTarget[1] == 0) {
		gpio_set_statement(OUTPUT1_OFF, OUTPUT1_PIN);
	}
	else if (outputStateTarget[1] == 1) {
		if (diagState == DIAG_NORMAL) {
			gpio_set_statement(OUTPUT1_ON, OUTPUT1_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(OUTPUT1_OFF, OUTPUT1_PIN);
		}
	}
	#endif

	#if act_protectedOutput_NR_OUTPUT_PINS >= 3
	if (outputStateTarget[2] == 0) {
		gpio_set_statement(OUTPUT2_OFF, OUTPUT2_PIN);
	}
	else if (outputStateTarget[2] == 1) {
		if (diagState == DIAG_NORMAL) {
			gpio_set_statement(OUTPUT2_ON, OUTPUT2_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(OUTPUT2_OFF, OUTPUT2_PIN);
		}
	}
	#endif

	#if act_protectedOutput_NR_OUTPUT_PINS >= 4
	if (outputStateTarget[3] == 0) {
		gpio_set_statement(OUTPUT3_OFF, OUTPUT3_PIN);
	}
	else if (outputStateTarget[3] == 1) {
		if (diagState == DIAG_NORMAL) {
			gpio_set_statement(OUTPUT3_ON, OUTPUT3_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(OUTPUT3_OFF, OUTPUT3_PIN);
		}
	}
	#endif
}


static void readDiagPin() {
	diagState = gpio_get_state(DIAG_PIN);
}


static void pcIntCallback(uint8_t id, uint8_t status) {
	// new state of the DIAG pin?
	if (id == act_protectedOutput_DIAG_PIN_PCINT) {
		diagState = status;
		updateOutput();
		// DIAG asserted?
		if (diagState == DIAG_ASSERTED) {
			// if retry-timer mechanism is enabled, initiate timer
			if (act_protectedOutput_RETRY_TIMER_TIME_S > 0) {
				Timer_SetTimeout(act_protectedOutput_RETRY_TIMER, act_protectedOutput_RETRY_TIMER_TIME_S*1000, TimerTypeOneShot, 0);
			}
			// if the retry-mechanism is disabled, change the target output state to OFF
			else {
				for (uint8_t i=0; i<act_protectedOutput_NR_OUTPUT_PINS; i++) {
					outputStateTarget[i] = 0;
				}
			}
		}
		// something interesting obviously happened. let's report it
		diagReportPending = 1;
	}
}


void act_protectedOutput_Init() {
	/*
	 * Configure DIAG input pin
	 */
	if (act_protectedOutput_DIAG_PIN_PULL_ENABLED) {
		// if DIAG is asserted low, we need pull-up
		gpio_set_statement(act_protectedOutput_DIAG_PIN_ASSERTED_STATE == 0 ? 1 : 0, DIAG_PIN);
	}
	gpio_set_in(DIAG_PIN);
	Pcint_SetCallbackPin(act_protectedOutput_DIAG_PIN_PCINT, DIAG_PIN, &pcIntCallback);
	
	/*
	 * Configure OUTPUT pins
	 */
	#if act_protectedOutput_NR_OUTPUT_PINS >= 1
	gpio_set_statement(OUTPUT0_OFF, OUTPUT0_PIN);
	gpio_set_out(act_protectedOutput_OUTPUT0_PIN);
	#endif
	
	#if act_protectedOutput_NR_OUTPUT_PINS >= 2
	gpio_set_statement(OUTPUT1_OFF, OUTPUT1_PIN);
	gpio_set_out(act_protectedOutput_OUTPUT1_PIN);
	#endif

	#if act_protectedOutput_NR_OUTPUT_PINS >= 3
	gpio_set_statement(OUTPUT2_OFF, OUTPUT2_PIN);
	gpio_set_out(act_protectedOutput_OUTPUT2_PIN);
	#endif

	#if act_protectedOutput_NR_OUTPUT_PINS >= 4
	gpio_set_statement(OUTPUT3_OFF, OUTPUT3_PIN);
	gpio_set_out(act_protectedOutput_OUTPUT3_PIN);
	#endif

	for (uint8_t i=0; i<act_protectedOutput_NR_OUTPUT_PINS; i++) {
		outputStateTarget[i] = 0;
	}

	diagState = DIAG_NORMAL;
}


void act_protectedOutput_Process() {

	// shall we retry to set target output state?
	if (Timer_Expired(act_protectedOutput_RETRY_TIMER)) {
		// read DIAG pin again and update outputs accordingly
		readDiagPin();
		updateOutput();
		if (diagState == DIAG_ASSERTED) {
			// if DIAG was still asserted, initiate another timer run
			Timer_SetTimeout(act_protectedOutput_RETRY_TIMER, act_protectedOutput_RETRY_TIMER_TIME_S*1000, TimerTypeOneShot, 0);
		}
		else {
			// things went back to normal. report this
			diagReportPending = 1;
		}
	}
	
	// shall we report diag status to CAN?
	if (diagReportPending) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PROTECTEDOUTPUT;
		txMsg.Header.ModuleId = act_protectedOutput_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PINSTATUS;
		txMsg.Length = 2;
		txMsg.Data[0] = 0; //TODO: add support for more channels
		// we follow the standard SNS_INPUT format, but the data corresponds to the "health" rather than physical level
		if (diagState == DIAG_NORMAL) {
			txMsg.Data[1] = 1;	//healthy, target output state stable
		} else {
			txMsg.Data[1] = 0;	//not healthy, DIAG pin ASSERTED, not in target output state
		}
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		diagReportPending = 0;
	}
}


void act_protectedOutput_HandleMessage(StdCan_Msg_t *rxMsg) {
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_PROTECTEDOUTPUT && 
		rxMsg->Header.ModuleId == act_protectedOutput_ID)
	{
		switch (rxMsg->Header.Command) {
			
			case CAN_MODULE_CMD_PHYSICAL_SETPIN:
				
				if (rxMsg->Length == 2) {
					uint8_t channel = rxMsg->Data[0];
					if (channel >= 0 && channel < act_protectedOutput_NR_OUTPUT_PINS) {
						outputStateTarget[channel] = rxMsg->Data[1];
						readDiagPin();
						updateOutput();
					}				
					StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
					rxMsg->Length = 2;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
				}
				break;
		}
	}
}


void act_protectedOutput_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PROTECTEDOUTPUT;
	txMsg.Header.ModuleId = act_protectedOutput_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

