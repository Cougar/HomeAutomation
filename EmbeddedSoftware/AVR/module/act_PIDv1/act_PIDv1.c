
#include "act_PIDv1.h"
#include <drivers/misc/PID_v1.h>
#include <drivers/misc/PID_AutoTune.h>

#if act_PIDv1_USEEEPROM==1
#include "act_PIDv1_eeprom.h"
struct eeprom_act_PIDv1 EEMEM eeprom_act_PIDv1 =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0x00000000,	// referenceValue
		CAN_MODULE_TYPE_SNS_DS18X20,	//SensorModuleType
		0x00,	//SensorModuleId
		0x00,	//SensorId
		0x00000000,	//uint32_t K_P; (float)
		0x00000000,	//uint32_t K_I; (float)
		0x00000000,	//uint32_t K_D; (float)
		0x00,	//uint8_t TimeMsOrS;
		0x0000,	//uint16_t Time;
		CAN_MODULE_TYPE_ACT_SOFTPWM,	//ActuatorModuleType
		0x00,	//ActuatorModuleId
		0x00,	//ActuatorId
		10,	//SendPeriod
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif
#define PID_ON 1
#define PID_OFF 0
#define PID_AUTO 2

//! Parameters for regulator
PidType pid;

//struct PID_DATA pidData;
//struct PIDv1_DEBUG_DATA pidDebugData;

uint8_t sensorModuleType, sensorModuleId,sensorId;
uint8_t PID_Status;
uint8_t calculatePID_flag,sendPID_flag = 0;
uint16_t pwmValue=0;
float referenceValue, measurementValue, outputValue;

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
		txMsg.Data[1] = ( ((uint16_t)outputValue)>>8)&0xff;
		txMsg.Data[2] = ( ((uint16_t)outputValue))&0xff;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	}
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PIDv1_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_PID_STATUS;
	txMsg.Length = 8;
	txMsg.Data[0] = (uint8_t)0x00ff & (((uint32_t)(measurementValue*64))>>8);
	txMsg.Data[1] = (uint8_t)0x00ff & ((uint32_t)(measurementValue*64));
	txMsg.Data[2] = (uint8_t)0x00ff & (((uint32_t)(referenceValue*64))>>8);
	txMsg.Data[3] = (uint8_t)0x00ff & ((uint32_t)(referenceValue*64));
	//uint16_t tempPWM =(uint16_t) (pwmValue*10000);
		txMsg.Data[4] = (uint8_t)( ((uint16_t)outputValue)>>8)&0xff;
		txMsg.Data[5] = (uint8_t)( ((uint16_t)outputValue))&0xff;
	txMsg.Data[6] = ((int16_t) (PID_GetITerm(&pid))>>8)&0xff;
	txMsg.Data[7] = ((int16_t) (PID_GetITerm(&pid)))&0xff;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

void sendPID_callback(uint8_t timer) {
  sendPID_flag=1;
}

#ifdef act_PIDv1_SEND_DEBUG_TIMER
void sendPID_debug_callback(uint8_t timer)
{
	
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PIDv1_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_DEBUG;
	txMsg.Length = 8;

	txMsg.Data[0] = ((int16_t) (PID_GetPTerm(&pid))>>8)&0xff;
	txMsg.Data[1] = ((int16_t) (PID_GetPTerm(&pid)))&0xff;
	txMsg.Data[2] = ((int16_t) (PID_GetITerm(&pid))>>8)&0xff;
	txMsg.Data[3] = ((int16_t) (PID_GetITerm(&pid)))&0xff;
	txMsg.Data[4] = ((int16_t) (PID_GetDTerm(&pid))>>8)&0xff;
	txMsg.Data[5] = ((int16_t) (PID_GetDTerm(&pid)))&0xff;
	//txMsg.Data[6] = ((int16_t) (pidDebugData.Sum)>>8)&0xff;
	//txMsg.Data[7] = ((int16_t) (pidDebugData.Sum))&0xff;
	txMsg.Data[6] = 0;
	txMsg.Data[7] = 0;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; 
	txMsg.Header.ModuleId = act_PIDv1_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_P_I_TERM;
	txMsg.Length = 8;
	FloatType data = PID_GetPTerm(&pid);
	data = 1.0f;
	uint8_t *ptr;
	ptr = (uint8_t*)&data;
	txMsg.Data[0] = ptr[0];
	txMsg.Data[1] = ptr[1];
	txMsg.Data[2] = ptr[2];
	txMsg.Data[3] = ptr[3];
	data = PID_GetITerm(&pid);
	data = -2.0f;
	ptr = (uint8_t*)&data;
	txMsg.Data[4] = ptr[0];
	txMsg.Data[5] = ptr[1];
	txMsg.Data[6] = ptr[2];
	txMsg.Data[7] = ptr[3];
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; 
	txMsg.Header.ModuleId = act_PIDv1_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PID_D_TERM_OUT;
	txMsg.Length = 8;
	data = PID_GetDTerm(&pid);
	ptr = (uint8_t*)&data;
	txMsg.Data[0] = ptr[0];
	txMsg.Data[1] = ptr[1];
	txMsg.Data[2] = ptr[2];
	txMsg.Data[3] = ptr[3];
	ptr = (uint8_t*)&outputValue;
	txMsg.Data[4] = ptr[0];
	txMsg.Data[5] = ptr[1];
	txMsg.Data[6] = ptr[2];
	txMsg.Data[7] = ptr[3];
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);	
	
//printf("PID: P:%d, I:%d, D:%d, S:%d\n",(int16_t)pidDebugData.P_term,(int16_t)pidDebugData.I_term,(int16_t)pidDebugData.D_term,(int16_t)pidDebugData.Sum);
}
#endif
void act_PIDv1_Init(void)
{
#if act_PIDv1_USEEEPROM==1
	if (EEDATA_OK)
	{
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_dword_crc(EEDATA32.referenceValue, 20.0f, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleType, PIDv1_TEMPERATURE_SENSOR_MODULE_TYPE, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorModuleId, PIDv1_TEMPERATURE_SENSOR_MODULE_ID, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sensorId, PIDv1_TEMPERATURE_SENSOR, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.K_P, 850.0f, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.K_I, 0.5f, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.K_D, 0.1f, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.TimeMsOrS, DEFAULT_PIDv1_CALC_PERIOD_UNIT, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.Time, DEFAULT_PIDv1_CALC_PERIOD, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorModuleType, PIDv1_PWM_ACTUATOR_MODULE_TYPE, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorModuleId, PIDv1_PWM_ACTUATOR_MODULE_ID, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.actuatorId, PIDv1_PWM_ACTUATOR, WITHOUT_CRC);
	  eeprom_write_byte_crc(EEDATA.sendPeriod, 10, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
	
#else
#error this driver needs EEPROM support	
#endif
	
	referenceValue = (float) eeprom_read_dword(EEDATA32.referenceValue);
	sensorModuleType = eeprom_read_byte(EEDATA.sensorModuleType);
	sensorModuleId = eeprom_read_byte(EEDATA.sensorModuleId);
	sensorId = eeprom_read_byte(EEDATA.sensorId);
	
	PID_init(&pid, &measurementValue, &outputValue, &referenceValue, (float) eeprom_read_dword(EEDATA32.K_P), (float) eeprom_read_dword(EEDATA32.K_I), (float) eeprom_read_dword(EEDATA32.K_D), PID_Direction_Direct);

	if (eeprom_read_byte(EEDATA.TimeMsOrS) == CAN_MODULE_ENUM_PID_CONFIG_PARAMETER_TIMEUNIT_S) {
		PID_SetSampleTime(&pid, (uint32_t)(eeprom_read_word(EEDATA16.Time))*1000);
	} else {
		PID_SetSampleTime(&pid, (uint32_t)(eeprom_read_word(EEDATA16.Time)));
	}

	outputValue = DEFAULT_PWM_VALUE;

	PID_SetMode(&pid, PID_Mode_Automatic);
	
	Timer_SetTimeout(act_PIDv1_SEND_TIMER, eeprom_read_byte(EEDATA.sendPeriod)*1000, TimerTypeFreeRunning, &sendPID_callback);
	#ifdef act_PIDv1_SEND_DEBUG_TIMER
	  Timer_SetTimeout(act_PIDv1_SEND_DEBUG_TIMER, PIDv1_SEND_DEBUG_PERIOD, TimerTypeFreeRunning, &sendPID_debug_callback);
	#endif
}

void act_PIDv1_Process(void)
{
	PID_Compute(&pid);
	
	if (sendPID_flag) {
		sendPID();
		sendPID_flag= 0;
	}
}

void act_PIDv1_HandleMessage(StdCan_Msg_t *rxMsg)
{
	FloatType data2;
	uint8_t *ptr;
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
			StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
			rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_PID &&
			rxMsg->Header.ModuleId == act_PIDv1_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS:
			if (rxMsg->Data[0]==0)	//sensor id shall be zero
			{
				printf("New setpoint with: %X %X\n",rxMsg->Data[1],rxMsg->Data[2]);

				if (rxMsg->Length == 3)
				{
					if (0x80 == rxMsg->Data[1] && 0x00 == rxMsg->Data[2]) //512 degrees
					{
						//pid_Reset_Integrator(&pidData);
						PID_Status = PID_AUTO;
					}
					else
					{
						//pid_Reset_Integrator(&pidData);
						referenceValue = (((float)((rxMsg->Data[1]<<8) + rxMsg->Data[2]))/64);
						eeprom_write_dword_crc(EEDATA32.referenceValue, referenceValue, WITH_CRC);
					}
				}
				rxMsg->Data[1] = (uint8_t)0x00ff & (((uint32_t)(referenceValue*64))>>8);
				rxMsg->Data[2] = (uint8_t)0x00ff & ((uint32_t)referenceValue*64);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 3;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
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
				Timer_SetTimeout(act_PIDv1_SEND_TIMER, rxMsg->Data[0]*1000, TimerTypeFreeRunning, &sendPID_callback);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			}
		break;

		case CAN_MODULE_CMD_PID_CONFIG_PARAMETER:
			if (rxMsg->Length == 8)
			{
				eeprom_write_dword_crc(EEDATA32.K_P, (((float)((rxMsg->Data[0]<<8) + rxMsg->Data[1]))/64), WITHOUT_CRC);
				eeprom_write_dword_crc(EEDATA32.K_I, (((float)((rxMsg->Data[2]<<8) + rxMsg->Data[3]))/64), WITHOUT_CRC);
				eeprom_write_dword_crc(EEDATA32.K_D, (((float)((rxMsg->Data[4]<<8) + rxMsg->Data[5]))/64), WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.TimeMsOrS, ((rxMsg->Data[6]&0x80)>>7), WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.Time, (uint16_t)rxMsg->Data[7]+((rxMsg->Data[6]&0x7f)<<8), WITH_CRC);
				if (eeprom_read_byte(EEDATA.TimeMsOrS) == 0) {
					//Timer_SetTimeout(act_PIDv1_TIMER, 1000, TimerTypeFreeRunning, &calculatePID_callback);
				} else {
					//Timer_SetTimeout(act_PIDv1_TIMER, eeprom_read_word(EEDATA16.Time), TimerTypeFreeRunning, &calculatePID_callback);
				}
				PID_SetTunings(&pid, (float) eeprom_read_dword(EEDATA32.K_P), (float) eeprom_read_dword(EEDATA32.K_I), (float) eeprom_read_dword(EEDATA32.K_D));
				PID_Status = PID_ON;
				pwmValue = DEFAULT_PWM_VALUE;
			}
			rxMsg->Data[0] = ((int16_t) (PID_GetKp(&pid))>>8)&0xff;
			rxMsg->Data[1] = ((int16_t) (PID_GetKp(&pid)))&0xff;
			rxMsg->Data[2] = ((int16_t) (PID_GetKi(&pid))>>8)&0xff;
			rxMsg->Data[3] = ((int16_t) (PID_GetKi(&pid)))&0xff;
			rxMsg->Data[4] = ((int16_t) (PID_GetKd(&pid))>>8)&0xff;
			rxMsg->Data[5] = ((int16_t) (PID_GetKd(&pid)))&0xff;
			rxMsg->Data[6] = (0x7f&(eeprom_read_word(EEDATA16.Time)>>8));
			rxMsg->Data[7] = (0xff&(eeprom_read_word(EEDATA16.Time)));
			rxMsg->Data[6] |= (0x80&(eeprom_read_byte(EEDATA.TimeMsOrS))<<7);
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			rxMsg->Length = 8;
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		break;
		
		case CAN_MODULE_CMD_PID_CONFIG_PARAMETER_D_T:
			if (rxMsg->Length == 8)
			{
				float *data = (float*)&rxMsg->Data[0];
				
				eeprom_write_dword_crc(EEDATA32.K_D, *data, WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.TimeMsOrS, ((rxMsg->Data[6]&0x80)>>7), WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.Time, (uint16_t)rxMsg->Data[7]+((rxMsg->Data[6]&0x7f)<<8), WITH_CRC);
				PID_SetTunings(&pid, PID_GetKp(&pid), PID_GetKi(&pid), (float) eeprom_read_dword(EEDATA32.K_D));
				PID_Status = PID_ON;
				pwmValue = DEFAULT_PWM_VALUE;
			}
			data2 = PID_GetKd(&pid);
			ptr = (uint8_t*)&data2;
			rxMsg->Data[0] = ptr[0];
			rxMsg->Data[1] = ptr[1];
			rxMsg->Data[2] = ptr[2];
			rxMsg->Data[3] = ptr[3];
			rxMsg->Data[4] = 0u;
			rxMsg->Data[5] = 0u;
			rxMsg->Data[6] = (0x7f&(eeprom_read_word(EEDATA16.Time)>>8));
			rxMsg->Data[7] = (0xff&(eeprom_read_word(EEDATA16.Time)));
			rxMsg->Data[6] |= (0x80&(eeprom_read_byte(EEDATA.TimeMsOrS))<<7);
			
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			rxMsg->Length = 8;
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		break;
		case CAN_MODULE_CMD_PID_CONFIG_PARAMETER_P_I:
			if (rxMsg->Length == 8)
			{
				float *data = (float*)&rxMsg->Data[0];
				
				eeprom_write_dword_crc(EEDATA32.K_P, *data, WITHOUT_CRC);
				data = (float*)&rxMsg->Data[4];
				eeprom_write_dword_crc(EEDATA32.K_I, *data, WITH_CRC);
				
				PID_SetTunings(&pid, (float) eeprom_read_dword(EEDATA32.K_P), (float) eeprom_read_dword(EEDATA32.K_I), PID_GetKd(&pid));
				PID_Status = PID_ON;
				pwmValue = DEFAULT_PWM_VALUE;
			}
			data2 = PID_GetKp(&pid);
			ptr = (uint8_t*)&data2;
			rxMsg->Data[0] = ptr[0];
			rxMsg->Data[1] = ptr[1];
			rxMsg->Data[2] = ptr[2];
			rxMsg->Data[3] = ptr[3];
			data2 = PID_GetKi(&pid);
			ptr = (uint8_t*)&data2;
			rxMsg->Data[4] = ptr[0];
			rxMsg->Data[5] = ptr[1];
			rxMsg->Data[6] = ptr[2];
			rxMsg->Data[7] = ptr[3];
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			rxMsg->Length = 8;
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
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

void act_PIDv1_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_PIDv1_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
