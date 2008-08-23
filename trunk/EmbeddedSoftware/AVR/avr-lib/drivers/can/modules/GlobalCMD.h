#ifndef GLOBAL_CMD_H_
#define GLOBAL_CMD_H_
/*
 *	This file contains commands that are not module specific, "global commands"
 *	How the Data-part of a frame with a sertain unit is formed shall be documented on the project wiki
 * 	Only Id:s with numbers between 0x00 and 0x3f is allowed
 */
//#define CAN_CMD_MODULE_GLOB_???			0
#define CAN_CMD_MODULE_NMT_LIST		0x00UL		// Tell the application to report which modules it has, or if 
#define CAN_CMD_MODULE_GLOB_ASCII	0x01UL
							// direction is from owner -> info from an module
#endif /*GLOBAL_CMD_H_*/
