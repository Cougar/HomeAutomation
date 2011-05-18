/* 
	linearAct started by dberg 2011.
	
	This module is written to control an old linear actuator from Landis & Gyr, but should work
	with a broad range of linear actuators and other stuff that conforms to its simple protocol:
	
	Movement is controlled by two signals, typically controlling two relays. Feedback of 
	position is sensed by pulses connected to the hardware counter.
	
	The two control signals are ON/OFF and DIRECTION - one relay to control actuator engine on/off
	and the other one to change the polarity.
	
	The position is always relative and must be calibrated manually. Use CALIBRATION command to
	give the module full moving space.
	
	Posible features to add:
	- automatic recalibration by external switch (one or perhaps two)
	- PWM speedcontrol
*/ 


#define USE_STIMULI 1					// Stimuli can be used to simulate a pulse-source. Hardware
												//  connection also needed, see config.inc.template for more info


#include "act_linearAct.h"

#ifdef act_linearAct_USEEEPROM
#include "act_linearAct_eeprom.h"
struct eeprom_act_linearAct EEMEM eeprom_act_linearAct =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		1200,		// pulses
		300,		// min
		815,		// low
		5600,		// high
		6100		// max
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

#define NOT_MOVING 0
#define UP 		1
#define DOWN 	2
#define stopTimeout 1500		// ms (timerquanta)

volatile uint16_t pulses;
uint16_t min;
uint16_t low;
uint16_t high;
uint16_t max;

volatile uint8_t stopfactor = 0;
volatile uint8_t direction = 0;
volatile uint8_t latest_stop_pulses_up = 0;
volatile uint8_t latest_stop_pulses_down = 0;

uint8_t act_linearAct_ReportInterval = (uint8_t)act_linearAct_SEND_PERIOD;

void linearAct_sendPosition(uint8_t dummy);
void linearAct_sendLimits (uint8_t dummy);
uint16_t linearAct_getPosition (void);
uint8_t linearAct_setPosition (uint16_t new_position);
void linearAct_stop(void);
void linearAct_cleanUp (uint8_t dummy);




ISR (TIMER1_COMPA_vect)
{
	linearAct_stop();							// stoppa maskineriet
}	




void act_linearAct_Init(void)
{
#ifdef act_linearAct_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  pulses = eeprom_read_word(EEDATA16.pulses_ee);
	  min = eeprom_read_word(EEDATA16.min_ee);
	  low = eeprom_read_word(EEDATA16.low_ee);
	  high = eeprom_read_word(EEDATA16.high_ee);
	  max = eeprom_read_word(EEDATA16.max_ee);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.pulses_ee, 1200, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.min_ee, 300, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.low_ee, 815, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.high_ee, 5600, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.max_ee, 6100, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#else	
	// Only for testing/debugging...
	pulses = 1200;
	min = 300;
	low = 815;
	high = 5600;
	max = 6100;
#endif
	
	// PD5 (EXP_G) is used as Timer1 input. Hardwired, no move possible!
	gpio_set_in(EXP_G);
	gpio_set_pullup(EXP_G);
	
	//PORTD &= ~(_BV(DIR_RELAY) | _BV(ON_RELAY));
	gpio_clr_pin(RELAY_ON);
	gpio_clr_pin(RELAY_DIR);
	
	// initiera utgångar 
	//DDRD = (_BV(DIR_RELAY) | _BV(ON_RELAY));
	gpio_set_out(RELAY_ON);
	gpio_set_out(RELAY_DIR);

	// Start report timer
	Timer_SetTimeout(act_linearAct_TIMER_report, act_linearAct_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#if (USE_STIMULI == 1)	
	// Set stimuli as an output
	gpio_set_out(STIMULI);
	Timer_SetTimeout(act_linearAct_TIMER_stimuli, 10, TimerTypeFreeRunning,0);
#endif
}

void act_linearAct_Process(void)
{
	if (Timer_Expired(act_linearAct_TIMER_report)) {
		linearAct_sendPosition(0);
	}
#if (USE_STIMULI == 1)
	if (Timer_Expired(act_linearAct_TIMER_stimuli)) {
		gpio_toggle_pin(STIMULI);
	}
#endif
}

void act_linearAct_HandleMessage(StdCan_Msg_t *rxMsg)
{
	uint16_t i;
	
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&		  
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_LINEARACT && 
		rxMsg->Header.ModuleId == act_linearAct_ID)
	{
		switch (rxMsg->Header.Command) {
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			act_linearAct_ReportInterval = rxMsg->Data[0];
			Timer_SetTimeout(act_linearAct_TIMER_report, act_linearAct_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			break;
		case CAN_MODULE_CMD_LINEARACT_POSITION:
			if (rxMsg->Length >= 3) {		// remain forward compatible by allowing longer messages, but not shorter
				if (rxMsg->Data[2] != 0) {
					Timer_SetTimeout(act_linearAct_TIMER_report, 750 , TimerTypeFreeRunning, 0);
					linearAct_setPosition(((uint16_t)rxMsg->Data[0] << 8) + rxMsg->Data[1]);
				} else {	// Data[2] signals moving speed, where zero stands for stop and also triggers transmission of limits
					linearAct_stop();
					// wait until complete stop (with timeout)
					i = 0;
					do {
						if (direction == 0) break;
						_delay_ms(10);
						i++;	
					} while (i < 200);
					
					linearAct_sendPosition(0);
					linearAct_sendLimits(0);
				}
			}
			break;
		case CAN_MODULE_CMD_LINEARACT_LIMITS:
			if (rxMsg->Length == 8) {
			
				min = (rxMsg->Data[0] << 8) + rxMsg->Data[1];		
				low = (rxMsg->Data[2] << 8) + rxMsg->Data[3];
				high = (rxMsg->Data[4] << 8) + rxMsg->Data[5];
				max = (rxMsg->Data[6] << 8) + rxMsg->Data[7];
			
				// pulses are already saved. eeprom_write_word_crc(EEDATA16.pulses_ee, 1200, WITHOUT_CRC);
	  			eeprom_write_word_crc(EEDATA16.min_ee, min, WITHOUT_CRC);
	  			eeprom_write_word_crc(EEDATA16.low_ee, low, WITHOUT_CRC);
	  			eeprom_write_word_crc(EEDATA16.high_ee, high, WITHOUT_CRC);
	  			eeprom_write_word_crc(EEDATA16.max_ee, max, WITHOUT_CRC);
	  			EEDATA_UPDATE_CRC;			
				
			}
			break;
		case CAN_MODULE_CMD_LINEARACT_CALIBRATION:		// set limits to absolute maximum to allow full movement
		   min = 0;
		   low = 0;
		   high = 0xFFFF;
		   max = 0xFFFF;
		   if (rxMsg->Length >= 2) {		// optional: set start position. Note! This will definitly ruin the old calibration.
		   	pulses = (rxMsg->Data[0] << 8) + rxMsg->Data[1];
		   }
		   break;
		default:
		   break;
		}
	} 
}

void act_linearAct_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_LINEARACT; 
	txMsg.Header.ModuleId = act_linearAct_ID;
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


void linearAct_sendPosition(uint8_t dummy) {
	StdCan_Msg_t txMsg;
	uint16_t w_tmp;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_LINEARACT; 
	txMsg.Header.ModuleId = act_linearAct_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_LINEARACT_POSITION;	
	txMsg.Length = 3;
	
	w_tmp = linearAct_getPosition();
	
	txMsg.Data[0] = w_tmp >> 8;
	txMsg.Data[1] = w_tmp;
	
	txMsg.Data[2] = direction;									

	//while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	StdCan_Put(&txMsg);										// No problem if we miss one	
}


void linearAct_sendLimits (uint8_t dummy) {
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_LINEARACT; 
	txMsg.Header.ModuleId = act_linearAct_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_LINEARACT_LIMITS;	
	txMsg.Length = 8;
	
	txMsg.Data[0] = min >> 8;
	txMsg.Data[1] = min;
		
	txMsg.Data[2] = low >> 8;
	txMsg.Data[3] = low;
			
	txMsg.Data[4] = high >> 8;
	txMsg.Data[5] = high;

	txMsg.Data[6] = max >> 8;
	txMsg.Data[7] = max;
		
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}


/**
	Return current position
*/
uint16_t linearAct_getPosition (void) {
uint16_t total_pulses;
	// Check if timer1 is running, if so, we have to take a snapshot
	if ((stopfactor) || (TCCR1B != 0)) {											
		if (direction == UP) total_pulses = pulses + TCNT1;
		else total_pulses = pulses - TCNT1;
	} 
	else total_pulses = pulses;
	
	return total_pulses;
}


/**
	Move to position
	Position is a 16 bits integer between min and max.
	Not degrees, nor percent.
	*/
uint8_t linearAct_setPosition (uint16_t new_position) {

	/* Check if position is within limits */
	if (new_position > max) new_position = max;
	if (new_position < min) new_position = min;
	
	// First check if we need to move at all
	if (new_position != pulses) {		
		
		// Check if we already are in movement, then exit with value 1
		if (direction != 0) {
			//stop();
			//while (in_motion());
			return 1;					// no autostop, let the user stop movement first. This may be improved.
		}
		
		if (new_position > pulses) {
		   if (latest_stop_pulses_up > (new_position - pulses)) return 1;			// We will move past target, so don't move
			OCR1A = new_position - pulses - latest_stop_pulses_up;	// set Output Compare target 
			TIMSK1 = _BV(OCIE1A);						// activate interrupts for output compare!
			direction = UP;								// Adjust direction for software
			gpio_set_pin(RELAY_DIR);					// Adjust direction physically
		}
		else  {
			if (latest_stop_pulses_up > (pulses - new_position)) return 1;			// We will move past target, so don't move
			OCR1A = pulses - new_position - latest_stop_pulses_down;	
			TIMSK1 = _BV(OCIE1A);						
			direction = DOWN;
			gpio_clr_pin(RELAY_DIR);
		}
		TCCR1B = _BV(CS12) | _BV(CS11);				// Activate counter
		_delay_ms(10);									// wait until direction relay is ready  
		gpio_clr_pin(RELAY_ON);						// Start moving!
	}
	return 0;
}

void linearAct_stop(void) {
	gpio_clr_pin(RELAY_ON);
	_delay_ms(10);
	gpio_clr_pin(RELAY_DIR);
	stopfactor = 1;
	
	// start stop-timer
	Timer_SetTimeout(act_linearAct_TIMER_stop, stopTimeout , TimerTypeOneShot, linearAct_cleanUp);
	//printf("In linearAct_stop\n");
}

void linearAct_cleanUp (uint8_t dummy) {
	if (stopfactor) {
		stopfactor = 0;
		// Add number of pulses
		if (direction == UP) {
			pulses += TCNT1;		// adjust official counter
			// keep track on how far beyond target we have moved
			if (TCNT1 > OCR1A) latest_stop_pulses_up = (uint8_t) TCNT1 - OCR1A;
			else latest_stop_pulses_up = 0;
		}
		else {
			pulses -= TCNT1;
			// keep track on how far beyond target we have moved
			if (TCNT1 > OCR1A) latest_stop_pulses_down = (uint8_t) TCNT1 - OCR1A;
			else latest_stop_pulses_down = 0;
		}
		TCCR1B = 0;	
		TCNT1 = 0;							// reset counter before next run
		direction = 0;
		eeprom_write_word_crc(EEDATA16.pulses_ee, pulses, WITH_CRC);
		//printf("In linearAct_cleanUp\n");
		Timer_SetTimeout(act_linearAct_TIMER_report, act_linearAct_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		linearAct_sendPosition(0);
	}
}



