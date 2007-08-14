#ifndef IR_PROTOCOLS_H_
#define IR_PROTOCOLS_H_

#include "ircommon.h"
/* Maybe ircommon.h should be removed and its contents be placed here instead.
 * After all, there's not much 'common' about it anymore. */

/* Protocol data structure */
typedef struct {
	uint8_t protocol;
	uint32_t data;
	uint16_t timeout;
} Ir_Protocol_Data_t;

/* Which protocols to use. These should perhaps be configuration options 
 * (config.inc). */
#define IR_PROTOCOLS_USE_SIRC		1
#define IR_PROTOCOLS_USE_RC5		1
#define IR_PROTOCOLS_USE_SHARP		1
#define IR_PROTOCOLS_USE_NEC		1
#define IR_PROTOCOLS_USE_SAMSUNG	1

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
/* Try to parse all above protocols until a match is found. */
int8_t parseProtocol(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);
/* The Hash protocol always succeeds and creates a one-way signature of the
 * pulse times buffer that is supposed to be unique and constant for a specific
 * IR event. It may be used to send a valid event even if the protocol hasn't
 * been implemented or decoded yet. */
int8_t parseHash(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto);

/* These functions expand a Ir_Protocol_Data_t structure into a raw buffer with
 * pulse lengths. They return IR_OK on success and IR_NOT_CORRECT_DATA if the
 * Ir_Protocol_Data_t structure is for another protocol or contains invalid
 * data. */
int8_t expandSIRC(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);
int8_t expandRC5(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);
int8_t expandSharp(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);
int8_t expandNEC(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);
int8_t expandSamsung(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);
/* Pass the Ir_Protocol_Data_t automatically to the correct function. */
int8_t expandProtocol(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto);

#endif /*IR_PROTOCOLS_H_*/
