// Built on Sun Jul 12 17:52:17 2009 by makeCanId.pl
// DO NOT CHANGE MANUALLY

#ifndef MODULEID_H_
#define MODULEID_H_

//------------------ //
// Class definitions //
//------------------ //

#define CAN_MODULE_CLASS_NMT 0
#define CAN_MODULE_CLASS_MNMT 11
#define CAN_MODULE_CLASS_ACT 12
#define CAN_MODULE_CLASS_SNS 13
#define CAN_MODULE_CLASS_DEF 14
#define CAN_MODULE_CLASS_TST 15
#define CAN_MODULE_CLASS_CHN 10

//-------------------- //
// Command definitions //
//-------------------- //

#define CAN_MODULE_CMD_GLOBAL_LIST 0
#define CAN_MODULE_CMD_GLOBAL_DEBUG 2
#define CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL 3
#define CAN_MODULE_CMD_PHYSICAL_PWM 64
#define CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS 65
#define CAN_MODULE_CMD_PHYSICAL_HUMIDITY_PERCENT 66
#define CAN_MODULE_CMD_PHYSICAL_VOLTAGE 67
#define CAN_MODULE_CMD_PHYSICAL_PHONENUMBER 68
#define CAN_MODULE_CMD_PHYSICAL_ROTARY_SWITCH 69
#define CAN_MODULE_CMD_PHYSICAL_BUTTON 70
#define CAN_MODULE_CMD_PHYSICAL_IR 71
#define CAN_MODULE_CMD_PHYSICAL_CURRENT 72
#define CAN_MODULE_CMD_PHYSICAL_ELECTRICPOWER 73
#define CAN_MODULE_CMD_POWER_SETENERGY 128
#define CAN_MODULE_CMD_IRRECEIVE_IRRAW 128
#define CAN_MODULE_CMD_NMEA_POSITION 128
#define CAN_MODULE_CMD_DEBUG_PRINTF 128
#define CAN_MODULE_CMD_HD44789_LCD_CLEAR 128
#define CAN_MODULE_CMD_HD44789_LCD_CURSOR 129
#define CAN_MODULE_CMD_HD44789_LCD_TEXT 130
#define CAN_MODULE_CMD_HD44789_LCD_SIZE 131
#define CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT 132
#define CAN_MODULE_CMD_HD44789_LCD_TEXTAT 133
#define CAN_MODULE_CMD_PID_DEBUG 128
#define CAN_MODULE_CMD_PID_CONFIG 129
#define CAN_MODULE_CMD_SLOWPWM_CONFIG 128
#define CAN_MODULE_CMD_KS0108_LCD_CLEAR 128
#define CAN_MODULE_CMD_KS0108_LCD_CURSOR 129
#define CAN_MODULE_CMD_KS0108_LCD_TEXT 130
#define CAN_MODULE_CMD_KS0108_LCD_INVERT 131
#define CAN_MODULE_CMD_KS0108_LCD_BACKLIGHT 132
#define CAN_MODULE_CMD_KS0108_LCD_TEXTAT 133
#define CAN_MODULE_CMD_KS0108_LCD_DRAWRECT 134
#define CAN_MODULE_CMD_KS0108_LCD_DRAWLINE 135
#define CAN_MODULE_CMD_KS0108_LCD_DRAWCIRCLE 136
#define CAN_MODULE_CMD_KS0108_LCD_INVERTRECT 137
#define CAN_MODULE_CMD_KS0108_LCD_NEW_IMAGE 138
#define CAN_MODULE_CMD_KS0108_LCD_ACK 139
#define CAN_MODULE_CMD_KS0108_LCD_ERROR 140
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_7 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_6 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_5 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_4 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_3 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_2 141
#define CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA_1 141
#define CAN_MODULE_CMD_KS0108_LCD_DATA_ID_ERROR 142
#define CAN_MODULE_CMD_KS0108_LCD_DATA_DONE 143
#define CAN_MODULE_CMD_DIMMER230_DEMO 128
#define CAN_MODULE_CMD_DIMMER230_START_FADE 129
#define CAN_MODULE_CMD_DIMMER230_STOP_FADE 130
#define CAN_MODULE_CMD_DIMMER230_ABS_FADE 131
#define CAN_MODULE_CMD_DIMMER230_REL_FADE 132
#define CAN_MODULE_CMD_DIMMER230_NETINFO 133
#define CAN_MODULE_CMD_SOFTPWM_CONFIG 128

//------------------------------- //
// Command enum value definitions //
//------------------------------- //

#define CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE 0
#define CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE 1
#define CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED 1
#define CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED 1
#define CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC5 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC6 1
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RCMM 2
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SIRC 3
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SHARP 4
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEC 5
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SAMSUNG 6
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_MARANTZ 7
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_UNKNOWN 255
#define CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_LOW 0
#define CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_HIGH 1
#define CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_TRISTATE 2
#define CAN_MODULE_ENUM_SLOWPWM_CONFIG_STARTUPSTATE_ON 0
#define CAN_MODULE_ENUM_SLOWPWM_CONFIG_STARTUPSTATE_OFF 1
#define CAN_MODULE_ENUM_KS0108_LCD_CLEAR_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_CLEAR_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_CURSOR_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_CURSOR_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_CURSOR_TRANSPARENT_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_CURSOR_TRANSPARENT_TRANSPARENT 1
#define CAN_MODULE_ENUM_KS0108_LCD_INVERT_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_INVERT_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_TEXTAT_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_TEXTAT_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_TEXTAT_TRANSPARENT_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_TEXTAT_TRANSPARENT_TRANSPARENT 1
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWRECT_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWRECT_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWRECT_FILL_NOFILL 0
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWRECT_FILL_FILL 1
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWLINE_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWLINE_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWCIRCLE_INVERTED_STANDARD 0
#define CAN_MODULE_ENUM_KS0108_LCD_DRAWCIRCLE_INVERTED_INVERTED 1
#define CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_DISCONNECTED 0
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_CONNECTED 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_ERROR_UNKNOWN 0
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_50HZ 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_60HZ 2

//------------------- //
// Module definitions //
//------------------- //

#define CAN_MODULE_TYPE_TST_DEBUG 0
#define CAN_MODULE_TYPE_DEF_DEFAULT 1
#define CAN_MODULE_TYPE_SNS_DS18X20 3
#define CAN_MODULE_TYPE_SNS_FOST02 5
#define CAN_MODULE_TYPE_SNS_BUSVOLTAGE 6
#define CAN_MODULE_TYPE_SNS_SIMPLEDTMF 7
#define CAN_MODULE_TYPE_SNS_ROTARY 8
#define CAN_MODULE_TYPE_ACT_HD44789 9
#define CAN_MODULE_TYPE_ACT_DIMMER230 10
#define CAN_MODULE_TYPE_ACT_KS0108 11
#define CAN_MODULE_TYPE_ACT_RGBDRIVER 12
#define CAN_MODULE_TYPE_ACT_LEDPWM 13
#define CAN_MODULE_TYPE_SNS_IRRECEIVE 14
#define CAN_MODULE_TYPE_SNS_NMEA 15
#define CAN_MODULE_TYPE_SNS_IRTRANSMIT 16
#define CAN_MODULE_TYPE_SNS_TC1047A 17
#define CAN_MODULE_TYPE_ACT_PID 18
#define CAN_MODULE_TYPE_ACT_SLOWPWM 19
#define CAN_MODULE_TYPE_SNS_POWER 20
#define CAN_MODULE_TYPE_ACT_SOFTPWM 21
#define CAN_MODULE_TYPE_CHN_CHNMASTER 30

//----------------- //
// Misc definitions //
//----------------- //

#define DIRECTIONFLAG_FROM_OWNER 1
#define DIRECTIONFLAG_TO_OWNER 0

#endif /*MODULEID_H_*/
