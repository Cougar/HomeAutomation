
#include "tst_TestPrgRig.h"

uint8_t DUTconnectState=DUTCONNSTATE_IDLE;


/*
This module will generate a lot of warnings when compiling due to CAN_PRINTF-define in configs, this is ok
To display the output of the rig use Atom's main window or the prinf-script in PcSoftware/scripts/canPrintf/

first go to personal folder
cd EmbeddedSoftware/AVR/personal/

then for each new DUT run these commands
svn export template_module modDUT; cd modDUT; make bios; make installbios; sleep 2; ./ModuleManager --add=sns_BusVoltage; make; make install; cd ..; rm -r modDUT

*/


#if tst_TestPrgRig_USEEEPROM==1
#include "tst_TestPrgRig_eeprom.h"
struct eeprom_tst_TestPrgRig EEMEM eeprom_tst_TestPrgRig = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void tst_TestPrgRig_Init(void)
{
#if tst_TestPrgRig_USEEEPROM==1
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA.y, 0x1234, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif  

	ADC_Init();
	Timer_SetTimeout(tst_TestPrgRig_REPORT_TIMER, tst_TestPrgRig_ReportInterval , TimerTypeFreeRunning, 0);

	gpio_set_in(tst_TestPrgRig_CONNPIN);
	gpio_set_pullup(tst_TestPrgRig_CONNPIN);
	gpio_set_out(tst_TestPrgRig_ENABLEPIN);
	
	DUTconnectState=DUTCONNSTATE_IDLE;
}

void tst_TestPrgRig_Process(void)
{
	/*uint16_t VccVoltage = ADC_Get(tst_TestPrgRig_5VFEEDBACKAD);
	VccVoltage = (VccVoltage & 0x03ff) * ADC_FACTOR;
	uint16_t Current = ADC_Get(tst_TestPrgRig_CURRFEEDBACKAD)>>1;
	*/
	uint8_t sendData = 0;
	
	uint16_t VccVoltage = ADC_Get(tst_TestPrgRig_5VFEEDBACKAD);
	VccVoltage = (VccVoltage & 0x03ff) * ADC_FACTOR;
	VccVoltage = VccVoltage>>1;
	uint16_t Current = ADC_Get(tst_TestPrgRig_CURRFEEDBACKAD)>>1;

	if (VccVoltage > 6000)
	{
		DUTconnectState=DUTCONNSTATE_FAILED_VOLT;
		/* disable power on DUT */
		gpio_clr_pin(tst_TestPrgRig_ENABLEPIN);
		sendData = 1;
	} 
	else if (Current > 40)
	{
		DUTconnectState=DUTCONNSTATE_FAILED_CURR;
		/* disable power on DUT */
		gpio_clr_pin(tst_TestPrgRig_ENABLEPIN);
		sendData = 1;
	}

	if (Timer_Expired(tst_TestPrgRig_REPORT_TIMER)) {
		if (!gpio_get_state(tst_TestPrgRig_CONNPIN) && DUTconnectState==DUTCONNSTATE_IDLE)
		{
			DUTconnectState=DUTCONNSTATE_QUAL1;
		}
		else if (!gpio_get_state(tst_TestPrgRig_CONNPIN) && DUTconnectState==DUTCONNSTATE_QUAL1)
		{
			DUTconnectState=DUTCONNSTATE_QUAL2;
		}
		else if (!gpio_get_state(tst_TestPrgRig_CONNPIN) && DUTconnectState==DUTCONNSTATE_QUAL2)
		{
			DUTconnectState=DUTCONNSTATE_ENABLED;
			/* enable power on DUT */
			gpio_set_pin(tst_TestPrgRig_ENABLEPIN);
		}
		else if (gpio_get_state(tst_TestPrgRig_CONNPIN))
		{
			DUTconnectState=DUTCONNSTATE_IDLE;
			/* disable power on DUT */
			gpio_clr_pin(tst_TestPrgRig_ENABLEPIN);
		}
		sendData = 1;
	}
		
	if (sendData == 1)
	{
		printf("V: %4dmV, C: %2dmA", VccVoltage, Current);
		if (!gpio_get_state(tst_TestPrgRig_CONNPIN))
		{
			printf(", DUT    connected");
		}
		else
		{
			printf(", DUT disconnected");
		}
		switch (DUTconnectState)
		{
			case 0:
				printf(", S: Idle");
				break;
			case 1:
				printf(", S: Q1");
				break;
			case 2:
				printf(", S: Q2");
				break;
			case 3:
				printf(", S: Enabled");
				break;
			case 254:
				printf(", S: Fail Curr");
				break;
			case 255:
				printf(", S: Fail Volt");
				break;
		}
		printf("\n");
		
		/*StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_TST);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
		txMsg.Header.ModuleId = tst_TestPrgRig_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_DEBUG_TESTERDATA;
		txMsg.Length = 6;
		txMsg.Data[0] = (VccVoltage>>(ADC_SCALE-6+8))&0xff;
		txMsg.Data[1] = (VccVoltage>>(ADC_SCALE-6))&0xff;
		txMsg.Data[2] = (Current>>(8-6))&0xff;
		txMsg.Data[3] = (Current<<(6))&0xff;
		txMsg.Data[4] = (!gpio_get_state(tst_TestPrgRig_CONNPIN))&0xff;
		txMsg.Data[5] = (DUTconnectState)&0xff;
		StdCan_Put(&txMsg);
		*/
	}
}

void tst_TestPrgRig_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == tst_TestPrgRig_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void tst_TestPrgRig_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_TST);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	txMsg.Header.ModuleId = tst_TestPrgRig_ID;
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
