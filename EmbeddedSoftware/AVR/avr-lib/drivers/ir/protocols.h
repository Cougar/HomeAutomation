/** 
 * @defgroup infrared IR Protocols Driver Library
 * @code #include <drivers/ir/protocols.h> @endcode
 * 
 * @brief  
 *
 *
 * @author	Andreas Fritiofson, Anders Runeson
 * @date	2007
 */

/**@{*/


#ifndef IR_PROTOCOLS_H_
#define IR_PROTOCOLS_H_
#include <stdio.h>
#include <config.h>

/* Protocol data structure */
typedef struct {
	uint8_t protocol;
	uint64_t data;
	uint16_t timeout;		//Set to 0 to indicate burst protocol (only one pulse train is sent)
	uint8_t repeats;
	uint16_t modfreq;
	uint8_t framecnt;
} Ir_Protocol_Data_t;

/* Which protocols to use. These should perhaps be configuration options 
 * (config.inc). Only applies to receiver right now */
#ifndef IR_PROTOCOLS_USE_SIRC
# define IR_PROTOCOLS_USE_SIRC		1
#endif
#ifndef IR_PROTOCOLS_USE_RC5
# define IR_PROTOCOLS_USE_RC5		1
#endif
#ifndef IR_PROTOCOLS_USE_SHARP
# define IR_PROTOCOLS_USE_SHARP		1
#endif
#ifndef IR_PROTOCOLS_USE_NEC
# define IR_PROTOCOLS_USE_NEC		1
#endif
#ifndef IR_PROTOCOLS_USE_SAMSUNG
# define IR_PROTOCOLS_USE_SAMSUNG	1
#endif
#ifndef IR_PROTOCOLS_USE_MARANTZ
# define IR_PROTOCOLS_USE_MARANTZ	1
#endif
#ifndef IR_PROTOCOLS_USE_PANASONIC
# define IR_PROTOCOLS_USE_PANASONIC	1
#endif
#ifndef IR_PROTOCOLS_USE_SKY
# define IR_PROTOCOLS_USE_SKY		1
#endif
#ifndef IR_PROTOCOLS_USE_NEXA2
# define IR_PROTOCOLS_USE_NEXA2		1
#endif
#ifndef IR_PROTOCOLS_USE_NEXA1
# define IR_PROTOCOLS_USE_NEXA1		1
#endif

/* All these functions take a buffer with pulse times and tries to parse it
 * to a Ir_Protocol_Data_t structure. They return IR_OK on success
 * and IR_NOT_CORRECT_DATA if there was no match. */
#if (IR_PROTOCOLS_USE_SIRC)
int8_t parseSIRC(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_RC5)
int8_t parseRC5(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_SHARP)
int8_t parseSharp(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_NEC)
int8_t parseNEC(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_SAMSUNG)
int8_t parseSamsung(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_MARANTZ)
int8_t parseMarantz(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_PANASONIC)
int8_t parsePanasonic(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_SKY)
int8_t parseSky(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
#endif

/* RF protocols needs index parameter */
#if (IR_PROTOCOLS_USE_NEXA2)
int8_t parseNexa2(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto);
#endif
#if (IR_PROTOCOLS_USE_NEXA1)
int8_t parseNexa1(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto);
#endif

/* Try to parse all above protocols until a match is found. */
int8_t parseProtocol(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto);
/* The Hash protocol always succeeds and creates a one-way signature of the
 * pulse times buffer that is supposed to be unique and constant for a specific
 * IR event. It may be used to send a valid event even if the protocol hasn't
 * been implemented or decoded yet. */
int8_t parseHash(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);

/* These functions expand a Ir_Protocol_Data_t structure into a raw buffer with
 * pulse lengths. They return IR_OK on success and IR_NOT_CORRECT_DATA if the
 * Ir_Protocol_Data_t structure is for another protocol or contains invalid
 * data. */
int8_t expandSIRC(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandRC5(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandSharp(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandNEC(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandSamsung(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandMarantz(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandPanasonic(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandSky(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandNexa2(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
int8_t expandNexa1(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);
/* Pass the Ir_Protocol_Data_t automatically to the correct function. */
int8_t expandProtocol(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto);


#define IR_OK 				1
#define IR_NO_PROTOCOL		2
#define IR_TIME_OVFL		3
#define IR_NO_DATA 			4
#define IR_NOT_CORRECT_DATA 5
#define IR_TO_MUCH_DATA		6
#define IR_NOT_FINISHED		7

#define IR_BUTTON_DOWN		0x0
#define IR_BUTTON_UP		0xf

// defined for the module, uses same definitions for pressed and released as other buttons
#define IR_BUTTON_RELEASED	0x0
#define IR_BUTTON_PRESSED	0x1


#define CYCLES_PER_US       (F_CPU/1000000UL)
#define TIMER_PRESC			(8)

#ifndef IR_MIN_PULSE_WIDTH
#define IR_MIN_PULSE_WIDTH	(0)									//us, used in continuous mode
#endif
#ifndef IR_MIN_STARTPULSE_WIDTH
#define IR_MIN_STARTPULSE_WIDTH	(0UL)							//us, not used when set to 0
#endif
#ifndef IR_MAX_PULSE_WIDTH
#define IR_MAX_PULSE_WIDTH	(12000UL)							//us
#endif

/* RC5 Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_RC5		(0)
#define IR_RC5_HALF_BIT		(889*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_RC5_BIT			(889*2*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_RC5_TIMEOUT		(117-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_RC5_REPS			(1)									//		(minimum number of times to repeat code)
#define IR_RC5_F_MOD		(36)								//kHz	(modulation frequency)
#define IR_RC5_TOL_DIV		(4)

/* RC6 Implementation
 * Receiver: 
 * Transmitter: 
 */
#define IR_PROTO_RC6		(1)
#define IR_RC6_ST_BIT		(1*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_RC6_TIMEOUT		(50-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_RC6_REPS			(1)									//		(minimum number of times to repeat code)
#define IR_RC6_F_MOD		(36)								//kHz	(modulation frequency)
#define IR_RC6_TOL_DIV		(4)

/* RCMM Implementation
 * Receiver: 
 * Transmitter: 
 */
#define IR_PROTO_RCMM		(2)
#define IR_RCMM_ST_BIT		(256*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_RCMM_TIMEOUT 	(170-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_RCMM_REPS		(1)									//		(minimum number of times to repeat code)
#define IR_RCMM_F_MOD		(36)								//kHz	(modulation frequency)
#define IR_RCMM_TOL_DIV		(4)

/* SIRC Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_SIRC		(3)
#define IR_SIRC_ST_BIT		(2400*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SIRC_LOW			(600*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SIRC_HIGH_ONE	(1200*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SIRC_HIGH_ZERO	(600*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SIRC_TIMEOUT		(52-IR_MAX_PULSE_WIDTH/1000)									//ms	(time between ir frames)
#define IR_SIRC_REPS		(3)									//		(minimum number of times to repeat code)
#define IR_SIRC_F_MOD		(40)								//kHz	(modulation frequency)
#define IR_SIRC_TOL_DIV		(4)

/* Sharp Implementation
 * Receiver: DONE
 * Transmitter: 
 */
#define IR_PROTO_SHARP		(4)
#define IR_SHARP_HIGH		(280*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SHARP_LOW_ONE	(1850*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SHARP_LOW_ZERO	(780*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SHARP_TIMEOUT	(67-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_SHARP_REPS		(2)									//		(minimum number of times to repeat code)
#define IR_SHARP_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_SHARP_TOL_DIV	(4)

/* NEC Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_NEC		(5)
#define IR_NEC_ST_BIT		(9000*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEC_ST_PAUSE		(4500*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEC_LOW_ONE		(1690*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEC_LOW_ZERO		(560*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_NEC_HIGH			(560*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_NEC_TIMEOUT		(122-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_NEC_ST_TIMEOUT	(65)								//ms	(time between first ir frames and second)
#define IR_NEC_REPS			(1)									//		(minimum number of times to repeat code)
#define IR_NEC_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_NEC_TOL_DIV		(4)

/* Samsung Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_SAMS		(6)
#define IR_SAMS_ST_BIT		(4500*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SAMS_ST_PAUSE	(4500*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SAMS_LOW_ONE		(1720*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SAMS_LOW_ZERO	(650*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SAMS_HIGH		(500*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SAMS_TIMEOUT		(62-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_SAMS_REPS		(1)									//		(minimum number of times to repeat code)
#define IR_SAMS_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_SAMS_TOL_DIV		(4)

/* Marantz Implementation
 * Receiver: DONE (has not been tested with odd adressbits)
 * Transmitter: DONE
 */
#define IR_PROTO_MARANTZ		(7)
#define IR_MARANTZ_HALF_BIT		(889*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_MARANTZ_BIT			(889*2*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_MARANTZ_TIMEOUT		(117-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_MARANTZ_REPS			(1)									//		(minimum number of times to repeat code)
#define IR_MARANTZ_F_MOD		(36)								//kHz	(modulation frequency)
#define IR_MARANTZ_TOL_DIV		(4)

/* Panasonic Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_PANASONIC	(8)
#define IR_PANA_ST_BIT		(3570*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_PANA_ST_PAUSE	(1630*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_PANA_LOW_ONE		(1240*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_PANA_LOW_ZERO	(400*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_PANA_HIGH		(495*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_PANA_TIMEOUT		(92-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_PANA_REPS		(1)									//		(minimum number of times to repeat code)
#define IR_PANA_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_PANA_TOL_DIV		(4)

/* Sky Implementation
 * Receiver: DONE
 * Transmitter: 
 */
#define IR_PROTO_SKY		(9)
#define IR_SKY_ST_BIT		(2800*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_SKY_SHORT		(460*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SKY_LONG			(920*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_SKY_TIMEOUT		(162-IR_MAX_PULSE_WIDTH/1000)		//ms	(time between ir frames)
#define IR_SKY_REPS			(1)									//		(minimum number of times to repeat code)
#define IR_SKY_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_SKY_TOL_DIV		(4)


/* Nexa2 Implementation
 * Receiver: DONE
 * Transmitter: DONE
 */
#define IR_PROTO_NEXA2		(10)
#define IR_NEXA2_START1 	(10000*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEXA2_START2 	(2500*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEXA2_HIGH 		(320*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_NEXA2_LOW_ONE	(210*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_NEXA2_LOW_ZERO	(1200*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEXA2_TIMEOUT	(200)								//ms	(time between ir frames)
#define IR_NEXA2_REPS		(5)									//		(minimum number of times to repeat code)
#define IR_NEXA2_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_NEXA2_TOL_DIV	(2)


/* Nexa1 Implementation
 * Receiver: DONE
 * Transmitter: 
 */
#define IR_PROTO_NEXA1		(11)
#define IR_NEXA1_START	 	(10000*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEXA1_SHORT 		(370*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_NEXA1_LONG		(1050*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_NEXA1_TIMEOUT	(200)								//ms	(time between ir frames)
#define IR_NEXA1_REPS		(4)									//		(minimum number of times to repeat code)
#define IR_NEXA1_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_NEXA1_TOL_DIV	(2)


/* Viking Temperature sensor Implementation
 * Receiver: 
 */
#define IR_PROTO_VIKING		(12)
#define IR_VIKING_LOW 		(940*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_VIKING_HIGH_ONE	(1483*CYCLES_PER_US/TIMER_PRESC)		//us
#define IR_VIKING_HIGH_ZERO	(550*CYCLES_PER_US/TIMER_PRESC)	//us
#define IR_VIKING_TIMEOUT	(0)									//ms BURST!	(time between ir frames)
#define IR_VIKING_REPS		(1)									//		(minimum number of times to repeat code)
#define IR_VIKING_F_MOD		(38)								//kHz	(modulation frequency)
#define IR_VIKING_TOL_DIV	(4)


#define IR_PROTO_HASH		0xfe
#define IR_PROTO_UNKNOWN	0xff								//



/**@}*/
#endif /*IR_PROTOCOLS_H_*/
