#include "protocols.h"

int8_t parseProtocol(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* Try all protocols in order. */
#if (IR_PROTOCOLS_USE_SIRC)
	if (parseSIRC(buf, len, proto)) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_RC5)
	if (parseRC5(buf, len, proto)) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_SHARP)
	if (parseSharp(buf, len, proto)) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_NEC)
	if (parseNEC(buf, len, proto)) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_SAMSUNG)
	if (parseSamsung(buf, len, proto)) return IR_OK;
#endif
	/* No protocol matched. */
	return IR_NOT_CORRECT_DATA;
}

int8_t parseHash(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	//TODO: Transform the buffer in some clever way to a 32 bit word. */
	proto->protocol = IR_PROTO_HASH;
	proto->timeout = 0;
	proto->data = 0;
}

int8_t expandProtocol(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	/* Call the expand function for the specified protocol. */
	switch (proto->protocol) {
	case IR_PROTO_SIRC:
		return expandSIRC(buf, len, proto);
	case IR_PROTO_RC5:
		return expandRC5(buf, len, proto);
	case IR_PROTO_SHARP:
		return expandSharp(buf, len, proto);
	case IR_PROTO_NEC:
		return expandNEC(buf, len, proto);
	case IR_PROTO_SAMSUNG:
		return expandSamsung(buf, len, proto);
	}
	/* Invalid protocol specified. */
	return IR_NOT_CORRECT_DATA;
}

#if (IR_PROTOCOLS_USE_SIRC)
/**
 * Test data on SIRC protocol, 12-bit version
 * http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
int8_t parseSIRC(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 25) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (buf[0] > IR_SIRC_ST_BIT + IR_SIRC_ST_BIT/IR_SIRC_TOL_DIV || buf[0] < IR_SIRC_ST_BIT - IR_SIRC_ST_BIT/IR_SIRC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint16_t rawbits=0;
	
	for (uint8_t i = 1; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_SIRC_LOW + IR_SIRC_LOW/IR_SIRC_TOL_DIV || buf[i] < IR_SIRC_LOW - IR_SIRC_LOW/IR_SIRC_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_SIRC_HIGH_ONE - IR_SIRC_HIGH_ONE/IR_SIRC_TOL_DIV && buf[i] < IR_SIRC_HIGH_ONE + IR_SIRC_HIGH_ONE/IR_SIRC_TOL_DIV) {
				/* write a one */
				rawbits |= 1<<((i-2)>>1);
			} else if (buf[i] > IR_SIRC_HIGH_ZERO - IR_SIRC_HIGH_ZERO/IR_SIRC_TOL_DIV && buf[i] < IR_SIRC_HIGH_ZERO + IR_SIRC_HIGH_ZERO/IR_SIRC_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	proto->protocol = IR_PROTO_SIRC;
	proto->timeout = IR_SIRC_REPETITION;
	proto->data = rawbits; //TODO: Should some other mapping be used?
	
	return IR_OK;
}

int8_t expandSIRC(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}
#endif

#if (IR_PROTOCOLS_USE_RC5)
/**
 * Test data on RC5 protocol 
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
int8_t parseRC5(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	uint8_t halfbitscnt = 1;
	uint16_t rawbits = 0;
	
	for (uint8_t i = 0; i<len; i++) {
		//halfbitscnt&1==1 in the middle of bits
		//i&1==0 positive flank

		if ((halfbitscnt&1)==1 && (i&1)==0) {		/* in the middle of bit AND a positve flank */
			rawbits |= (1<<(13-(halfbitscnt>>1)));
		}
		
		if (buf[i] > IR_RC5_HALF_BIT - IR_RC5_HALF_BIT/IR_RC5_TOL_DIV && buf[i] < IR_RC5_HALF_BIT + IR_RC5_HALF_BIT/IR_RC5_TOL_DIV) {
			halfbitscnt += 1;
		} else if (buf[i] > IR_RC5_BIT - IR_RC5_BIT/IR_RC5_TOL_DIV && buf[i] < IR_RC5_BIT + IR_RC5_BIT/IR_RC5_TOL_DIV) {
			halfbitscnt += 2;
		} else {
			return IR_NOT_CORRECT_DATA;
		}
		
	}

	//TODO: Fix this parser to use the proto struct instead.
	*command = ((uint8_t)rawbits&0x3f);
	*address = ((uint8_t)(rawbits>>6)&0x7f);	

	//support RC5-extended by putting the inversion of startbit 2 as the 7th commandbit
	*command |= (~(*address) & 0x40);
	*address &= 0x1f;		//remove togglebit and both startbits from address
	
	return IR_OK;
}

int8_t expandRC5(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}
#endif

#if (IR_PROTOCOLS_USE_SHARP)
/**
 * Test data on SHARP protocol
 * http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
int8_t parseSharp(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 31) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint16_t rawbits=0;
	
	for (uint8_t i = 1; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_SHARP_LOW_ONE - IR_SHARP_LOW_ONE/IR_SHARP_TOL_DIV && buf[i] < IR_SHARP_LOW_ONE + IR_SHARP_LOW_ONE/IR_SHARP_TOL_DIV) {
				/* write a one */
				rawbits |= 1<<((i-1)>>1);
			} else if (buf[i] > IR_SHARP_LOW_ZERO - IR_SHARP_LOW_ZERO/IR_SHARP_TOL_DIV && buf[i] < IR_SHARP_LOW_ZERO + IR_SHARP_LOW_ZERO/IR_SHARP_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_SHARP_HIGH + IR_SHARP_HIGH/IR_SHARP_TOL_DIV || buf[i] < IR_SHARP_HIGH - IR_SHARP_HIGH/IR_SHARP_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	//TODO: Fix this parser to use the proto struct instead.
	*command = ((uint8_t)(rawbits>>5)&0xff);
	*address = ((uint8_t)rawbits&0x1f);
	
	return IR_OK;
}

int8_t expandSharp(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}
#endif

#if (IR_PROTOCOLS_USE_NEC)
/**
 * Test data on NEC protocol
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
int8_t parseNEC(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 67) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (buf[0] > IR_NEC_ST_BIT + IR_NEC_ST_BIT/IR_NEC_TOL_DIV || buf[0] < IR_NEC_ST_BIT - IR_NEC_ST_BIT/IR_NEC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	/* check pause after startbit */
	if (buf[1] > IR_NEC_ST_PAUSE + IR_NEC_ST_PAUSE/IR_NEC_TOL_DIV || buf[1] < IR_NEC_ST_PAUSE - IR_NEC_ST_PAUSE/IR_NEC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	//TODO: Fix this parser to use the proto struct instead. Do not write to the struct unless IR_OK is returned, use temp variables.
	*command = 0;
	*address = 0;
	
	for (uint8_t i = 3; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_NEC_LOW_ONE - IR_NEC_LOW_ONE/IR_NEC_TOL_DIV && buf[i] < IR_NEC_LOW_ONE + IR_NEC_LOW_ONE/IR_NEC_TOL_DIV) {
				if (i>2 && i<18) {
					/* write a one */
					*address |= 1<<((i-3)>>1);
				}
				if (i>34 && i<50) {
					/* write a one */
					*command |= 1<<((i-35)>>1);
				}
			} else if (buf[i] > IR_NEC_LOW_ZERO - IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV && buf[i] < IR_NEC_LOW_ZERO + IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_NEC_HIGH + IR_NEC_HIGH/IR_NEC_TOL_DIV || buf[i] < IR_NEC_HIGH - IR_NEC_HIGH/IR_NEC_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	return IR_OK;
}

int8_t expandNEC(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}
#endif

#if (IR_PROTOCOLS_USE_SAMSUNG)
/**
 * Test data on Samsung protocol
 * Very much like NEC, different start bit/pause lengths etc.
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
int8_t parseSamsung(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 67) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (buf[0] > IR_SAMS_ST_BIT + IR_SAMS_ST_BIT/IR_SAMS_TOL_DIV || buf[0] < IR_SAMS_ST_BIT - IR_SAMS_ST_BIT/IR_SAMS_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	/* check pause after startbit */
	if (buf[1] > IR_SAMS_ST_PAUSE + IR_SAMS_ST_PAUSE/IR_SAMS_TOL_DIV || buf[1] < IR_SAMS_ST_PAUSE - IR_SAMS_ST_PAUSE/IR_SAMS_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	//TODO: Fix this parser to use the proto struct instead. Do not write to the struct unless IR_OK is returned, use temp variables.
	*command = 0;
	*address = 0;
	
	for (uint8_t i = 3; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_SAMS_LOW_ONE - IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV && buf[i] < IR_SAMS_LOW_ONE + IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV) {
				if (i>2 && i<18) {
					/* write a one */
					*address |= 1<<((i-3)>>1);
				}
				if (i>34 && i<50) {
					/* write a one */
					*command |= 1<<((i-35)>>1);
				}
			} else if (buf[i] > IR_SAMS_LOW_ZERO - IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV && buf[i] < IR_SAMS_LOW_ZERO + IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_SAMS_HIGH + IR_SAMS_HIGH/IR_SAMS_TOL_DIV || buf[i] < IR_SAMS_HIGH - IR_SAMS_HIGH/IR_SAMS_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	return IR_OK;
}

int8_t expandSamsung(uint16_t *buf, uint8_t len, const Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}
#endif

