/**
 * IR receiver and transmitter protocols.
 * 
 * @date	2006-12-10
 * 
 * @author	Anders Runeson, Andreas Fritiofson, Martin Nordin
 *   
 */

#include "protocols.h"
#include <bios.h>

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
#if (IR_PROTOCOLS_USE_MARANTZ)
	if (parseMarantz(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_PANASONIC)
	if (parsePanasonic(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_SKY)
	if (parseSky(buf, len, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_NEXA2)
	if (parseNexa2(buf, len, proto)==IR_OK) return IR_OK;
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
	case IR_PROTO_MARANTZ:
		return expandMarantz(buf, len, proto);
	case IR_PROTO_PANASONIC:
		return expandPanasonic(buf, len, proto);
	case IR_PROTO_SKY:
		return expandSky(buf, len, proto);
	}
	/* Invalid protocol specified. */
	return IR_NOT_CORRECT_DATA;
}

#if (IR_PROTOCOLS_USE_SIRC)
/**
 * Test data on SIRC protocol, 12-bit version
 * http://www.sbprojects.com/knowledge/ir/sirc.htm
 * http://picprojects.org.uk/projects/sirc/sonysirc.pdf
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

	/* check if we have correct amount of data.
           supporting two versions of SIRC:
           12 bit = 25, 15 bit = 31
           there is also a 20 bit protocol, but we don't support it
         */
	if (len != 25 && len != 31) {
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
	buf[0] = IR_SIRC_ST_BIT;
	buf[1] = IR_SIRC_LOW; //start pulse finished

        /* Assume 12 bit protocol */
        *len = 25;
        /* If data to big, use 15 bit protocol */
        if (proto->data > (1<<11)) { // cannot be represented by 12 bits
          *len = 31;
        }
        for (uint8_t i = 0; i < *len-2; i++) {
          if ((i&1) == 1) {		/* if odd, ir-pause */
            buf[i+2] = IR_SIRC_LOW;
          } else {			/* if even, ir-bit */
            if ((proto->data>>(i>>1))&1) {
              buf[i+2] = IR_SIRC_HIGH_ONE;
            } else {
              buf[i+2] = IR_SIRC_HIGH_ZERO;
            }
          }
        }	

	proto->modfreq=IR_SIRC_F_MOD;
	proto->timeout=IR_SIRC_TIMEOUT;
	proto->repeats=IR_SIRC_REPS;
        
	return IR_OK;
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
	proto->data = rawbits&0x37ff; //This seems to be wrong? Does not invert second start bit and keeps first start bit
	//proto->data = (rawbits&0x07ff) | ((~rawbits)&0x0100);

	
	return IR_OK;
}
#endif

/**
 * Used by the expandRC5 to ensure that we toggle the signal with each button press.
 */
int8_t rc5_toggle=0;

/**
 * Expand data from RC5 protocol
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * One is defined as low then high
 * Zero is defined as high then low
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

	//This is the raw message that we should create the IR times for
	//Lets copy the data locally to ensure that no interups will modify the vector.
	uint16_t rawMessage=proto->data & 0x3fff;
	
	uint8_t previousBit;
	/* Set up startbit */
	//Bit = 0
	//We start with a low signal since the diode 
	//isn't active before we send anything,
	buf[0] = IR_RC5_HALF_BIT;//first start bit
	
	// Bit = 1
	buf[1] = IR_RC5_HALF_BIT;
	buf[2] = IR_RC5_HALF_BIT;//second start bit

	if (rc5_toggle==0){
		buf[3] = IR_RC5_HALF_BIT;
		buf[4] = IR_RC5_HALF_BIT; //toggle bit (yes i know it should not be hardcoded)
		*len = 5;
		previousBit = 1; //Same as last startbit
	} else {
		//We are reusing the signal from the previous signal 
		//and extend the time into this bit.
		buf[2] = IR_RC5_BIT;
		buf[3] = IR_RC5_HALF_BIT;
		*len = 4;
		previousBit = 0; //Toggled from last startbit
	}
	//Invert the toggle for next time
	rc5_toggle=!rc5_toggle & 1;
	
	//Decode the message
	//We know that RC5 messages are 14 bits long
	for(uint8_t pos=11;pos>0;pos--)
	{		
		// Check the current bit
		if(previousBit == ((rawMessage>>(pos-1)) & 1))
		{
			buf[*len]=IR_RC5_HALF_BIT;
			buf[*len+1]=IR_RC5_HALF_BIT;
			*len=*len+2;
		}
		else
		{
			//We are having the same signal as we ended the last bit with,
			//Expand the time that that signal is active to cover 
			//half of this bit aswell
			buf[*len-1]=IR_RC5_BIT;
			buf[*len]=IR_RC5_HALF_BIT;
			*len=*len+1;
			
			//Invert the previous bit
			previousBit = (!previousBit) & 1;
		}
	}
	//We have to handle the last bit specially since we have to 
	//end with low signal on the IR diod
	if(previousBit == 0)
	{
		//We have to remove the last time since that would bring us to a high signal again.
		*len=*len-1;
		buf[*len]=0;
	}
	
	proto->modfreq=IR_RC5_F_MOD;
	proto->timeout=IR_RC5_TIMEOUT;
	proto->repeats=IR_RC5_REPS;
	return IR_OK;
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
	proto->modfreq=IR_NEC_F_MOD;
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
	
	proto->modfreq=IR_SAMS_F_MOD;
	proto->timeout=IR_SAMS_TIMEOUT;
	proto->repeats=IR_SAMS_REPS;
	return IR_OK;
}

#if (IR_PROTOCOLS_USE_MARANTZ)
/**
 * Test data on Marantz protocol 
 * Reverse-Engineered by Noddan, very similar to RC-5.
 * Not tested with odd adresses since I have no remote that sends them.
 * Don't know what happens with the extra long bit in that case.
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
int8_t parseMarantz(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	uint8_t halfbitscnt = 1;
	uint32_t rawbits = 0;
	
	for (uint8_t i = 0; i<len; i++) {
		//halfbitscnt&1==1 in the middle of bits
		//i&1==0 positive flank

		if ((halfbitscnt&1)==1 && (i&1)==0) {		/* in the middle of bit AND a positve flank */
			rawbits |= (uint32_t)1<<(19-(halfbitscnt>>1));
		}
		
		if (buf[i] > IR_MARANTZ_HALF_BIT - IR_MARANTZ_HALF_BIT/IR_MARANTZ_TOL_DIV && buf[i] < IR_MARANTZ_HALF_BIT + IR_MARANTZ_HALF_BIT/IR_MARANTZ_TOL_DIV) {
			halfbitscnt += 1;
		} else if (buf[i] > IR_MARANTZ_BIT - IR_MARANTZ_BIT/IR_MARANTZ_TOL_DIV && buf[i] < IR_MARANTZ_BIT + IR_MARANTZ_BIT/IR_MARANTZ_TOL_DIV) {
			halfbitscnt += 2;
		} else if (buf[i] > IR_MARANTZ_BIT - IR_MARANTZ_BIT/IR_MARANTZ_TOL_DIV && buf[i] < 5*IR_MARANTZ_HALF_BIT + IR_MARANTZ_BIT/IR_MARANTZ_TOL_DIV) {
			halfbitscnt += 1; //It seems to work, not entirely sure of the purpose of this long zero though.
		} else {
			return IR_NOT_CORRECT_DATA;
		}
		
	}
	
	proto->protocol=IR_PROTO_MARANTZ;
	proto->timeout=IR_MARANTZ_TIMEOUT;
	proto->data = rawbits&0x0001ffff;
	
	return IR_OK;
}
#endif

/**
 * Expand data from Marantz. Written by Martin Nordin
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
int8_t expandMarantz(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	uint8_t previousBit;
	uint32_t tempdata;
	
	/* Set up startbits */
	buf[0] = IR_MARANTZ_HALF_BIT;//first start bit
	buf[1] = IR_MARANTZ_HALF_BIT;
	buf[2] = IR_MARANTZ_HALF_BIT;//second start bit
	//TODO: Toggle bit should be better, not hard-coded
	buf[3] = IR_MARANTZ_HALF_BIT;
	buf[4] = IR_MARANTZ_HALF_BIT;//toggle bit
	*len=5;
	previousBit = 1;
	
	tempdata = (uint32_t)(proto->data)<<14;
		
	for(uint8_t i = 0; i < 17; i++) {
		tempdata = (uint32_t)tempdata<<1; 

		if (((uint32_t)tempdata>>31)==1){
			if (previousBit == 1){//11
				buf[*len] = IR_MARANTZ_HALF_BIT;
				buf[*len+1] = IR_MARANTZ_HALF_BIT;
				*len = *len + 2;
			} else {//01
				buf[*len-1] = IR_MARANTZ_BIT;
				buf[*len] = IR_MARANTZ_HALF_BIT;
				*len = *len + 1;
			}
			previousBit = 1;
		} else {
			if (previousBit == 1){//10
				buf[*len-1] = IR_MARANTZ_BIT;
				buf[*len] = IR_MARANTZ_HALF_BIT;
				*len = *len + 1;
			} else {//00
				buf[*len] = IR_MARANTZ_HALF_BIT;
				if (i==4){
					buf[*len+1] = IR_MARANTZ_HALF_BIT*5;
				} else {
					buf[*len+1] = IR_MARANTZ_HALF_BIT;
				}
				
				*len = *len + 2;
			}
			previousBit = 0;
		}
	}
	//make sure that we finish high by removing the last zero if needed
	if (*len%2 == 0){
		*len = *len - 1;
	} 

	proto->modfreq=IR_MARANTZ_F_MOD;
	proto->timeout=IR_MARANTZ_TIMEOUT;
	proto->repeats=IR_MARANTZ_REPS;
	return IR_OK;
}

#if (IR_PROTOCOLS_USE_PANASONIC)
/**
 * Test data on Panasonic protocol
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
int8_t parsePanasonic(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 99) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (buf[0] > IR_PANA_ST_BIT + IR_PANA_ST_BIT/IR_PANA_TOL_DIV || buf[0] < IR_PANA_ST_BIT - IR_PANA_ST_BIT/IR_PANA_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	/* check pause after startbit */
	if (buf[1] > IR_PANA_ST_PAUSE + IR_PANA_ST_PAUSE/IR_PANA_TOL_DIV || buf[1] < IR_PANA_ST_PAUSE - IR_PANA_ST_PAUSE/IR_PANA_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	uint32_t rawbits = 0;
	
	/* skip start bit, start bit pause and first 16 bits (32 values) */
	for (uint8_t i = (3+16*2); i < len; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[i] > IR_PANA_LOW_ONE - IR_PANA_LOW_ONE/IR_PANA_TOL_DIV && buf[i] < IR_PANA_LOW_ONE + IR_PANA_LOW_ONE/IR_PANA_TOL_DIV) {
				/* write a one */
				rawbits |= 1UL<<((i-(3+16*2))>>1);
			} else if (buf[i] > IR_PANA_LOW_ZERO - IR_PANA_LOW_ZERO/IR_PANA_TOL_DIV && buf[i] < IR_PANA_LOW_ZERO + IR_PANA_LOW_ZERO/IR_PANA_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (buf[i] > IR_PANA_HIGH + IR_PANA_HIGH/IR_PANA_TOL_DIV || buf[i] < IR_PANA_HIGH - IR_PANA_HIGH/IR_PANA_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	proto->protocol=IR_PROTO_PANASONIC;
	proto->timeout=IR_PANA_TIMEOUT;
	proto->data=rawbits;	
	return IR_OK;
}
#endif

/**
 * Expand data from Panasonic protocol
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
int8_t expandPanasonic(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	/* Set up startbit */
	buf[0] = IR_PANA_ST_BIT;
	buf[1] = IR_PANA_ST_PAUSE;
	
	/* add the first 16 static bits */
	uint16_t staticBits = 0x2002;
	for (uint8_t i = 0; i < 32; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((staticBits>>(i>>1))&1) {
				buf[i+2] = IR_PANA_LOW_ONE;
			} else {
				buf[i+2] = IR_PANA_LOW_ZERO;
			}
		} else {				/* if even, ir-bit */
			buf[i+2] = IR_PANA_HIGH;
		}
	}
	
	/* then add the value bits */
	for (uint8_t i = 0; i < 65; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((proto->data>>(i>>1))&1) {
				buf[i+2+32] = IR_PANA_LOW_ONE;
			} else {
				buf[i+2+32] = IR_PANA_LOW_ZERO;
			}
		} else {				/* if even, ir-bit */
			buf[i+2+32] = IR_PANA_HIGH;
		}
	}
	
	*len = 99;
	
	proto->modfreq=IR_PANA_F_MOD;
	proto->timeout=IR_PANA_TIMEOUT;
	proto->repeats=IR_PANA_REPS;
	return IR_OK;
}

#if (IR_PROTOCOLS_USE_SKY)
/**
 * Test data on Sky protocol
 * 
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
int8_t parseSky(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check startbit */
	if (buf[0] > IR_SKY_ST_BIT + IR_SKY_ST_BIT/IR_SKY_TOL_DIV || buf[0] < IR_SKY_ST_BIT - IR_SKY_ST_BIT/IR_SKY_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint32_t rawbits=0;
	uint8_t current=0;
	uint8_t previous=0;
	uint8_t cnt=0;
#define SKYLONG 0
#define SKYSHORT 1
	for (uint8_t i = 1; i < len; i++) 
	{
		if (buf[i] > IR_SKY_SHORT - IR_SKY_SHORT/IR_SKY_TOL_DIV && buf[i] < IR_SKY_SHORT + IR_SKY_SHORT/IR_SKY_TOL_DIV) {
			current = SKYSHORT;
		}
		else if (buf[i] > IR_SKY_LONG - IR_SKY_LONG/IR_SKY_TOL_DIV && buf[i] < IR_SKY_LONG + IR_SKY_LONG/IR_SKY_TOL_DIV) {
			current = SKYLONG;
		}
		else {
			return IR_NOT_CORRECT_DATA;
		}
		
		/* if level is low */
		if ((rawbits&1)==0) {
			/* and there is a long pulse */
			if (current == SKYLONG) {
				/* push a one */
				rawbits = rawbits<<1;
				rawbits |= 1;
				cnt = 0;
			}
			else if (cnt == 0) {
				cnt=1;
				/* push a zero */
				rawbits = rawbits<<1;
				
			} 
			else {
				cnt = 0;
			}
		}
		
		/* if level is high */
		if ((rawbits&1)==1) {
			/* and there is a long pulse */
			if (current == SKYLONG) {
				/* push a zero */
				rawbits = rawbits<<1;
				
				if (previous == SKYLONG) {
					cnt = 1;
				}
				else {
					cnt = 0;
				}
			}
			else if (cnt == 0) {
				cnt=1;
				/* push a one */
				rawbits = rawbits<<1;
				rawbits |= 1;
			} 
			else {
				cnt = 0;
			}
		}
		
		previous=current;
		
	}
	
	proto->protocol = IR_PROTO_SKY;
	proto->timeout = IR_SKY_TIMEOUT;
	proto->data = rawbits; 
	
	return IR_OK;
}
#endif

/**
 * Expand data from Sky protocol
 * 
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
int8_t expandSky(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	//TODO: Implement this function.
	buf[0] = IR_SKY_ST_BIT;
	buf[1] = IR_SKY_LONG;	//
	
	
	return IR_NOT_CORRECT_DATA;
}

#if (IR_PROTOCOLS_USE_NEXA2)
/**
 * Test data on NEXA protocol
 * http://elektronikforumet.com/wiki/index.php?title=RF_Protokoll_-_Nexa_sj%C3%A4lvl%C3%A4rande
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
int8_t parseNexa2(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	/* check if we have correct amount of data */ 
	if (len != 146 || len != 130) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint32_t rawbits = 0;
	uint64_t rawbitsTemp = 0;
	if (buf[1] < IR_NEXA2_START - IR_NEXA2_START/IR_NEXA2_TOL_DIV && buf[1] > IR_NEXA2_START + IR_NEXA2_START/IR_NEXA2_TOL_DIV) { //check start bit
		return IR_NOT_CORRECT_DATA;
	}
	  
	  uint8_t bitCounter = 0;
	for (uint8_t i = 2; i < len; i++) {	//Skip first bit
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (buf[1] > IR_NEXA2_LOW_ONE - IR_NEXA2_LOW_ONE/IR_NEXA2_TOL_DIV && buf[1] < IR_NEXA2_LOW_ONE + IR_NEXA2_LOW_ONE/IR_NEXA2_TOL_DIV) {
				/* write a one */
				rawbitsTemp |= 1<<(bitCounter++);
			} else if (buf[1] > IR_NEXA2_LOW_ZERO - IR_NEXA2_LOW_ZERO/IR_NEXA2_TOL_DIV && buf[1] < IR_NEXA2_LOW_ZERO + IR_NEXA2_LOW_ZERO/IR_NEXA2_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
				bitCounter++;
			} else {
				return IR_NOT_CORRECT_DATA;
			}
			i+=2; 	// skip every other bit,implement check here in the future
		} else {			/* if even, ir-bit */
			if (buf[1] < IR_NEXA2_HIGH - IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV && buf[1] > IR_NEXA2_HIGH + IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	
	rawbits = (uint32_t) (0xffff & rawbitsTemp);
	
	proto->protocol=IR_PROTO_NEXA2;
	proto->timeout=IR_NEXA2_TIMEOUT;
	proto->data=rawbits;
	return IR_OK;
}
#endif

