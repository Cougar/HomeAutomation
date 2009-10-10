
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
		CAN_MODULE_TYPE_SNS_DS18X20,	//SensorModuleType
		0x00,	//SensorModuleId
		0x00,	//SensorId
		0x0000,	//uint16_t K_P;
		0x0000,	//uint16_t K_I;
		0x0000,	//uint16_t K_D;
		0x00,	//uint8_t TimeMsOrS;
		0x0000,	//uint16_t Time;
		CAN_MODULE_TYPE_ACT_SOFTPWM,	//ActuatorModuleType
		0x00,	//ActuatorModuleId
		0x00,	//ActuatorId
		10,	//SendPeriod

	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 

void calculatePID(void) {
	if (PID_Status == PID_OFF){
		;// use current PWMvalue
	} else {
//printf("PWM: %d\n",pwmValue);
uint16_t tempPwm = pwmValue;
		tempPwm += (int16_t) pid_Controller(referenceValue, measurementValue, &pidData, &pidDebugData);
//printf("InOut: ref: %d, meas: %d\n",(int16_t) (referenceValue*10), (int16_t) (measurementValue*10));
//printf("PWM: %d\n",pwmValue);

		if (tempPwm < MIN_PWM_VALUE) {
			tempPwm = MIN_PWM_VALUE;
		}
		else if (tempPwm > MAX_PWM_VALUE) {
			tempPwm = MAX_PWM_VALUE-1;
		}
		cli();
		pwmValue = tempPwm;
		sei();
//printf("PWM: %d\n",pwmValue);
		//send current PWM value as soon as possible
		sendPID_flag=1;
	}

}


void calculatePID_callback(uint8_t timer)
{

	if (eeprom_read_byte(EEDATA.TimeMsOrS) == CAN_MODULE_ENUM_PID_CONFIG_PARAMETER_TIMEUNIT_SECONDS) {
		static uint8_t seconds = 0;
		seconds++;
		if (seconds >= eeprom_read_word(EEDATA16.Time))
		{
			seconds = 0;
			calculatePID_flag = 1;
		}
	} else {
		calculatePID_flag = 1;
	}
}

void sendPID(void)
{
	if (eeprom_read_byte(EEDATA.actuatorModuleType) != 0) {	
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_TO_OWNER);
		txMsg.Header.ModuleType = eeprom_read_byte(EEDATA.actuatorModuleType); ///TODO: Change this to the actual module type
		txMsg.Header.ModuleId = eeprom_read_byte(EEDATA.actuatorModuleId);
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
		txMsg.Length = 3;
		txMsg.Data[0] = eeprom_read_byte(EEDATA.actuatorId);
		//uint16_t tempPWM =(uint16_t) (pwmValue*10000);
		txMsg.Data[1] = ( (pwmValue)>>8)&0xff;
		txMsg.Data[2] = ( (pwmValue))&0xff;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	}
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PID_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_PID_STATUS;
	txMsg.Length = 8;
	txMsg.Data[0] = (uint8_t)0x00ff & (((uint32_t)(measurementValue*64))>>8);
	txMsg.Data[1] = (uint8_t)0x00ff & ((uint32_t)measurementValue*64);
	txMsg.Data[2] = (uint8_t)0x00ff & (((uint32_t)(referenceValue*64))>>8);
	txMsg.Data[3] = (uint8_t)0x00ff & ((uint32_t)referenceValue*64);
	//uint16_t tempPWM =(uint16_t) (pwmValue*10000);
		txMsg.Data[4] = ( (pwmValue)>>8)&0xff;
		txMsg.Data[5] = ( (pwmValue))&0xff;
	txMsg.Data[6] = ((int16_t) (pidDebugData.Sum)>>8)&0xff;
	txMsg.Data[7] = ((int16_t) (pidDebugData.Sum))&0xff;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
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
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
//printf("PID: P:%d, I:%d, D:%d, S:%d\n",(int16_t)pidDebugData.P_term,(int16_t)pidDebugData.I_term,(int16_t)pidDebugData.D_term,(int16_t)pidDebugData.Sum);
}
#endif

void act_PID_Init(void)
{
	if (EEDATA_OK)
	{
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.referenceValue, 0x0520, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleType, PID_TEMPERATURE_SENSOR_MODULE_TYPE, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleId, PID_TEMPERATURE_SENSOR_MODULE_ID, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorId, PID_TEMPERATURE_SENSOR, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.K_P, (int16_t)(DEFAULT_K_P*64), WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.K_I, (int16_t)(DEFAULT_K_I*64), WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.K_D, (int16_t)(DEFAULT_K_D*64), WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.TimeMsOrS, DEFAULT_PID_CALC_PERIOD_UNIT, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.Time, DEFAULT_PID_CALC_PERIOD, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorModuleType, PID_PWM_ACTUATOR_MODULE_TYPE, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorModuleId, PID_PWM_ACTUATOR_MODULE_ID, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorId, PID_PWM_ACTUATOR, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sendPeriod, 10, WITH_CRC);
	}
	referenceValue = (float) eeprom_read_word(EEDATA16.referenceValue)/64;
	sensorModuleType = eeprom_read_byte(EEDATA.sensorModuleType);
	sensorModuleId = eeprom_read_byte(EEDATA.sensorModuleId);
	sensorId = eeprom_read_byte(EEDATA.sensorId);


	if (eeprom_read_byte(EEDATA.TimeMsOrS) == CAN_MODULE_ENUM_PID_CONFIG_PARAMETER_TIMEUNIT_SECONDS) {
		Timer_SetTimeout(act_PID_TIMER, 1000, TimerTypeFreeRunning, &calculatePID_callback);
	} else {
		Timer_SetTimeout(act_PID_TIMER, eeprom_read_word(EEDATA16.Time), TimerTypeFreeRunning, &calculatePID_callback);
	}

	pid_Init((float) eeprom_read_word(EEDATA16.K_P)/64 * SCALING_FACTOR, (float) eeprom_read_word(EEDATA16.K_I)/64 * SCALING_FACTOR , (float) eeprom_read_word(EEDATA16.K_D)/64 * SCALING_FACTOR , &pidData);
	pwmValue += (int16_t) pid_Controller(referenceValue, measurementValue, &pidData, &pidDebugData);
	PID_Status = PID_ON;
	pwmValue = DEFAULT_PWM_VALUE;
	Timer_SetTimeout(act_PID_SEND_TIMER, eeprom_read_byte(EEDATA.sendPeriod)*1000, TimerTypeFreeRunning, &sendPID_callback);
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
		sendPID_flag= 0;
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
		case CAN_MODULE_CMD_PID_CONFIG_SENSOR:
			if (rxMsg->Length == 3)
			{
				eeprom_write_byte_crc(EEDATA.sensorModuleType, rxMsg->Data[0] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.sensorModuleId, rxMsg->Data[1] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.sensorId, rxMsg->Data[2] , WITH_CRC);
				sensorModuleType = eeprom_read_byte(EEDATA.sensorModuleType);
				sensorModuleId = eeprom_read_byte(EEDATA.sensorModuleId);
				sensorId = eeprom_read_byte(EEDATA.sensorId);
			}
			rxMsg->Data[0] = eeprom_read_byte(EEDATA.sensorModuleType);
			rxMsg->Data[1] = eeprom_read_byte(EEDATA.sensorModuleId);
			rxMsg->Data[2] = eeprom_read_byte(EEDATA.sensorId);
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			rxMsg->Length = 3;
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		break;
		case CAN_MODULE_CMD_PID_CONFIG_ACTUATOR:
			if (rxMsg->Length == 3)
			{
				eeprom_write_byte_crc(EEDATA.actuatorModuleType, rxMsg->Data[0] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.actuatorModuleId, rxMsg->Data[1] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.actuatorId, rxMsg->Data[2] , WITH_CRC);
			} 
			rxMsg->Data[0] = eeprom_read_byte(EEDATA.actuatorModuleType);
			rxMsg->Data[1] = eeprom_read_byte(EEDATA.actuatorModuleId);
			rxMsg->Data[2] = eeprom_read_byte(EEDATA.actuatorId);
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			rxMsg->Length = 3;
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		break;
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length == 1) {
				if (65 <= rxMsg->Data[0])
					rxMsg->Data[0]=65;
				if (0 == rxMsg->Data[0])
					rxMsg->Data[0]=1;
				eeprom_write_byte_crc(EEDATA.sendPeriod, rxMsg->Data[0], WITH_CRC);
				Timer_SetTimeout(act_PID_SEND_TIMER, rxMsg->Data[0]*1000, TimerTypeFreeRunning, &sendPID_callback);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			}
		break;

		case CAN_MODULE_CMD_PID_CONFIG_PARAMETER:
			if (rxMsg->Length == 8)
			{
				eeprom_write_word_crc(EEDATA16.K_P, (uint16_t)rxMsg->Data[1]+(rxMsg->Data[0]<<8), WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.K_I, (uint16_t)rxMsg->Data[3]+(rxMsg->Data[2]<<8), WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.K_D, (uint16_t)rxMsg->Data[5]+(rxMsg->Data[4]<<8), WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.TimeMsOrS, ((rxMsg->Data[6]&0x80)>>7), WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.Time, (uint16_t)rxMsg->Data[7]+((rxMsg->Data[6]&0x7f)<<8), WITH_CRC);
				if (eeprom_read_byte(EEDATA.TimeMsOrS) == 0) {
					Timer_SetTimeout(act_PID_TIMER, 1000, TimerTypeFreeRunning, &calculatePID_callback);
				} else {
					Timer_SetTimeout(act_PID_TIMER, eeprom_read_word(EEDATA16.Time), TimerTypeFreeRunning, &calculatePID_callback);
				}

				pid_Init((float) eeprom_read_word(EEDATA16.K_P)/64 * SCALING_FACTOR, (float) eeprom_read_word(EEDATA16.K_I)/64 * SCALING_FACTOR , (float) eeprom_read_word(EEDATA16.K_D)/64 * SCALING_FACTOR , &pidData);
				pwmValue += (int16_t) pid_Controller(referenceValue, measurementValue, &pidData, &pidDebugData);
				PID_Status = PID_ON;
				pwmValue = DEFAULT_PWM_VALUE;

				rxMsg->Data[0] = (0xff&(eeprom_read_word(EEDATA16.K_P)>>8));
				rxMsg->Data[1] = (0xff&(eeprom_read_word(EEDATA16.K_P)));
				rxMsg->Data[2] = (0xff&(eeprom_read_word(EEDATA16.K_I)>>8));
				rxMsg->Data[3] = (0xff&(eeprom_read_word(EEDATA16.K_I)));
				rxMsg->Data[4] = (0xff&(eeprom_read_word(EEDATA16.K_D)>>8));
				rxMsg->Data[5] = (0xff&(eeprom_read_word(EEDATA16.K_D)));
				rxMsg->Data[6] = (0xff&(eeprom_read_word(EEDATA16.Time)>>8));
				rxMsg->Data[7] = (0xff&(eeprom_read_word(EEDATA16.Time)));
				rxMsg->Data[6] |= (0x80&(eeprom_read_byte(EEDATA.TimeMsOrS))<<7);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 8;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			} else
			{
				rxMsg->Data[0] = (0xff&(eeprom_read_word(EEDATA16.K_P)>>8));
				rxMsg->Data[1] = (0xff&(eeprom_read_word(EEDATA16.K_P)));
				rxMsg->Data[2] = (0xff&(eeprom_read_word(EEDATA16.K_I)>>8));
				rxMsg->Data[3] = (0xff&(eeprom_read_word(EEDATA16.K_I)));
				rxMsg->Data[4] = (0xff&(eeprom_read_word(EEDATA16.K_D)>>8));
				rxMsg->Data[5] = (0xff&(eeprom_read_word(EEDATA16.K_D)));
				rxMsg->Data[6] = (0xff&(eeprom_read_word(EEDATA16.Time)>>8));
				rxMsg->Data[7] = (0xff&(eeprom_read_word(EEDATA16.Time)));
				rxMsg->Data[6] |= (0x80&(eeprom_read_byte(EEDATA.TimeMsOrS))<<7);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 8;
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
				measurementValue = ((float)((rxMsg->Data[1]<<8) + rxMsg->Data[2]))/64;
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
