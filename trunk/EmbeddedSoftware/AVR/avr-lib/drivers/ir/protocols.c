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

#include <drivers/can/moduleid.h>

//#include <drivers/mcu/gpio.h>



int8_t parseProtocol(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) {
	uint8_t res;
	proto->protocol=IR_PROTO_UNKNOWN;
	proto->data=0;
	proto->timeout=1;
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
#if (IR_PROTOCOLS_USE_IROBOT)
	if (parseiRobot(buf, len, proto)==IR_OK) return IR_OK;
#endif


/* RF protocols needs index parameter */
#if (IR_PROTOCOLS_USE_NEXA2)
	if (parseNexa2(buf, len, index, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_NEXA1)
	if (parseNexa1(buf, len, index, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_VIKING)
	if (parseViking(buf, len, index, proto)==IR_OK) return IR_OK;
#endif
#if (IR_PROTOCOLS_USE_VIKING_STEAK)
	res = parseVikingSteak(buf, len, index, proto);
	if (res!=IR_NOT_CORRECT_DATA) return res;
#endif
#if (IR_PROTOCOLS_USE_RUBICSON)
	res = parseRubicson(buf, len, index, proto);
	if (res!=IR_NOT_CORRECT_DATA) return res;
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
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SIRC:
		return expandSIRC(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC5:
		return expandRC5(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SHARP:
		return expandSharp(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEC:
		return expandNEC(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SAMSUNG:
		return expandSamsung(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_MARANTZ:
		return expandMarantz(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_PANASONIC:
		return expandPanasonic(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SKY:
		return expandSky(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_IROBOT:
		return expandiRobot(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA2:
		return expandNexa2(buf, len, proto);
	case CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA:
		return expandNexa1(buf, len, proto);
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
	
	proto->protocol = CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SIRC;
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

	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RC5;
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
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SHARP;
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

	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEC;
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
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SAMSUNG;
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
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_MARANTZ;
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
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_PANASONIC;
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
	
	proto->protocol = CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_SKY;
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

#if (IR_PROTOCOLS_USE_IROBOT)
/**
 * Test data on iRobot protocol
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
int8_t parseiRobot(const uint16_t *buf, uint8_t len, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */	
	uint32_t rawbits=0;
	uint8_t current=0;
	uint8_t previous=0;
	uint8_t cnt=0;
#define IROBOTLONG 0
#define IROBOTSHORT 1

	/* check if we have correct amount of data */ 
	if (len != 16) {
		return IR_NOT_CORRECT_DATA;
	}
	

	for (uint8_t i = 0; i < len; i++) 
	{
		if (buf[i] > IR_IROBOT_SHORT - IR_IROBOT_SHORT/IR_IROBOT_TOL_DIV && buf[i] < IR_IROBOT_SHORT + IR_IROBOT_SHORT/IR_IROBOT_TOL_DIV) {
			current = IROBOTSHORT;
		}
		else if (buf[i] > IR_IROBOT_LONG - IR_IROBOT_LONG/IR_IROBOT_TOL_DIV && buf[i] < IR_IROBOT_LONG + IR_IROBOT_LONG/IR_IROBOT_TOL_DIV) {
			current = IROBOTLONG;
		}
		else {
			return IR_NOT_CORRECT_DATA;
		}
		
		/* if level is low */
		if ((rawbits&1)==0) {
			/* and there is a long pulse */
			if (current == IROBOTLONG) {
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
			if (current == IROBOTLONG) {
				/* push a zero */
				rawbits = rawbits<<1;
				
				if (previous == IROBOTLONG) {
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
	
	proto->protocol = CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_IROBOT;
	proto->timeout = IR_IROBOT_TIMEOUT;
	proto->data = rawbits; 
	
	return IR_OK;
}
#endif

/**
 * Expand data from iRobot protocol
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
int8_t expandiRobot(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	uint8_t temp;
	uint8_t lookup[16] = {
				   0x0, 0x8, 0x4, 0xC,
				   0x2, 0xA, 0x6, 0xE,
				   0x1, 0x9, 0x5, 0xD,
				   0x3, 0xB, 0x7, 0xF };
	temp = (uint8_t)proto->data;
	temp = (lookup[temp &0x0F] << 4) | lookup[temp >>4];
	proto->data = temp;
	//proto->data = temp << 8;
	//proto->data += 0x52;
	for (uint8_t i = 0; i < 16; i++) {
		if ((proto->data>>(i>>1))&1) {
			buf[i] = IR_IROBOT_LONG;
			i++;
			buf[i] = IR_IROBOT_SHORT;	
		} else {
			buf[i] = IR_IROBOT_SHORT;
			i++;
			buf[i] = IR_IROBOT_LONG;
		}
	}
	
	*len = 15;
	proto->modfreq=IR_IROBOT_F_MOD;
	proto->timeout=IR_IROBOT_TIMEOUT;
	proto->repeats=IR_IROBOT_REPS;
	return IR_OK;
}


#if (IR_PROTOCOLS_USE_NEXA2)
/**
 * Test data on NEXA protocol
 * http://elektronikforumet.com/wiki/index.php?title=RF_Protokoll_-_Nexa_sj%C3%A4lvl%C3%A4rande
 * http://pastebin.com/PJX3bRAs
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

int8_t parseNexa2(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) 
{
	/* check if we have correct amount of data */ 
	if (len < 132) {
		return IR_NOT_CORRECT_DATA;
	}
	uint8_t i;
#if IR_RX_CONTINUOUS_MODE==0
	i = 0;
#else
	i=index-132;
	if (i>index)
		i+=MAX_NR_TIMES;
#endif
	if ((buf[i] < IR_NEXA2_START1 - IR_NEXA2_START1/IR_NEXA2_TOL_DIV) || (buf[i] > IR_NEXA2_START1 + IR_NEXA2_START1/IR_NEXA2_TOL_DIV)) { //check start bit
		return IR_NOT_CORRECT_DATA;
	}
#if IR_RX_CONTINUOUS_MODE==0
	i = 1;
#else
	i=index-131;
	if (i>index)
		i+=MAX_NR_TIMES;
#endif
	if ((buf[i] < IR_NEXA2_HIGH - IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV) || (buf[i] > IR_NEXA2_HIGH + IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV)) { //check start bit
		return IR_NOT_CORRECT_DATA;
	}
#if IR_RX_CONTINUOUS_MODE==0
	i = 2;
#else
	i=index-130;
	if (i>index)
		i+=MAX_NR_TIMES;
#endif
	if ((buf[i] < IR_NEXA2_START2 - IR_NEXA2_START2/IR_NEXA2_TOL_DIV) || (buf[i] > IR_NEXA2_START2 + IR_NEXA2_START2/IR_NEXA2_TOL_DIV)) { //check start bit
		return IR_NOT_CORRECT_DATA;
	}

	/* Incoming data could actually be longer than 32bits when a dimming command is received */
	uint64_t rawbitsTemp = 0;
	uint8_t bitCounter = 0;
	uint8_t i2;
	for (i = 3; i < 132; i++) {
#if IR_RX_CONTINUOUS_MODE==0
		i2 = i;
#else
		i2=index-(132-i);
		if (i2>index)
			i2+=MAX_NR_TIMES;
#endif
		if ((i&1) == 0) {		/* if even, data */
			/* check length of transmit pulse */
			if ((buf[i2] > IR_NEXA2_LOW_ONE - IR_NEXA2_LOW_ONE/IR_NEXA2_TOL_DIV) && (buf[i2] < IR_NEXA2_LOW_ONE + IR_NEXA2_LOW_ONE/IR_NEXA2_TOL_DIV)) {
				/* write a one */
				rawbitsTemp |= (1UL)<<(bitCounter++);
			} else if ((buf[i2] > IR_NEXA2_LOW_ZERO - IR_NEXA2_LOW_ZERO/IR_NEXA2_TOL_DIV) && (buf[i2] < IR_NEXA2_LOW_ZERO + IR_NEXA2_LOW_ZERO/IR_NEXA2_TOL_DIV)) {
				/* do nothing, a zero is already in rawbits */
				bitCounter++;
			} else {
				return IR_NOT_CORRECT_DATA;
			}
			i+=2; 	// skip every other bit, implement check here in the future
		} else {			/* if odd, no data */
			if ((buf[i2] < IR_NEXA2_HIGH - IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV) || (buf[i2] > IR_NEXA2_HIGH + IR_NEXA2_HIGH/IR_NEXA2_TOL_DIV)) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA2;
	proto->timeout=IR_NEXA2_TIMEOUT;
	proto->data=rawbitsTemp;

	return IR_OK;
}
#endif

/**
 * Expand data from Nexa2 protocol
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
int8_t expandNexa2(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	buf[0] = IR_NEXA2_HIGH;
	buf[1] = IR_NEXA2_START2;

	uint64_t tempshift = proto->data;

	/* No dimming */
	*len = 131;
	uint8_t dimming = 0;
	/* If most significant bit is set, then dimming should be sent */
	if ((uint32_t)(tempshift>> 32)&0x80)
	{
		/* Dimming */
		*len = 147;
		dimming=1;
	}
	
	for (uint8_t i = 2; i < *len; i+=4)
	{
		buf[i] = IR_NEXA2_HIGH;
		buf[i+2] = IR_NEXA2_HIGH;
		if (tempshift&1) {
			buf[i+1] = IR_NEXA2_LOW_ONE;
			buf[i+3] = IR_NEXA2_LOW_ZERO;
		} else {
			buf[i+1] = IR_NEXA2_LOW_ZERO;
			buf[i+3] = IR_NEXA2_LOW_ONE;
		}
		tempshift = tempshift>>1;
	}
	
	if (dimming)
	{
		buf[111] = IR_NEXA2_LOW_ONE;
		buf[113] = IR_NEXA2_LOW_ONE;
	}
	proto->modfreq=IR_NEXA2_F_MOD;
	proto->timeout=IR_NEXA2_START1/1000;
	proto->repeats=IR_NEXA2_REPS;
	return IR_OK;
}


#if (IR_PROTOCOLS_USE_NEXA1)
/**
 * Test data on NEXA protocol
 * http://www.elektronikforumet.com/wiki/index.php/RF_Protokoll_-_Nexa/Proove_(%C3%A4ldre,_ej_sj%C3%A4lvl%C3%A4rande)
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
int8_t parseNexa1(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) {
	/* parse buf[], max is len */

	uint8_t i;
	/* check if we have correct amount of data */ 
	if (len < 50) {
		return IR_NOT_CORRECT_DATA;
	}
#if IR_RX_CONTINUOUS_MODE==0
	i = 0;
#else
	i=index-50;
	if (i>index)
		i+=MAX_NR_TIMES;
#endif
	if (buf[i] < IR_NEXA1_START - IR_NEXA1_START/IR_NEXA1_TOL_DIV || buf[i] > IR_NEXA1_START + IR_NEXA1_START/IR_NEXA1_TOL_DIV) { //check start bit
		return IR_NOT_CORRECT_DATA;
	}

	uint32_t rawbitsTemp = 0;
	uint8_t bitCounter = 0;

	for (i = 1; i < 48; i+=4) 
	{
		uint8_t i2;
#if IR_RX_CONTINUOUS_MODE==0
		i2 = i;
#else
		i2=index-(50-i);
		if (i2>index)
			i2+=MAX_NR_TIMES;
#endif
		/* Check if '0' bit */
		if (
			(buf[i2+0] > IR_NEXA1_SHORT - IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) && (buf[i2+0] < IR_NEXA1_SHORT + IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) &&
			(buf[i2+1] > IR_NEXA1_LONG  - IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) && (buf[i2+1] < IR_NEXA1_LONG  + IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) &&
			(buf[i2+2] > IR_NEXA1_SHORT - IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) && (buf[i2+2] < IR_NEXA1_SHORT + IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) &&
			(buf[i2+3] > IR_NEXA1_LONG  - IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) && (buf[i2+3] < IR_NEXA1_LONG  + IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) )
		{
			/* write a one */
			rawbitsTemp |= (1UL)<<(bitCounter++);
		}
		/* Check if 'X' bit */
		else if (
			(buf[i2+0] > IR_NEXA1_SHORT - IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) && (buf[i2+0] < IR_NEXA1_SHORT + IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) &&
			(buf[i2+1] > IR_NEXA1_LONG  - IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) && (buf[i2+1] < IR_NEXA1_LONG  + IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) &&
			(buf[i2+2] > IR_NEXA1_LONG  - IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) && (buf[i2+2] < IR_NEXA1_LONG  + IR_NEXA1_LONG /IR_NEXA1_TOL_DIV) &&
			(buf[i2+3] > IR_NEXA1_SHORT - IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) && (buf[i2+3] < IR_NEXA1_SHORT + IR_NEXA1_SHORT/IR_NEXA1_TOL_DIV) )
		{
			/* do nothing, a zero is already in rawbits */
			bitCounter++;
		}
		else
		{
			return IR_NOT_CORRECT_DATA;
		}
	}

	if (rawbitsTemp==0)
	{
		/* Bogus RF data */
		return IR_NOT_CORRECT_DATA;
	}
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_NEXA;
	proto->timeout=IR_NEXA1_TIMEOUT;
	proto->data=rawbitsTemp;
	return IR_OK;
}

#endif

/**
 * Expand data from Nexa1 protocol
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
int8_t expandNexa1(uint16_t *buf, uint8_t *len, Ir_Protocol_Data_t *proto) {
	uint64_t tempshift = proto->data;
  
	/* 12 data bits + 1 stop bit */
	*len = 49;

	/* encode data bits */
	for (uint8_t i = 0; i < 45; i += 4)
	{
		if (tempshift & 1) {
			/* encode 0 bit */
			buf[i+0] = IR_NEXA1_SHORT;
			buf[i+1] = IR_NEXA1_LONG;
			buf[i+2] = IR_NEXA1_SHORT;
			buf[i+3] = IR_NEXA1_LONG;
		} else {
			/* encode X bit */
			buf[i+0] = IR_NEXA1_SHORT;
			buf[i+1] = IR_NEXA1_LONG;
			buf[i+2] = IR_NEXA1_LONG;
			buf[i+3] = IR_NEXA1_SHORT;
		}
		tempshift = tempshift>>1;
	}

	/* encode stop/sync bit */
	buf[48] = IR_NEXA1_SHORT;
	
	proto->modfreq = IR_NEXA1_F_MOD;
	proto->timeout = IR_NEXA1_START/1000;
	proto->repeats = IR_NEXA1_REPS;
	return IR_OK;  	
}


#if (IR_PROTOCOLS_USE_VIKING)
/**
 * Test data on Viking temperature sensor protocol
 * 
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

int8_t parseViking(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) 
{
#if IR_RX_CONTINUOUS_MODE==1
	/* check if we have correct amount of data */ 
	if (len < 90) {
		return IR_NOT_CORRECT_DATA;
	}
	uint8_t i, i2;
	uint64_t rawbitsTemp = 0;//0xffffffffffffffff;
	
	for (i = 90; i > 0; i--)
	{
		i2=index-i;
		if (i2>index)
			i2+=MAX_NR_TIMES;

		/* Check if correct amount of data have been received */
		if ((i == 78) && (rawbitsTemp != 0b00001))
			return IR_NOT_CORRECT_DATA;

		if ((i&1) == 0) 
		{		/* if even, no data */
			if ((buf[i2] < IR_VIKING_LOW - IR_VIKING_LOW/IR_VIKING_TOL_DIV) || (buf[i2] > IR_VIKING_LOW + IR_VIKING_LOW/IR_VIKING_TOL_DIV)) 
			{
				return IR_NOT_CORRECT_DATA;
			}
		} 
		else
		{			/* if odd, data */
			/* check length of transmit pulse */
			if ((buf[i2] > IR_VIKING_HIGH_ONE - IR_VIKING_HIGH_ONE/IR_VIKING_TOL_DIV) && (buf[i2] < IR_VIKING_HIGH_ONE + IR_VIKING_HIGH_ONE/IR_VIKING_TOL_DIV)) 
			{
				/* write a one */
				rawbitsTemp = rawbitsTemp<<1;
				rawbitsTemp |= 1;
			}
			else if ((buf[i2] > IR_VIKING_HIGH_ZERO - IR_VIKING_HIGH_ZERO/IR_VIKING_TOL_DIV) && (buf[i2] < IR_VIKING_HIGH_ZERO + IR_VIKING_HIGH_ZERO/IR_VIKING_TOL_DIV)) 
			{
				/* do nothing, a zero is already in rawbits */
				rawbitsTemp = rawbitsTemp<<1;
			}
			else 
			{
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	rawbitsTemp = ~rawbitsTemp;
	rawbitsTemp = rawbitsTemp&0xFFFFFFFFFF;
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_VIKING;
	proto->timeout=0;
	proto->data=rawbitsTemp;

	return IR_OK;
#else
	return IR_NOT_CORRECT_DATA;
#endif
}
#endif


#if (IR_PROTOCOLS_USE_VIKING_STEAK)
/**
 * Test data on Viking steak temperature sensor protocol
 * 
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

int8_t parseVikingSteak(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) 
{
#if IR_RX_CONTINUOUS_MODE==1
	/* check if we have correct amount of data */ 
	if (len < 74) {
		return IR_NOT_CORRECT_DATA;
	}
	uint8_t i, i2;
	uint64_t rawbitsTemp = 0;//0xffffffffffffffff;
	
	/* Check start bit condition */
	i2=index-74;
	if (i2>index)
		i2+=MAX_NR_TIMES;

	proto->data=i2;		/*Store startindex for debug output */
	
	if ((buf[i2] < IR_VIKING_STEAK_LOW_START - IR_VIKING_STEAK_LOW_START/IR_VIKING_STEAK_TOL_DIV) || (buf[i2] > IR_VIKING_STEAK_LOW_START + IR_VIKING_STEAK_LOW_START/IR_VIKING_STEAK_TOL_DIV)) 
	{
		return IR_NOT_CORRECT_DATA;
	}
	
	for (i = 73; i > 0; i--)
	{
		i2=index-i;
		if (i2>index)
			i2+=MAX_NR_TIMES;

		/* Check if correct amount of data have been received */
		//if ((i == 78) && (rawbitsTemp != 0b00001))
		//	return IR_NOT_CORRECT_DATA;

		if ((i&1) != 0) 
		{		/* if odd, no data */
			if ((buf[i2] < IR_VIKING_STEAK_HIGH - IR_VIKING_STEAK_HIGH/IR_VIKING_STEAK_TOL_DIV) || (buf[i2] > IR_VIKING_STEAK_HIGH + IR_VIKING_STEAK_HIGH/IR_VIKING_STEAK_TOL_DIV)) 
			{
				return IR_NOT_CORRECT_DATA;
			}
		} 
		else
		{			/* if even, data */
			/* check length of transmit pulse */
			if ((buf[i2] > IR_VIKING_STEAK_LOW_ONE - IR_VIKING_STEAK_LOW_ONE/IR_VIKING_STEAK_TOL_DIV) && (buf[i2] < IR_VIKING_STEAK_LOW_ONE + IR_VIKING_STEAK_LOW_ONE/IR_VIKING_STEAK_TOL_DIV)) 
			{
				/* write a one */
				rawbitsTemp = rawbitsTemp<<1;
				rawbitsTemp |= 1;
			}
			else if ((buf[i2] > IR_VIKING_STEAK_LOW_ZERO - IR_VIKING_STEAK_LOW_ZERO/IR_VIKING_STEAK_TOL_DIV) && (buf[i2] < IR_VIKING_STEAK_LOW_ZERO + IR_VIKING_STEAK_LOW_ZERO/IR_VIKING_STEAK_TOL_DIV)) 
			{
				/* do nothing, a zero is already in rawbits */
				rawbitsTemp = rawbitsTemp<<1;
			}
			else 
			{
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	//rawbitsTemp = ~rawbitsTemp;
	//rawbitsTemp = rawbitsTemp&0xFFFFFFFFFF;
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_VIKINGSTEAK;
	proto->timeout=IR_VIKING_STEAK_TIMEOUT;
	proto->data=rawbitsTemp;

	return IR_OK;
#else
	return IR_NOT_CORRECT_DATA;
#endif
}
#endif

#if (IR_PROTOCOLS_USE_RUBICSON)
/**
 * Test data on Rubicson temperature sensor protocol
 * 
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

int8_t parseRubicson(const uint16_t *buf, uint8_t len, uint8_t index, Ir_Protocol_Data_t *proto) 
{
#if IR_RX_CONTINUOUS_MODE==1
	/* check if we have correct amount of data */ 
	if (len < 74) {
		return IR_NOT_CORRECT_DATA;
	}
	uint8_t i, i2;
	uint64_t rawbitsTemp = 0;//0xffffffffffffffff;
	
	/* Check start bit condition */
	i2=index-74;
	if (i2>index)
		i2+=MAX_NR_TIMES;

	proto->data=i2;		/*Store startindex for debug output */
	
	if ((buf[i2] < IR_RUBICSON_LOW_START - IR_RUBICSON_LOW_START/IR_RUBICSON_TOL_DIV) || (buf[i2] > IR_RUBICSON_LOW_START + IR_RUBICSON_LOW_START/IR_RUBICSON_TOL_DIV)) 
	{
		return IR_NOT_CORRECT_DATA;
	}
	
	for (i = 73; i > 0; i--)
	{
		i2=index-i;
		if (i2>index)
			i2+=MAX_NR_TIMES;

		/* Check if correct amount of data have been received */
		//if ((i == 78) && (rawbitsTemp != 0b00001))
		//	return IR_NOT_CORRECT_DATA;

		if ((i&1) != 0) 
		{		/* if odd, no data */
			if ((buf[i2] < IR_RUBICSON_HIGH - IR_RUBICSON_HIGH/IR_RUBICSON_TOL_DIV) || (buf[i2] > IR_RUBICSON_HIGH + IR_RUBICSON_HIGH/IR_RUBICSON_TOL_DIV)) 
			{
				return IR_NOT_CORRECT_DATA;
			}
		} 
		else
		{			/* if even, data */
			/* check length of transmit pulse */
			if ((buf[i2] > IR_RUBICSON_LOW_ONE - IR_RUBICSON_LOW_ONE/IR_RUBICSON_TOL_DIV) && (buf[i2] < IR_RUBICSON_LOW_ONE + IR_RUBICSON_LOW_ONE/IR_RUBICSON_TOL_DIV)) 
			{
				/* write a one */
				rawbitsTemp = rawbitsTemp<<1;
				rawbitsTemp |= 1;
			}
			else if ((buf[i2] > IR_RUBICSON_LOW_ZERO - IR_RUBICSON_LOW_ZERO/IR_RUBICSON_TOL_DIV) && (buf[i2] < IR_RUBICSON_LOW_ZERO + IR_RUBICSON_LOW_ZERO/IR_RUBICSON_TOL_DIV)) 
			{
				/* do nothing, a zero is already in rawbits */
				rawbitsTemp = rawbitsTemp<<1;
			}
			else 
			{
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	//rawbitsTemp = ~rawbitsTemp;
	//rawbitsTemp = rawbitsTemp&0xFFFFFFFFFF;
	
	proto->protocol=CAN_MODULE_ENUM_PHYSICAL_IR_PROTOCOL_RUBICSON;
	proto->timeout=IR_RUBICSON_TIMEOUT;
	proto->data=rawbitsTemp;

	return IR_OK;
#else
	return IR_NOT_CORRECT_DATA;
#endif
}
#endif

