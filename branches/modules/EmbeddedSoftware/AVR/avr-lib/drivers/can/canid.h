#ifndef CANID_H_
#define CANID_H_

//---------------------------------------------------------------------------
// CAN ID definitions

#define CAN_MASK_CLASS		0x1E000000
#define CAN_SHIFT_CLASS		25
//------------------------------------
#define CAN_NMT			0x00UL
#define CAN_MASK_NMT_TYPE		0x00FF0000
#define CAN_SHIFT_NMT_TYPE		16
#define CAN_MASK_NMT_SID		0x0000FF00
#define CAN_SHIFT_NMT_SID		8
#define CAN_MASK_NMT_RID		0x000000FF
#define CAN_SHIFT_NMT_RID		0
#define CAN_NMT_TIME		0x00UL	//D0:3 UNIX_TIME, D4:7 {YY:6, MM:4, DD:5, hh:5, mm:6, ss:6}
#define CAN_NMT_RESET		0x04UL
#define CAN_NMT_BIOS_START		0x08UL	//D0: VER7:0, D1:VER15:8, D2: 0 if no app, >0 if app
#define CAN_NMT_PGM_START		0x0CUL	//D0: ADR7:0, D1:ADR15:8, D2:ADR23:16, D3:ADR31:24
#define CAN_NMT_PGM_DATA		0x10UL	//D0: OFS7:0, D1: OFS15:8, D(n+2): DATAn7:0 [n=0..x] [x=0..5]
#define CAN_NMT_PGM_END		0x14UL	//D0: CRC7:0, D1: CRC15:8
#define CAN_NMT_PGM_COPY		0x18UL	//D0: SRC7:0, D1:SRC15:8, D0: DST7:0, D1:DST15:8, D0: LEN7:0, D1:LEN15:8
#define CAN_NMT_PGM_ACK		0x1CUL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_PGM_NACK		0x20UL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_START_APP		0x24UL
#define CAN_NMT_APP_START		0x28UL	//D0: ID7:0, D1:ID15:8, D2: VER7:0, D3:VER15:8
#define CAN_NMT_HEARTBEAT		0x2CUL
//------------------------------------

// This is old definitions, not needed for modulebased applications
#define CAN_SNS			0x02UL
#define CAN_MASK_SNS_TYPE		0x01FF8000
#define CAN_SHIFT_SNS_TYPE		15
#define CAN_MASK_SNS_ID		0x00007E00
#define CAN_SHIFT_SNS_ID		9
#define CAN_MASK_SNS_SID		0x000001FF
#define CAN_SHIFT_SNS_SID		0

#define CAN_ACT			0x04UL
#define CAN_MASK_ACT_TYPE		0x01FF8000
#define CAN_SHIFT_ACT_TYPE		15
#define CAN_MASK_ACT_ID		0x00007E00
#define CAN_SHIFT_ACT_ID		9
#define CAN_MASK_ACT_SID		0x000001FF
#define CAN_SHIFT_ACT_SID		0

#define CAN_PKT				0x06UL

#define CAN_CON				0x08UL

#define CAN_TST				0x0FUL
//--------------------------------------------------------------------


//--------------------------------------------------- //
// New class definitions for modulebased applications //
//--------------------------------------------------- //

// Module classes
//------------------------------------
#define CAN_CLASS_MODULE_NMT		0x0BUL		// If we move the functionality for this to BIOS we should use CAN_NMT instead
#define CAN_CLASS_MODULE_ACT		0x0CUL
#define CAN_CLASS_MODULE_SNS		0x0DUL
#define CAN_CLASS_MODULE_DEF		0x0EUL

// Module frame direction
//------------------------------------
#define DIR_FROM_OWNER			1		// If a module sends a packet
#define DIR_TO_OWNER			0		// If a module sends a packet to another module

// Module types
//------------------------------------
#define CAN_TYPE_MODULE_def_default	0x01
#define CAN_TYPE_MODULE_sns_ds18x20	0x03
#define CAN_TYPE_MODULE_sns_FOST02	0x05
#define CAN_TYPE_MODULE_sns_BusVoltage	0x06
#define CAN_TYPE_MODULE_act_hd44789	0x04

// Module commands
//------------------------------------
//#define CAN_CMD_MODULE_NMT_LIST		0x00UL		// Tell the application to report which modules it has




//---------------------------------------------------------------------------
// CAN ID definitions for NMT messages

#define CAN_ID_NMT_BIOS_START  ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_BIOS_START << CAN_SHIFT_NMT_TYPE))
#define CAN_ID_NMT_PGM_ACK     ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_ACK << CAN_SHIFT_NMT_TYPE))
#define CAN_ID_NMT_PGM_NACK    ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_NACK << CAN_SHIFT_NMT_TYPE))

#define CAN_APPTYPES_IRRECEIVER     0xf010
#define CAN_APPTYPES_RELAY          0xf015
#define CAN_APPTYPES_SERVO          0xf020
#define CAN_APPTYPES_SENSOR			0xf025
#define CAN_APPTYPES_IRTRANSMITTER  0xf030
#define CAN_APPTYPES_RGBLED			0xf035
#define CAN_APPTYPES_FANCONTROLLER	0xf040

// CAN ID definitions for SNS messages
#define SNS_TYPE_STATUS             0x10L
#define SNS_TYPE_IR                 0x12L
#define SNS_TYPE_TEMPERATURE        0x14L
#define SNS_TYPE_LIGHT              0x18L
#define SNS_TYPE_PHONENR            0x1aL
#define SNS_TYPE_PRESSURE			0x1bL
#define SNS_TYPE_VOLTAGE			0x1cL
#define SNS_TYPE_ROTSPEED			0x1dL
#define SNS_TYPE_VELOCITY			0x1eL
#define SNS_TYPE_FUELCONS			0x1fL


#define SNS_ID_RELAY_STATUS           0x0BL
#define SNS_ID_SERVO_STATUS           0x0dL

//Definitions of precision of different sensors
#define SNS_DECPOINT_TEMPERATURE	0x04
#define SNS_DECPOINT_PRESSURE		0x06
#define SNS_DECPOINT_ROTSPEED		0x02
#define SNS_DECPOINT_VOLTAGE		0x06
#define SNS_DECPOINT_VELOCITY		0x02
#define SNS_DECPOINT_FUELCONS		0x06


// CAN ID definitions for ACT messages
#define ACT_TYPE_RELAY         	0x01L
#define ACT_TYPE_SERVO         	0x02L
#define ACT_TYPE_RGBLED		   	0x03L
#define ACT_TYPE_IR            	0x04L
#define ACT_TYPE_LCD			0x05L
#define ACT_TYPE_FAN			0x06L



#endif /*CANID_H_*/
