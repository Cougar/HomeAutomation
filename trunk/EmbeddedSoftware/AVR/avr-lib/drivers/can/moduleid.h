// Built on Mon Dec 22 19:34:04 2008 by makeCanId.pl
// DO NOT CHANGE MANUALLY

//------------------ //
// Class definitions //
//------------------ //

#define CAN_MODULE_CLASS_NMT 11
#define CAN_MODULE_CLASS_ACT 12
#define CAN_MODULE_CLASS_SNS 13
#define CAN_MODULE_CLASS_DEF 14
#define CAN_MODULE_CLASS_TST 15

//-------------------- //
// Command definitions //
//-------------------- //

#define CAN_MODULE_CMD_GLOBAL_LIST 0
#define CAN_MODULE_CMD_GLOBAL_ASCII 1
#define CAN_MODULE_CMD_PHYSICAL_PWM 64
#define CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS 65
#define CAN_MODULE_CMD_PHYSICAL_HUMIDITY_PERCENT 66
#define CAN_MODULE_CMD_PHYSICAL_VOLTAGE 67
#define CAN_MODULE_CMD_PHYSICAL_PHONENUMBER 68
#define CAN_MODULE_CMD_PHYSICAL_ROTARY_SWITCH 69
#define CAN_MODULE_CMD_SPECIFIC_LCD_CLEAR 128
#define CAN_MODULE_CMD_SPECIFIC_LCD_CURSOR 129
#define CAN_MODULE_CMD_SPECIFIC_LCD_TEXT 130
#define CAN_MODULE_CMD_SPECIFIC_LCD_SIZE 131
#define CAN_MODULE_CMD_SPECIFIC_LCD_BACKLIGHT 132
#define CAN_MODULE_CMD_SPECIFIC_SENSOR_REPORT_INTERVAL 133
#define CAN_MODULE_CMD_SPECIFIC_LCD_TEXTAT 134

//------------------- //
// Module definitions //
//------------------- //

#define CAN_MODULE_TYPE_DEF_DEFAULT 1
#define CAN_MODULE_TYPE_SNS_DS18X20 3
#define CAN_MODULE_TYPE_SNS_FOST02 5
#define CAN_MODULE_TYPE_SNS_BUSVOLTAGE 6
#define CAN_MODULE_TYPE_SNS_SIMPLEDTMF 7
#define CAN_MODULE_TYPE_SNS_ROTARY 8
#define CAN_MODULE_TYPE_ACT_HD44789 4
#define CAN_MODULE_TYPE_ACT_DIMMER230 8

//----------------- //
// Misc definitions //
//----------------- //

#define DIRECTIONFLAG_FROM_OWNER 1
#define DIRECTIONFLAG_TO_OWNER 0
#define NONE_HEARTBEATHEADER 002c0000

