
#include "act_PID_eeprom.h"

#include "act_PID.h"
#include <drivers/misc/pid.h>

#define PID_ON 1
#define PID_OFF 0
#define PID_AUTO 2

//! Parameters for regulator
struct PID_DATA pidData;
struct PID_DEBUG_DATA pidDebugData;

uint8_t sensorModuleType, sensorModuleId,sensorId;
uint8_t PID_Status;
uint8_t calculatePID_flag,sendPID_flag = 0;
uint16_t pwmValue=0;
float referenceValue, measurementValue, inputValue;

struct eeprom_act_PID EEMEM eeprom_act_PID = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0x0200,	// referenceValue
		0x00,	//SensorModuleType
		0x00,	//SensorModuleId
		0x00	//SensorId
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 

void calculatePID(void) {
	if (PID_Status == PID_OFF){
		;// use current PWMvalue
	} else {
		pwmValue += (int16_t) pid_Controller(referenceValue, measurementValue, &pidData, &pidDebugData);
		
		if (pwmValue < MIN_PWM_VALUE) {
			pwmValue = MIN_PWM_VALUE;
		}
		else if (pwmValue > MAX_PWM_VALUE) {
			pwmValue = MAX_PWM_VALUE-1;
		}
		//send current PWM value as soon as possible
		sendPID_flag=1;
	}
}


void calculatePID_callback(uint8_t timer)
{
	#ifdef PID_CALC_PERIOD_SECONDS
	static uint8_t seconds = 0;
	seconds++;
	if (seconds >= PID_CALC_PERIOD_SECONDS)
	{
	  seconds = 0;
	  calculatePID_flag = 1;
	}
	#else
	calculatePID_flag = 1;
	#endif
}

void sendPID(void)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PID_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_SLOWPWM;
	txMsg.Length = 3;
	txMsg.Data[0] = PWM_ID;
	txMsg.Data[1] = ((int16_t) (pwmValue)>>8)&0xff;
	txMsg.Data[2] = ((int16_t) (pwmValue))&0xff;
	
	StdCan_Put(&txMsg);
}

void sendPID_callback(uint8_t timer) {
  sendPID_flag=1;
}

#ifdef act_PID_SEND_DEBUG_TIMER
void sendPID_debug_callback(uint8_t timer)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PID_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_DEBUG;
	txMsg.Length = 8;

	txMsg.Data[0] = ((int16_t) (pidDebugData.P_term)>>8)&0xff;
	txMsg.Data[1] = ((int16_t) (pidDebugData.P_term))&0xff;
	txMsg.Data[2] = ((int16_t) (pidDebugData.I_term)>>8)&0xff;
	txMsg.Data[3] = ((int16_t) (pidDebugData.I_term))&0xff;
	txMsg.Data[4] = ((int16_t) (pidDebugData.D_term)>>8)&0xff;
	txMsg.Data[5] = ((int16_t) (pidDebugData.D_term))&0xff;
	txMsg.Data[6] = ((int16_t) (pidDebugData.Sum)>>8)&0xff;
	txMsg.Data[7] = ((int16_t) (pidDebugData.Sum))&0xff;
	StdCan_Put(&txMsg);
}
#endif

void act_PID_Init(void)
{
	if (EEDATA_OK)
	{
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.referenceValue, 0x0200, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleType, PID_TEMPERATURE_SENSOR_MODULE_TYPE, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleId, PID_TEMPERATURE_SENSOR_MODULE_ID, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorId, PID_TEMPERATURE_SENSOR, WITH_CRC);
	}
	referenceValue = (float) eeprom_read_word(EEDATA16.referenceValue)/64;
	sensorModuleType = eeprom_read_byte(EEDATA.sensorModuleType);
	sensorModuleId = eeprom_read_byte(EEDATA.sensorModuleId);
	sensorId = eeprom_read_byte(EEDATA.sensorId);

	#ifdef PID_CALC_PERIOD_SECONDS
	Timer_SetTimeout(act_PID_TIMER, 1000, TimerTypeFreeRunning, &calculatePID_callback);
	#else
	Timer_SetTimeout(act_PID_TIMER, PID_CALC_PERIOD_mSECONDS, TimerTypeFreeRunning, &calculatePID_callback);
	#endif

	pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData);
	pwmValue += (int16_t) pid_Controller(referenceValue, measurementValue, &pidData, &pidDebugData);
	PID_Status = PID_ON;
	pwmValue = DEFAULT_PWM_VALUE;
	Timer_SetTimeout(act_PID_SEND_TIMER, PID_SEND_PERIOD, TimerTypeFreeRunning, &sendPID_callback);
	#ifdef act_PID_SEND_DEBUG_TIMER
	  Timer_SetTimeout(act_PID_SEND_DEBUG_TIMER, PID_SEND_DEBUG_PERIOD, TimerTypeFreeRunning, &sendPID_debug_callback);
	#endif
}

void act_PID_Process(void)
{
	if (calculatePID_flag) {
		calculatePID();
		calculatePID_flag = 0;
	}
	if (sendPID_flag) {
		sendPID();
	}
}

void act_PID_HandleMessage(StdCan_Msg_t *rxMsg)
{

	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
			StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
			rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_PID &&
			rxMsg->Header.ModuleId == act_PID_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS:
			if (rxMsg->Data[0]==0)	//sensor id shall be zero
			{
				if (rxMsg->Length == 3)
				{
					if (0x80 == rxMsg->Data[1] && 0x00 == rxMsg->Data[2])
					{
						pid_Reset_Integrator(&pidData);
						PID_Status = PID_AUTO;
					}
					else
					{
						pid_Reset_Integrator(&pidData);
						eeprom_write_word_crc(EEDATA16.referenceValue, ((rxMsg->Data[1]<<8) + rxMsg->Data[2]), WITH_CRC);
						referenceValue = ((rxMsg->Data[1]<<8) + rxMsg->Data[2])/64;
					}
				} else
				{
					rxMsg->Data[1] = (uint8_t)0x00ff & (((uint32_t)(referenceValue*64))>>8);
					rxMsg->Data[2] = (uint8_t)0x00ff & ((uint32_t)referenceValue*64);
					StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
					rxMsg->Length = 3;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
				}

			}
		break;
		case CAN_MODULE_CMD_PID_CONFIG:
			if (rxMsg->Length == 3)
			{
				eeprom_write_byte_crc(EEDATA.sensorModuleType, rxMsg->Data[0] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.sensorModuleId, rxMsg->Data[1] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.sensorId, rxMsg->Data[2] , WITH_CRC);
				sensorModuleType = eeprom_read_byte(EEDATA.sensorModuleType);
				sensorModuleId = eeprom_read_byte(EEDATA.sensorModuleId);
				sensorId = eeprom_read_byte(EEDATA.sensorId);
			} else
			{
				rxMsg->Data[0] = eeprom_read_byte(EEDATA.sensorModuleType);
				rxMsg->Data[1] = eeprom_read_byte(EEDATA.sensorModuleId);
				rxMsg->Data[2] = eeprom_read_byte(EEDATA.sensorId);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 3;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			}
		break;
		}
	}


	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
				StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
				rxMsg->Header.ModuleType == sensorModuleType &&
				rxMsg->Header.ModuleId == sensorModuleId &&
				rxMsg->Header.Command == CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS &&
				rxMsg->Data[0] == sensorId)
		{
			if (0x80 == rxMsg->Data[1] && 0x00 == rxMsg->Data[2])
			{
				//Error on the temperature signal, do something
			}
			else
			{
				measurementValue = ((rxMsg->Data[1]<<8) + rxMsg->Data[2])/64;
			}

		}

}

void act_PID_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PID_ID;
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
