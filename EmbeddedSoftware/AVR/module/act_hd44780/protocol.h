#ifndef ACT_HD44780_PROTOCOL
#define ACT_HD44780_PROTOCOL

#include <drivers/can/modules/GlobalCMD.h>
/*
 *      This file contains commands used for only for this module
 *      How the Data-part of a frame with a sertain command is formed shall be documented on the project wiki
 *      Only Id:s with numbers between 128 and 191 is allowed
 */

#define CAN_CMD_MODULE_LCD_CLEAR		128
#define CAN_CMD_MODULE_LCD_CURSOR		129
#define CAN_CMD_MODULE_LCD_PRINT		130
#define CAN_CMD_MODULE_LCD_SIZE		131
#define CAN_CMD_MODULE_LCD_BACKLIGHT	132

#endif // ACT_HD44780_PROTOCOL
