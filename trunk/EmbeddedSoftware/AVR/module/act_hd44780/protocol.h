#ifndef ACT_HD44780_PROTOCOL
#define ACT_HD44780_PROTOCOL

#include <drivers/can/modules/GlobalCMD.h>
/*
 *      This file contains commands used for only for this module
 *      How the Data-part of a frame with a sertain command is formed shall be documented on the project wiki
 *      Only Id:s with numbers between 0x80 and 0xbf is allowed
 */

#define CAN_CMD_MODULE_LCD_CLEAR		0x80
#define CAN_CMD_MODULE_LCD_CURSOR		0x81
#define CAN_CMD_MODULE_LCD_PRINT		0x82
#define CAN_CMD_MODULE_LCD_SIZE		0x83
#define CAN_CMD_MODULE_LCD_BACKLIGHT	0x84
#define CAN_CMD_MODULE_LCD_PRINTAT	0x86
#endif // ACT_HD44780_PROTOCOL
