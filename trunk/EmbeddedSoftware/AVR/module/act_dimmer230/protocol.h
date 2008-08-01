#ifndef ACT_DIMMER230_PROTOCOL
#define ACT_DIMMER230_PROTOCOL
#include <drivers/can/modules/PhysicalQuantityCMD.h>
#include <drivers/can/modules/GlobalCMD.h>
/*
 *      This file contains commands used for only for this module
 *      How the Data-part of a frame with a sertain command is formed shall be documented on the project wiki
 *      Only Id:s with numbers between 0x80 and 0xbf is allowed
 */
#define CAN_CMD_MODULE_DIMMER_DEMO			0x80
#define CAN_CMD_MODULE_DIMMER_START_FADE	0x81
#define CAN_CMD_MODULE_DIMMER_STOP_FADE		0x82
#define CAN_CMD_MODULE_DIMMER_ABS_FADE		0x83
#define CAN_CMD_MODULE_DIMMER_REL_FADE		0x84


#endif // ACT_DIMMER230_PROTOCOL
