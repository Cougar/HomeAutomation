// Built on Sun Aug 19 13:03:45 2012 by makeCanId.pl
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
#define CAN_MODULE_CMD_GLOBAL_GPSTIME 10
#define CAN_MODULE_CMD_GLOBAL_GPSDATE 11
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
#define CAN_MODULE_CMD_PHYSICAL_SETPIN 74
#define CAN_MODULE_CMD_PHYSICAL_PINSTATUS 75
#define CAN_MODULE_CMD_PHYSICAL_COUNTER 76
#define CAN_MODULE_CMD_PHYSICAL_FLOW 77
#define CAN_MODULE_CMD_POWER_SETENERGY 128
#define CAN_MODULE_CMD_POWER_AVGPOWER 129
#define CAN_MODULE_CMD_IRRECEIVE_IRRAW 128
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRRAW 128
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRCONFIG 129
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOSTART 130
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOTIMING 131
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOCONTINUE 132
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOSTOP 133
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTORESPONSE 134
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1 135
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA2 136
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA3 137
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA4 138
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA5 139
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA6 140
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA7 141
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA8 142
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA9 143
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA10 144
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA11 145
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA12 146
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA13 147
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA14 148
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA15 149
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA16 150
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND1 151
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND2 152
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND3 153
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND4 154
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND5 155
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND6 156
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND7 157
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND8 158
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND9 159
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND10 160
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND11 161
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND12 162
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND13 163
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND14 164
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND15 165
#define CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND16 166
#define CAN_MODULE_CMD_NMEA_POSITION 128
#define CAN_MODULE_CMD_NMEA_HEADING 129
#define CAN_MODULE_CMD_DEBUG_PRINTF 128
#define CAN_MODULE_CMD_HD44789_LCD_CLEAR 128
#define CAN_MODULE_CMD_HD44789_LCD_CURSOR 129
#define CAN_MODULE_CMD_HD44789_LCD_TEXT 130
#define CAN_MODULE_CMD_HD44789_LCD_SIZE 131
#define CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT 132
#define CAN_MODULE_CMD_HD44789_LCD_TEXTAT 133
#define CAN_MODULE_CMD_RGBDRIVER_DEBUG 128
#define CAN_MODULE_CMD_SERIAL_SERIALDATA 128
#define CAN_MODULE_CMD_SERIAL_SERIALCONFIG 129
#define CAN_MODULE_CMD_SERIAL_SERIALFILTER 130
#define CAN_MODULE_CMD_INPUTANALOG_ANALOGCONFIG 128
#define CAN_MODULE_CMD_PID_DEBUG 128
#define CAN_MODULE_CMD_PID_CONFIG_SENSOR 129
#define CAN_MODULE_CMD_PID_CONFIG_ACTUATOR 130
#define CAN_MODULE_CMD_PID_CONFIG_PARAMETER 131
#define CAN_MODULE_CMD_PID_PID_STATUS 132
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
#define CAN_MODULE_CMD_PIR_MOTION 128
#define CAN_MODULE_CMD_DIMMER230_DEMO 128
#define CAN_MODULE_CMD_DIMMER230_START_FADE 129
#define CAN_MODULE_CMD_DIMMER230_STOP_FADE 130
#define CAN_MODULE_CMD_DIMMER230_ABS_FADE 131
#define CAN_MODULE_CMD_DIMMER230_REL_FADE 132
#define CAN_MODULE_CMD_DIMMER230_NETINFO 133
#define CAN_MODULE_CMD_HWPWM_DEMO 128
#define CAN_MODULE_CMD_HWPWM_START_FADE 129
#define CAN_MODULE_CMD_HWPWM_STOP_FADE 130
#define CAN_MODULE_CMD_HWPWM_ABS_FADE 131
#define CAN_MODULE_CMD_HWPWM_REL_FADE 132
#define CAN_MODULE_CMD_PCAPWM_DEMO 128
#define CAN_MODULE_CMD_PCAPWM_START_FADE 129
#define CAN_MODULE_CMD_PCAPWM_STOP_FADE 130
#define CAN_MODULE_CMD_PCAPWM_ABS_FADE 131
#define CAN_MODULE_CMD_PCAPWM_REL_FADE 132
#define CAN_MODULE_CMD_RGBW_DEMO 128
#define CAN_MODULE_CMD_RGBW_START_FADE 129
#define CAN_MODULE_CMD_RGBW_STOP_FADE 130
#define CAN_MODULE_CMD_RGBW_ABS_FADE 131
#define CAN_MODULE_CMD_RGBW_REL_FADE 132
#define CAN_MODULE_CMD_IDENTIFICATION_ID 128
#define CAN_MODULE_CMD_SOFTPWM_CONFIG 128
#define CAN_MODULE_CMD_TOUCH_GESTURE 128
#define CAN_MODULE_CMD_TOUCH_RAW 129
#define CAN_MODULE_CMD_LINEARACT_POSITION 128
#define CAN_MODULE_CMD_LINEARACT_LIMITS 129
#define CAN_MODULE_CMD_LINEARACT_CALIBRATION 130
#define CAN_MODULE_CMD_COUNTER_SETCOUNTER 128
#define CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE1 128
#define CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE2 129
#define CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE3 130
#define CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE4 131
#define CAN_MODULE_CMD_WATER_SETVOLUME 128

//------------------------------- //
// Command enum value definitions //
//------------------------------- //

#define CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE 0
#define CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE 1
#define CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED 1
#define CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED 1
#define CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_BURST 2
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC5 0
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC6 1
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RCMM 2
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SIRC 3
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SHARP 4
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEC 5
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SAMSUNG 6
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_MARANTZ 7
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_PANASONIC 8
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SKY 9
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA2 10
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA 11
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_VIKING 12
#define CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_UNKNOWN 255
#define CAN_MODULE_ENUM_PHYSICAL_SETPIN_STATUS_HIGH 1
#define CAN_MODULE_ENUM_PHYSICAL_SETPIN_STATUS_LOW 0
#define CAN_MODULE_ENUM_PHYSICAL_PINSTATUS_STATUS_HIGH 1
#define CAN_MODULE_ENUM_PHYSICAL_PINSTATUS_STATUS_LOW 0
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO 0
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_TRANSMIT 1
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_RECEIVE 2
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_OK 0
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_STOPPED 1
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_WAITING 2
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_TRANSMITTING 4
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ALREADYSTOPPED 241
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_BUSY_ALREADYSENDINGPRONTO 242
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_BUSY_CHANNELBUSY 243
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_BUSY_OTHERFREQUENCYLOCKED 244
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR_WRONGPRONTOCHANNEL 245
#define CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR 255
#define CAN_MODULE_ENUM_HD44789_LCD_BACKLIGHT_AUTOLIGHT_OFF 0
#define CAN_MODULE_ENUM_HD44789_LCD_BACKLIGHT_AUTOLIGHT_ON 1
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS232 0
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485FULLDUPLEX 1
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEX 2
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEXWITHTERMINATION 3
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK 255
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PARITY_PARITYNONE 0
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PARITY_PARITYEVEN 1
#define CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PARITY_PARITYODD 2
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE 0
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_DIGITALINPUT 1
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE 0
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_ENABLE 1
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE 0
#define CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_ENABLE 1
#define CAN_MODULE_ENUM_PID_CONFIG_PARAMETER_TIMEUNIT_S 0
#define CAN_MODULE_ENUM_PID_CONFIG_PARAMETER_TIMEUNIT_MS 1
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
#define CAN_MODULE_ENUM_KS0108_LCD_BACKLIGHT_AUTOLIGHT_OFF 0
#define CAN_MODULE_ENUM_KS0108_LCD_BACKLIGHT_AUTOLIGHT_ON 1
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
#define CAN_MODULE_ENUM_PIR_MOTION_MOTIONDETECT_NOMOTION 0
#define CAN_MODULE_ENUM_PIR_MOTION_MOTIONDETECT_MOTIONDETECTED 1
#define CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_DISCONNECTED 0
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_CONNECTED 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_ERROR_UNKNOWN 0
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_50HZ 1
#define CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_60HZ 2
#define CAN_MODULE_ENUM_HWPWM_START_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_HWPWM_START_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_HWPWM_REL_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_HWPWM_REL_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_PCAPWM_START_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_PCAPWM_START_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_PCAPWM_REL_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_PCAPWM_REL_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_RGBW_START_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_RGBW_START_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_RGBW_REL_FADE_DIRECTION_DECREASE 0
#define CAN_MODULE_ENUM_RGBW_REL_FADE_DIRECTION_INCREASE 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F3_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F3_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F4_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F4_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F5_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F5_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F6_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F6_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F8_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F8_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F9_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F9_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F2_M 0
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F2_P 1
#define CAN_MODULE_ENUM_TOUCH_GESTURE_F2_U 2
#define CAN_MODULE_ENUM_TOUCH_RAW_STATUS_PRESSED 1
#define CAN_MODULE_ENUM_TOUCH_RAW_STATUS_RELEASED 0

//------------------- //
// Module definitions //
//------------------- //

#define CAN_MODULE_TYPE_TST_DEBUG 0
#define CAN_MODULE_TYPE_DEF_DEFAULT 1
#define CAN_MODULE_TYPE_SNS_DS18X20 3
#define CAN_MODULE_TYPE_SNS_HEATPOWER 4
#define CAN_MODULE_TYPE_SNS_FOST02 5
#define CAN_MODULE_TYPE_SNS_BUSVOLTAGE 6
#define CAN_MODULE_TYPE_SNS_SIMPLEDTMF 7
#define CAN_MODULE_TYPE_SNS_ROTARY 8
#define CAN_MODULE_TYPE_ACT_HD44789 9
#define CAN_MODULE_TYPE_ACT_DIMMER230 10
#define CAN_MODULE_TYPE_ACT_KS0108 11
#define CAN_MODULE_TYPE_ACT_RGBDRIVER 12
#define CAN_MODULE_TYPE_ACT_HWPWM 13
#define CAN_MODULE_TYPE_SNS_IRRECEIVE 14
#define CAN_MODULE_TYPE_SNS_NMEA 15
#define CAN_MODULE_TYPE_SNS_IRTRANSMIT 16
#define CAN_MODULE_TYPE_SNS_TC1047A 17
#define CAN_MODULE_TYPE_ACT_PID 18
#define CAN_MODULE_TYPE_ACT_SLOWPWM 19
#define CAN_MODULE_TYPE_SNS_POWER 20
#define CAN_MODULE_TYPE_ACT_SOFTPWM 21
#define CAN_MODULE_TYPE_SNS_TOUCH 22
#define CAN_MODULE_TYPE_SNS_ROTARYHEX 23
#define CAN_MODULE_TYPE_ACT_OUTPUT 24
#define CAN_MODULE_TYPE_SNS_INPUT 25
#define CAN_MODULE_TYPE_ACT_RGBW 26
#define CAN_MODULE_TYPE_SNS_IRTRANSCEIVE 27
#define CAN_MODULE_TYPE_SNS_IDENTIFICATION 28
#define CAN_MODULE_TYPE_ACT_PCAPWM 29
#define CAN_MODULE_TYPE_CHN_CHNMASTER 30
#define CAN_MODULE_TYPE_CHN_CHNCONTROLLER 31
#define CAN_MODULE_TYPE_SNS_RFID 32
#define CAN_MODULE_TYPE_SNS_WATER 33
#define CAN_MODULE_TYPE_SNS_PIR 45
#define CAN_MODULE_TYPE_SNS_TCN75A 46
#define CAN_MODULE_TYPE_SNS_VOLTAGECURRENT 47
#define CAN_MODULE_TYPE_SNS_SERIAL 48
#define CAN_MODULE_TYPE_SNS_PROTECTEDOUTPUT 49
#define CAN_MODULE_TYPE_SNS_INPUTANALOG 50
#define CAN_MODULE_TYPE_SNS_LM335 51
#define CAN_MODULE_TYPE_SNS_SH11 52
#define CAN_MODULE_TYPE_SNS_NTC 55
#define CAN_MODULE_TYPE_ACT_SIGMADELTA_DIMMER 56
#define CAN_MODULE_TYPE_SNS_COUNTER 57
#define CAN_MODULE_TYPE_ACT_MULTISWITCH 58
#define CAN_MODULE_TYPE_ACT_LINEARACT 59
#define CAN_MODULE_TYPE_ACT_DOTMATRIX 60
#define CAN_MODULE_TYPE_SNS_DHT11 61
#define CAN_MODULE_TYPE_SNS_RFTRANSCEIVE 62
#define CAN_MODULE_TYPE_SNS_QTOUCH 63

//----------------- //
// Misc definitions //
//----------------- //

#define DIRECTIONFLAG_FROM_OWNER 1
#define DIRECTIONFLAG_TO_OWNER 0

#endif /*MODULEID_H_*/
