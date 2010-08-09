/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "act_protectedOutput.h"


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

// make sure all config params are present

#ifndef act_protectedOutput_CH_COUNT
#error Config value missing: "act_protectedOutput_CH_COUNT"
#endif

#ifndef act_protectedOutput_EEPROM_ENABLED
#error Config value missing: "act_protectedOutput_EEPROM_ENABLED"
#endif

#if act_protectedOutput_CH_COUNT >= 1
#ifndef act_protectedOutput_CH0
#error Config value missing: "act_protectedOutput_CH0"
#endif
#ifndef act_protectedOutput_CH0_POLARITY
#error Config value missing: "act_protectedOutput_CH0_POLARITY"
#endif
#endif

#if act_protectedOutput_CH_COUNT >= 2
#ifndef act_protectedOutput_CH1
#error Config value missing: "act_protectedOutput_CH1"
#endif
#ifndef act_protectedOutput_CH1_POLARITY
#error Config value missing: "act_protectedOutput_CH1_POLARITY"
#endif
#endif

#if act_protectedOutput_CH_COUNT >= 3
#ifndef act_protectedOutput_CH2
#error Config value missing: "act_protectedOutput_CH2"
#endif
#ifndef act_protectedOutput_CH2_POLARITY
#error Config value missing: "act_protectedOutput_CH2_POLARITY"
#endif
#endif

#if act_protectedOutput_CH_COUNT >= 4
#ifndef act_protectedOutput_CH3
#error Config value missing: "act_protectedOutput_CH3"
#endif
#ifndef act_protectedOutput_CH3_POLARITY
#error Config value missing: "act_protectedOutput_CH3_POLARITY"
#endif
#endif

#if act_protectedOutput_CH_COUNT >= 5
#error Config value out of range: "act_protectedOutput_CH_COUNT"
#endif

#define	CH0_ON		(act_protectedOutput_CH0_POLARITY)
#define	CH0_OFF		(1 - CH0_ON)
#define	CH0_PIN		act_protectedOutput_CH0

#define	CH1_ON		(act_protectedOutput_CH1_POLARITY)
#define	CH1_OFF		(1 - CH1_ON)
#define	CH1_PIN		act_protectedOutput_CH1

#define	CH2_ON		(act_protectedOutput_CH2_POLARITY)
#define	CH2_OFF		(1 - CH2_ON)
#define	CH2_PIN		act_protectedOutput_CH2

#define	CH3_ON		(act_protectedOutput_CH3_POLARITY)
#define	CH3_OFF		(1 - CH3_ON)
#define	CH3_PIN		act_protectedOutput_CH3

#define	DIAG_ASSERTED	(act_protectedOutput_DIAG_PIN_POLARITY)
#define	DIAG_NORMAL		(1 - DIAG_ASSERTED)
#define	DIAG_PIN		act_protectedOutput_DIAG_PIN


/*-----------------------------------------------------------------------------
 * Types
 *---------------------------------------------------------------------------*/


#if act_protectedOutput_EEPROM_ENABLED == 1
#include "act_protectedOutput_eeprom.h"
struct eeprom_act_protectedOutput EEMEM eeprom_act_protectedOutput =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
#if	act_protectedOutput_CH_COUNT >= 1
		0x00,
#endif
#if	act_protectedOutput_CH_COUNT >= 2
		0x00,
#endif
#if	act_protectedOutput_CH_COUNT >= 3
		0x00,
#endif
#if	act_protectedOutput_CH_COUNT >= 4
		0x00,
#endif
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif


/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/

static uint8_t chTargetState[act_protectedOutput_CH_COUNT];
static uint8_t diagState = DIAG_NORMAL;

// is there a recent status change that we should report via CAN?
static uint8_t diagReportPending = 0;


/*-----------------------------------------------------------------------------
 * Internal Function Prototypes
 *---------------------------------------------------------------------------*/
static void updateOutput(uint8_t skipDiagCheck);
static void readDiagPin(void);
static void pcIntCallback(uint8_t id, uint8_t status);


/*-----------------------------------------------------------------------------
 * Function Implementations
 *---------------------------------------------------------------------------*/

static void updateOutput(uint8_t skipDiagCheck) {

	#if act_protectedOutput_CH_COUNT >= 1
	if (chTargetState[0] == 0) {
		gpio_set_statement(CH0_OFF, CH0_PIN);
	}
	else if (chTargetState[0] == 1) {
		if (diagState == DIAG_NORMAL || skipDiagCheck) {
			gpio_set_statement(CH0_ON, CH0_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(CH0_OFF, CH0_PIN);
		}
	}
	#endif

	#if act_protectedOutput_CH_COUNT >= 2
	if (chTargetState[1] == 0) {
		gpio_set_statement(CH1_OFF, CH1_PIN);
	}
	else if (chTargetState[1] == 1) {
		if (diagState == DIAG_NORMAL || skipDiagCheck) {
			gpio_set_statement(CH1_ON, CH1_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(CH1_OFF, CH1_PIN);
		}
	}
	#endif

	#if act_protectedOutput_CH_COUNT >= 3
	if (chTargetState[2] == 0) {
		gpio_set_statement(CH2_OFF, CH2_PIN);
	}
	else if (chTargetState[2] == 1) {
		if (diagState == DIAG_NORMAL || skipDiagCheck) {
			gpio_set_statement(CH2_ON, CH2_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(CH2_OFF, CH2_PIN);
		}
	}
	#endif

	#if act_protectedOutput_CH_COUNT >= 4
	if (chTargetState[3] == 0) {
		gpio_set_statement(CH3_OFF, CH3_PIN);
	}
	else if (chTargetState[3] == 1) {
		if (diagState == DIAG_NORMAL || skipDiagCheck) {
			gpio_set_statement(CH3_ON, CH3_PIN);
		}
		else {
			// DIAG override, we cannot set ON state right now
			gpio_set_statement(CH3_OFF, CH3_PIN);
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
		//diagState = status;
		readDiagPin();
		updateOutput(0);
		// DIAG asserted?
		if (diagState == DIAG_ASSERTED) {
			// if retry-timer mechanism is enabled, initiate timer
			if (act_protectedOutput_RETRY_TIMER_TIME_S > 0) {
				Timer_SetTimeout(act_protectedOutput_RETRY_TIMER, act_protectedOutput_RETRY_TIMER_TIME_S*1000, TimerTypeOneShot, 0);
			}
			// if the retry-mechanism is disabled, change the target output state to OFF
			else {
				for (uint8_t i=0; i<act_protectedOutput_CH_COUNT; i++) {
					chTargetState[i] = 0;
				}
#if act_protectedOutput_EEPROM_ENABLED == 1
				// output states changed, store to EE after this timer delay
				Timer_SetTimeout(act_protectedOutput_STORE_VALUE_TIMEOUT, act_protectedOutput_STORE_VALUE_TIMEOUT_TIME_S*1000, TimerTypeOneShot, 0);
#endif
			}
		}
		// something interesting obviously happened. let's report it
		diagReportPending = 1;
	}
}


void act_protectedOutput_Init() {
	/*
	 * Init target state vector, in case EEPROM is disabled
	 */
	for (uint8_t i=0; i<act_protectedOutput_CH_COUNT; i++) {
		chTargetState[i] = 0;
	}

	/*
	 * Configure DIAG input pin
	 */
	if (act_protectedOutput_DIAG_PIN_PULL_ENABLED) {
		// if DIAG is asserted low, we need pull-up
		gpio_set_statement(act_protectedOutput_DIAG_PIN_POLARITY == 0 ? 1 : 0, DIAG_PIN);
	}
	gpio_set_in(DIAG_PIN);
	Pcint_SetCallbackPin(act_protectedOutput_DIAG_PIN_PCINT, DIAG_PIN, &pcIntCallback);
	
	/*
	 * Read EEPROM data
	 */
#if act_protectedOutput_EEPROM_ENABLED == 1
	if (EEDATA_OK) {
#if	act_protectedOutput_CH_COUNT >= 1
	  chTargetState[0] = eeprom_read_byte(EEDATA.ch0);
#endif
#if	act_protectedOutput_CH_COUNT >= 2
	  chTargetState[1] = eeprom_read_byte(EEDATA.ch1);
#endif
#if	act_protectedOutput_CH_COUNT >= 3
	  chTargetState[2] = eeprom_read_byte(EEDATA.ch2);
#endif
#if	act_protectedOutput_CH_COUNT >= 4
	  chTargetState[3] = eeprom_read_byte(EEDATA.ch3);
#endif
	}
	else {
		//The CRC of the EEPROM is not correct, store default values and update CRC
#if	act_protectedOutput_CH_COUNT >= 1
		eeprom_write_byte_crc(EEDATA.ch0, 0x00, WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 2
		eeprom_write_byte_crc(EEDATA.ch1, 0x00, WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 3
		eeprom_write_byte_crc(EEDATA.ch2, 0x00, WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 4
		eeprom_write_byte_crc(EEDATA.ch3, 0x00, WITHOUT_CRC);
#endif
		EEDATA_UPDATE_CRC;
	}
#endif

	/*
	 * Configure OUTPUT pins
	 */
	#if act_protectedOutput_CH_COUNT >= 1
	gpio_set_statement(CH0_OFF, CH0_PIN);
	gpio_set_out(act_protectedOutput_CH0);
	#endif
	
	#if act_protectedOutput_CH_COUNT >= 2
	gpio_set_statement(CH1_OFF, CH1_PIN);
	gpio_set_out(act_protectedOutput_CH1);
	#endif

	#if act_protectedOutput_CH_COUNT >= 3
	gpio_set_statement(CH2_OFF, CH2_PIN);
	gpio_set_out(act_protectedOutput_CH2);
	#endif

	#if act_protectedOutput_CH_COUNT >= 4
	gpio_set_statement(CH3_OFF, CH3_PIN);
	gpio_set_out(act_protectedOutput_CH3);
	#endif

	diagState = DIAG_NORMAL;
}


void act_protectedOutput_Process() {

#if act_protectedOutput_EEPROM_ENABLED == 1
	if (Timer_Expired(act_protectedOutput_STORE_VALUE_TIMEOUT)) {
#if	act_protectedOutput_CH_COUNT >= 1
		eeprom_write_byte_crc(EEDATA.ch0, chTargetState[0], WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 2
		eeprom_write_byte_crc(EEDATA.ch1, chTargetState[1], WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 3
		eeprom_write_byte_crc(EEDATA.ch2, chTargetState[2], WITHOUT_CRC);
#endif
#if	act_protectedOutput_CH_COUNT >= 4
		eeprom_write_byte_crc(EEDATA.ch3, chTargetState[3], WITHOUT_CRC);
#endif
		EEDATA_UPDATE_CRC;
	}
#endif

	// shall we retry to set target output state?
	if (Timer_Expired(act_protectedOutput_RETRY_TIMER)) {
		// read DIAG pin again and update outputs accordingly
		readDiagPin();
#if act_protectedOutput_FORCED_RETRIES == 1
		// forced retry to set output states, regardless of DIAG
		updateOutput(1);
		// read DIAG pin again and hope we have a better flag
		readDiagPin();
#else
		// if DIAG allows it, retry to set the output states
		updateOutput(0);
#endif
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
				/*
				 * This message is used to activate/deactivate
				 * an output channel.
				 */
				if (rxMsg->Length == 2) {
					uint8_t channel = rxMsg->Data[0];
					if (channel >= 0 && channel < act_protectedOutput_CH_COUNT) {
						chTargetState[channel] = rxMsg->Data[1];
						readDiagPin();
#if act_protectedOutput_FORCED_RETRIES == 1
						updateOutput(1);
#else
						updateOutput(0);
#endif
#if act_protectedOutput_EEPROM_ENABLED == 1
						// output states changed, store to EE after this timer delay
						Timer_SetTimeout(act_protectedOutput_STORE_VALUE_TIMEOUT, act_protectedOutput_STORE_VALUE_TIMEOUT_TIME_S*1000, TimerTypeOneShot, 0);
#endif
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

