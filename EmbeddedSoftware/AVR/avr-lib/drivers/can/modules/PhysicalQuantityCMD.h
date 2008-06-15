#ifndef PHYS_QUAN_CMD_H_
#define PHYS_QUAN_CMD_H_
/*
 *	This file contains commands used for standard units used by the modules build system
 *	How the Data-part of a frame with a sertain unit is formed shall be documented on the project wiki
 * 	Only Id:s with numbers between 0x40 and 0x7f is allowed
 */
#define CAN_CMD_MODULE_PHYS_PWM			0x40UL
#define CAN_CMD_MODULE_PHYS_TEMPERATURE_CELSIUS	0x41UL
#define CAN_CMD_MODULE_PHYS_HUMIDITY_PERCENT	0x42UL
#define CAN_CMD_MODULE_PHYS_VOLTAGE			0x43UL
#define CAN_CMD_MODULE_PHYS_PHONENUMBER		0x44UL

#endif /*PHYS_QUAN_CMD_H_*/
