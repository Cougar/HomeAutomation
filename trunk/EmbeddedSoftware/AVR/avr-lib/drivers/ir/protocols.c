/**
 * IR receiver and transmitter protocols.
 * 
 * @date	2006-12-10
 * 
 * @author	Anders Runeson, Andreas Fritiofson
 *   
 */

#include <bios.h> //FIXME: Ta bort mig, jag finns bara f�r debug av noddans RC6

#include "protocols.h"

int8_t parseProtocol(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	proto->protocol=IR_PROTO_UNKNOWN;
	proto->data=0;
	proto->timeout=100;
	/* Try all protocols in order. */
#if (IR_PROTOCOLS_USE_SIRC)
	if (parseSIRC(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_RC5)
	if (parseRC5(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_SHARP)
	if (parseSharp(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_NEC)
	if (parseNEC(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_SAMSUNG)
	if (parseSamsung(buf, len, proto)==IR_OK) return IR_OK;
#endif
	/* No protocol matched. */
	proto->protocol = IR_PROTO_UNKNOWN;
	return IR_NOT_CORRECT_DATA;
}

int8_t parseHash(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	//TODO: Transform the buffer in some clever way to a 32 bit word. */
	proto->protocol = IR_PROTO_HASH;
	proto->timeout = 200;
	proto->data = 0;
	
	return 0;
}

int8_t expandProtocol(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
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
	case IR_PROTO_SAMS:
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
 * @param buf
 * 		Pointer to buffer to where to data to parse is stored
 * @param len
 * 		Length of the data
 * @param proto
 * 		Pointer to protocol information
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
	proto->timeout = IR_SIRC_TIMEOUT;
	proto->data = rawbits; 
	
	return IR_OK;
}
#endif

/**
 * Expand data from SIRC protocol
 * http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param buf
 * 		Pointer to buffer to store the expanded data
 * @param len
 * 		Pointer to length of the data
 * @param proto
 * 		Pointer to protocol information
 * @return
 * 		IR_OK if data expanded successfully, one of several errormessages if not
 */
int8_t expandSIRC(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}


#if (IR_PROTOCOLS_USE_RC5)
/**
 * Test data on RC5 protocol 
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param buf
 * 		Pointer to buffer to where to data to parse is stored
 * @param len
 * 		Length of the data
 * @param proto
 * 		Pointer to protocol information
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

	proto->protocol=IR_PROTO_RC5;
	proto->timeout=IR_RC5_TIMEOUT;
	//support RC5-extended keeping second startbit 
	//remove togglebit
	proto->data = rawbits&0x37ff;

	
	return IR_OK;
}
#endif

/**
 * Expand data from RC5 protocol
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param buf
 * 		Pointer to buffer to store the expanded data
 * @param len
 * 		Pointer to length of the data
 * @param proto
 * 		Pointer to protocol information
 * @return
 * 		IR_OK if data expanded successfully, one of several errormessages if not
 */
int8_t expandRC5(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	uint8_t tempadress;
	uint8_t tempcommand;
	uint8_t previousBit;
	
	//TODO: Implement this function. Martin testar!
	/* Set up startbit */
	buf[0] = 0; // no start-one
	buf[1] = IR_RC5_HALF_BIT;
	buf[2] = IR_RC5_HALF_BIT;//first start bit
	buf[3] = IR_RC5_HALF_BIT;
	buf[4] = IR_RC5_HALF_BIT;//second start bit

#define Toggle 0 //TODO: Fix toggle in some proper way
	if (Toggle==0){
		buf[5] = IR_RC5_HALF_BIT;
		buf[6] = IR_RC5_HALF_BIT; //toggle bit (yes i know it should not be hardcoded)
		*len = 7;
	} else {
		buf[4] = IR_RC5_BIT;
		buf[5] = IR_RC5_HALF_BIT;
		*len = 6;
	}
		
	
	previousBit = 1;
	
	//let's start with the adress bits..
	tempadress = (proto->data>>8)&(0b00011111);
	tempcommand = proto->data&(0b00111111);
	

	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 5;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = tempadress;
	//txMsg.Data.words[1] = tempcommand;
	//txMsg.Data.words[2] = *len+3;
	//BIOS_CanSend(&txMsg);
	
	tempadress = tempadress<<2;
	
	for(uint8_t i = 0; i < 5; i++) {
		tempadress = tempadress<<1;
		//temptemp = tempadress&0xA0;
		if (((tempadress&(0xA0))>>7)==1){
			if (previousBit == 1){//11
				buf[*len] = IR_RC5_HALF_BIT;
				buf[*len+1] = IR_RC5_HALF_BIT;
				*len = *len + 2;
			} else {//01
				buf[*len-1] = IR_RC5_BIT;
				buf[*len] = IR_RC5_HALF_BIT;
				*len = *len + 1;
			}
			previousBit = 1;
		} else {//10
			if (previousBit == 1){
				buf[*len-1] = IR_RC5_BIT;
				buf[*len] = IR_RC5_HALF_BIT;
				*len = *len + 1;
			} else {//00
				buf[*len] = IR_RC5_HALF_BIT;
				buf[*len+1] = IR_RC5_HALF_BIT;
				*len = *len + 2;
			}
			previousBit = 0;
		}
	}
	
	txMsg.DataLength = 6;
	
	tempcommand = tempcommand << 1;
	
	for(uint8_t i = 0; i < 6; i++) {
		tempcommand = tempcommand<<1;
		//temptemp = tempadress&0xA0;
		if (((tempcommand&(0xA0))>>7)==1){
			if (previousBit == 1){//11
				buf[*len] = IR_RC5_HALF_BIT;
				buf[*len+1] = IR_RC5_HALF_BIT;
				*len = *len + 2;
			} else {//01
				buf[*len-1] = IR_RC5_BIT;
				buf[*len] = IR_RC5_HALF_BIT;
				*len = *len + 1;
			}
			previousBit = 1;
		} else {//10
			if (previousBit == 1){
				buf[*len-1] = IR_RC5_BIT;
				buf[*len] = IR_RC5_HALF_BIT;
				*len = *len + 1;
			} else {//00
				buf[*len] = IR_RC5_HALF_BIT;
				buf[*len+1] = IR_RC5_HALF_BIT;
				*len = *len + 2;
			}
			previousBit = 0;
		}
	}

	/*
	//time for the command then!
	for(uint8_t i = 0; i < 6; i++) {
		tempcommand = tempcommand << 1;
		if (((tempcommand&0xA0)>>7)==1){
			buf[*len] = 3; //zero at the start
			buf[*len+1] = IR_RC5_HALF_BIT; //keep it low
			buf[*len+2] = IR_RC5_HALF_BIT; //and then, make it high
			buf[*len+3] = 3; //and then we waste another cycle low to get in sync
			*len = *len + 4;
		} else {
			buf[*len] = IR_RC5_HALF_BIT;
			buf[*len+1] = IR_RC5_HALF_BIT;
			*len = *len + 2;
		}
		
	}	
	*/
	proto->modfreq=(((F_CPU/2000)/IR_RC5_F_MOD) -1);
	proto->timeout=IR_RC5_TIMEOUT;
	proto->repeats=IR_RC5_REPS;
	return IR_OK;
	//return IR_NOT_CORRECT_DATA;
}


#if (IR_PROTOCOLS_USE_SHARP)
/**
 * Test data on SHARP protocol
 * http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param buf
 * 		Pointer to buffer to where to data to parse is stored
 * @param len
 * 		Length of the data
 * @param proto
 * 		Pointer to protocol information
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
	
	proto->protocol=IR_PROTO_SHARP;
	proto->timeout=IR_SHARP_TIMEOUT;
	proto->data=rawbits;
	return IR_OK;
}
#endif

/**
 * Expand data from Sharp protocol
 * http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param buf
 * 		Pointer to buffer to store the expanded data
 * @param len
 * 		Pointer to length of the data
 * @param proto
 * 		Pointer to protocol information
 * @return
 * 		IR_OK if data expanded successfully, one of several errormessages if not
 */
int8_t expandSharp(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	return IR_NOT_CORRECT_DATA;
}


#if (IR_PROTOCOLS_USE_NEC)
/**
 * Test data on NEC protocol
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param buf
 * 		Pointer to buffer to where to data to parse is stored
 * @param len
 * 		Length of the data
 * @param proto
 * 		Pointer to protocol information
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

	uint32_t rawbits = 0;

	for (uint8_t i = 3; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_NEC_LOW_ONE - IR_NEC_LOW_ONE/IR_NEC_TOL_DIV && buf[i] < IR_NEC_LOW_ONE + IR_NEC_LOW_ONE/IR_NEC_TOL_DIV) {
				/* write a one */
				rawbits |= 1UL<<((i-3)>>1);
			} else if (buf[i] > IR_NEC_LOW_ZERO - IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV && buf[i] < IR_NEC_LOW_ZERO + IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV) {
				/* do nothing, a zero is already in place */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_NEC_HIGH + IR_NEC_HIGH/IR_NEC_TOL_DIV || buf[i] < IR_NEC_HIGH - IR_NEC_HIGH/IR_NEC_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}

	proto->protocol=IR_PROTO_NEC;
	proto->timeout=IR_NEC_TIMEOUT;
	proto->data=rawbits;	
	return IR_OK;
}
#endif

/**
 * Expand data from NEC protocol
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param buf
 * 		Pointer to buffer to store the expanded data
 * @param len
 * 		Pointer to length of the data
 * @param proto
 * 		Pointer to protocol information
 * @return
 * 		IR_OK if data expanded successfully, one of several errormessages if not
 */
int8_t expandNEC(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	/* Set up startbit */
	buf[0] = IR_NEC_ST_BIT;
	
	if (proto->framecnt == 0) {
		buf[1] = IR_NEC_ST_PAUSE;
	
		*len = 67;
		for (uint8_t i = 0; i < 65; i++) {
			if ((i&1) == 1) {		/* if odd, ir-pause */
				if ((proto->data>>(i>>1))&1) {
					buf[i+2] = IR_NEC_LOW_ONE;
				} else {
					buf[i+2] = IR_NEC_LOW_ZERO;
				}
			} else {			/* if even, ir-bit */
				buf[i+2] = IR_NEC_HIGH;
			}
		}
		proto->timeout=IR_NEC_TIMEOUT;
	} else {
		buf[1] = IR_NEC_ST_PAUSE/2;
		buf[2] = IR_NEC_HIGH;
		proto->timeout=IR_NEC_ST_TIMEOUT;
		*len = 3;
	}
	proto->modfreq=(((F_CPU/2000)/IR_NEC_F_MOD) -1);
	proto->repeats=IR_NEC_REPS;
	return IR_OK;
}


#if (IR_PROTOCOLS_USE_SAMSUNG)
/**
 * Test data on Samsung protocol
 * Very much like NEC, different start bit/pause lengths etc.
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param buf
 * 		Pointer to buffer to where to data to parse is stored
 * @param len
 * 		Length of the data
 * @param proto
 * 		Pointer to protocol information
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

	uint32_t rawbits = 0;
	
	for (uint8_t i = 3; i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_SAMS_LOW_ONE - IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV && buf[i] < IR_SAMS_LOW_ONE + IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV) {
				/* write a one */
				rawbits |= 1UL<<((i-3)>>1);
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
	
	proto->protocol=IR_PROTO_SAMS;
	proto->timeout=IR_SAMS_TIMEOUT;
	proto->data=rawbits;	
	return IR_OK;
}
#endif

/**
 * Expand data from Samsung protocol
 * Very much like NEC, different start bit/pause lengths etc.
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param buf
 * 		Pointer to buffer to store the expanded data
 * @param len
 * 		Pointer to length of the data
 * @param proto
 * 		Pointer to protocol information
 * @return
 * 		IR_OK if data expanded successfully, one of several errormessages if not
 */
int8_t expandSamsung(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	/* Set up startbit */
	buf[0] = IR_SAMS_ST_BIT;
	buf[1] = IR_SAMS_ST_PAUSE;
	
	for (uint8_t i = 0; i < 65; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((proto->data>>(i>>1))&1) {
				buf[i+2] = IR_SAMS_LOW_ONE;
			} else {
				buf[i+2] = IR_SAMS_LOW_ZERO;
			}
		} else {				/* if even, ir-bit */
			buf[i+2] = IR_SAMS_HIGH;
		}
	}
	
	*len = 67;
	
	proto->modfreq=(((F_CPU/2000)/IR_SAMS_F_MOD) -1);
	proto->timeout=IR_SAMS_TIMEOUT;
	proto->repeats=IR_SAMS_REPS;
	return IR_OK;
}
