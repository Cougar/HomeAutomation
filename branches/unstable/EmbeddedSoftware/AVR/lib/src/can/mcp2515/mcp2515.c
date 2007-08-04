/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : mcp2515.c
 * Version : 0.9
 * 
 * Summary : MCP2515 "low-level" driver
 *
 * Parts of this code are adapted from a MCP2510 sample-application 
 * by KVASER AB, http://www.kvaser.com (KVASER-code is marked as free)
 *
 * This code-module is free to use but you have to keep the copyright
 * notice.
 *
 *****************************************************************************/

#include <inttypes.h>
#include <stdio.h>
#include <avr/io.h>
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "mcp2515_bittime.h"

#include "can.h"


#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_DDR		DDRB

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	#define SPI_SCK   PB5  
	#define SPI_MISO  PB4 
	#define SPI_MOSI  PB3  
	#define SPI_SS    PB2 
#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
	#define SPI_SCK   PB7
	#define SPI_MISO  PB6 
	#define SPI_MOSI  PB5  
	#define SPI_SS    PB4
#else
	#error "SPI pins undefined for this target (see spi.h)"
#endif

#define SPI_SS_HIGH()	(SPIPORT |= (1<<SPISS))
#define SPI_SS_LOW()	(SPIPORT &= ~(1<<SPISS))

static void SPI_Init(void);
static uint8_t SPI_ReadWrite(uint8_t data);
static uint8_t SPI_Read(void);

#define SPI_DONTCARE (0x00)

/* init as SPI-Master */
static void SPI_Init(void) {
	/* SCK, SS!!, MOSI as outputs */
	SPI_DDR |= (1<<SPI_SCK) | (1<<SPI_SS) | (1<<SPI_MOSI);
	/* MISO as input */
	SPI_DDR &= ~(1<<SPI_MISO);
	/* INIT interface, Master, set clock rate fck/4 */
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);
	/* enable double rate */
	SPSR = (1<<SPI2X); /* we will now gain fck/2 instead of fck/4 */
}

static uint8_t SPI_ReadWrite(uint8_t data) {
	/* set data to send into SPI data register */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* return data read from SPI (if any) */
	return SPDR;
}

static uint8_t SPI_Read(void) {
	return SPI_ReadWrite(SPI_DONTCARE);
}


void MCP2515_Reset(void) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_RESET);
	MCP2515_UNSELECT();
	/* delay at least 128 MCP clock cycles */
	volatile int16_t i;
	for (i=0; i<32000; i++) {
		if (i < 0) break;
	}
}

uint8_t MCP2515_ReadRegister(const uint8_t address) {
	uint8_t ret;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ);
	SPI_ReadWrite(address);
	ret = SPI_Read();
	MCP2515_UNSELECT();
	
	return ret;
}

void MCP2515_ReadRegisterS(const uint8_t address, uint8_t values[], const uint8_t n) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ);
	SPI_ReadWrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		values[i] = SPI_Read();
	}
	MCP2515_UNSELECT();
}

void MCP2515_SetRegister(const uint8_t address, const uint8_t value) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(address);
	SPI_ReadWrite(value);
	MCP2515_UNSELECT();
}

void MCP2515_SetRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		SPI_ReadWrite(values[i]);
	}
	MCP2515_UNSELECT();
}

void MCP2515_ModifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_BITMOD);
	SPI_ReadWrite(address);
	SPI_ReadWrite(mask);
	SPI_ReadWrite(data);
	MCP2515_UNSELECT();
}

static uint8_t MCP2515_ReadXXStatus_Helper(const uint8_t cmd) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(cmd);
	i = SPI_Read();
	MCP2515_UNSELECT();
	
	return i;
}
	
uint8_t MCP2515_ReadStatus(void) {
	return MCP2515_ReadXXStatus_Helper(MCP_READ_STATUS);
}

uint8_t MCP2515_RxStatus(void) {
	return MCP2515_ReadXXStatus_Helper(MCP_RX_STATUS);
}

uint8_t MCP2515_SetCanCtrlMode(const uint8_t newmode) {
	uint8_t i;
	MCP2515_ModifyRegister(MCP_CANCTRL, MODE_MASK, newmode);
	// verify as advised in datasheet
	i = MCP2515_ReadRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}

uint8_t MCP2515_SetClkout(const uint8_t newmode) {
	uint8_t i;
	MCP2515_ModifyRegister(MCP_CANCTRL, CLK_MASK, newmode);
	// verify, perhaps not needed?
	i = MCP2515_ReadRegister(MCP_CANCTRL);
	i &= CLK_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}


uint8_t MCP2515_ConfigRate(const Can_Bitrate_t canBitrate) {
	uint8_t set, cfg1, cfg2, cfg3;
	
	set = 0;
	
	switch (canBitrate) {
		case (CAN_BITRATE_125K) :
			cfg1 = MCP_125KBPS_CFG1;
			cfg2 = MCP_125KBPS_CFG2;
			cfg3 = MCP_125KBPS_CFG3;
			set = 1;
			break;
		case (CAN_BITRATE_250K) :
			cfg1 = MCP_250KBPS_CFG1;
			cfg2 = MCP_250KBPS_CFG2;
			cfg3 = MCP_250KBPS_CFG3;
			set = 1;
			break;
		case (CAN_BITRATE_500K) :
			cfg1 = MCP_500KBPS_CFG1;
			cfg2 = MCP_500KBPS_CFG2;
			cfg3 = MCP_500KBPS_CFG3;
			set = 1;
			break;
		/* 1000kbit/s only available with clock freqs above 8MHz */
		#if MCP_CLOCK_FREQ_MHZ > 8
		case (CAN_BITRATE_1M) :
			cfg1 = MCP_1000KBPS_CFG1;
			cfg2 = MCP_1000KBPS_CFG2;
			cfg3 = MCP_1000KBPS_CFG3;
			set = 1;
			break;
		#endif
		default:
			set = 0;
			break;
	}
	
	if (set) {
		MCP2515_SetRegister(MCP_CNF1, cfg1);
		MCP2515_SetRegister(MCP_CNF2, cfg2);
		MCP2515_SetRegister(MCP_CNF3, cfg3);
		return MCP2515_OK;
	}
	else {
		return MCP2515_FAIL;
	}
} 


/**
 * Reads a CAN message from the specified buffer base address in the MCP2515. The message
 * is stored in the following format:
 * 
 * 		BASE+0:
 * 		| SID10 | SID9  | SID8  | SID7  | SID6  | SID5  | SID4  | SID3  |
 * 		
 * 		BASE+1:
 * 		| SID2  | SID1  | SID0  | SRR   | IDE   |       | EID17 | EID16 |
 * 		
 * 		BASE+2:
 * 		| EID15 | EID14 | EID13 | EID12 | EID11 | EID10 | EID9  | EID8  |
 * 		
 * 		BASE+3:
 * 		| EID7  | EID6  | EID5  | EID4  | EID3  | EID2  | EID1  | EID0  |
 * 		
 * 		BASE+4:
 * 		|       |  RTR  |  RB1  |  RB0  | DLC3  | DLC2  | DLC1  | DCL0  |
 * 		
 * 		BASE+5 - BASE+12:
 * 		DATA0 - DATA7
 *
 * @param buffer_sidh_addr
 * 		Base address to the RX buffer.
 * 
 * @param msg
 * 		Pointer to the message buffer into which the data should be copied.
 * 
 */
void MCP2515_ReadCanMsg(const uint8_t buffer_sidh_addr, Can_Message_t* msg) {
	uint8_t tbufdata[13];
	uint8_t mcp_addr;
	mcp_addr = buffer_sidh_addr;
	
	/* read ID */
	MCP2515_ReadRegisterS(mcp_addr, tbufdata, 13);
	
	/* extract SID10-SID0 */
	msg->Id = 0;
	msg->Id = (((uint32_t)(tbufdata[0])) << 3) + (((uint32_t)(tbufdata[1] & 0xE0)) >> 5);
	
	if ((tbufdata[1] & MCP_TXB_EXIDE_M) != 0) {
		/* extended message */
		msg->ExtendedFlag = 1;
		/* SID10-SID0 are still most significant, so shift them up
		 * and make room for 18 extended least significant EID-bits */
		msg->Id = (uint32_t)(msg->Id) << 18;
		/* exctract EID17-EID16 */ 
		msg->Id = (uint32_t)msg->Id | ((uint32_t)(((uint32_t)(tbufdata[1] & 0x03))<<16));
		/* extract EID15-EID8 */
		msg->Id |= (((uint32_t)tbufdata[2])<<8);
		/* extract EID7-EID0 */
		msg->Id |= ((uint32_t)tbufdata[3]);
		/* mask ID to guarantee 29bit range */
		msg->Id &= 0x1FFFFFFF;
    }
    else {
    	/* standard message */
    	msg->ExtendedFlag = 0;
    	/* mask ID to guarantee 11bit range */
    	msg->Id &= 0x000007FF;
    }
    
    /* extract DLC */
    msg->DataLength = tbufdata[4] & 0x0F;
    if (msg->DataLength > 8) msg->DataLength = 8;	/* saturate DLC to guarantee 0-8 range */
    
    /* check RTR flag */
    if (tbufdata[4] & 0x40) {
    	msg->RemoteFlag = 1;
    } else {
        msg->RemoteFlag = 0;
    }
    
    /* copy data */
    for (uint8_t i=0; i<msg->DataLength; i++) {
    	msg->Data.bytes[i] = tbufdata[5+i];
    }
}



void MCP2515_WriteCanId(const uint8_t mcp_addr, const uint8_t ext, const uint32_t can_id) {
    uint16_t canid;
    uint8_t tbufdata[4];
	canid = (uint16_t)(can_id & 0x0FFFF);
    
	if (ext == 1) {
        tbufdata[3] = (uint8_t) (canid & 0xFF);
        tbufdata[2] = (uint8_t) (canid / 256);
        canid = (uint16_t)( can_id / 0x10000L );
        tbufdata[1] = (uint8_t) (canid & 0x03);
        tbufdata[1] += (uint8_t) ((canid & 0x1C )*8);
        tbufdata[1] |= MCP_TXB_EXIDE_M;
        tbufdata[0] = (uint8_t) (canid / 32 );
    }
    else {
        tbufdata[0] = (uint8_t) (canid / 8 );
        tbufdata[1] = (uint8_t) ((canid & 0x07 )*32);
        tbufdata[2] = 0;
        tbufdata[3] = 0;
    }
	MCP2515_SetRegisterS(mcp_addr, tbufdata, 4);
}

// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void MCP2515_WriteCanMsg( const uint8_t buffer_sidh_addr, const Can_Message_t* msg) {
    uint8_t mcp_addr, dlc;
	mcp_addr = buffer_sidh_addr;
	dlc = msg->DataLength;
	MCP2515_SetRegisterS(mcp_addr+5, &(msg->Data.bytes[0]), dlc);  // write data bytes
    MCP2515_WriteCanId(mcp_addr, msg->ExtendedFlag, msg->Id);  // write CAN id
    if (msg->RemoteFlag == 1)  dlc |= MCP_RTR_MASK;  // if RTR set bit in byte
    MCP2515_SetRegister((mcp_addr+4), dlc);  // write the RTR and DLC
}

/*
 ** Start the transmission from one of the tx buffers.
 */
// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void MCP2515_StartTransmit(const uint8_t buffer_sidh_addr) {
	// TXBnCTRL_addr = TXBnSIDH_addr - 1
    MCP2515_ModifyRegister( buffer_sidh_addr-1 , MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M );
}

uint8_t MCP2515_GetNextFreeTXBuf(uint8_t *txbuf_n) {
	uint8_t res, i, ctrlval;
	uint8_t ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };
	
	res = MCP_ALLTXBUSY;
	*txbuf_n = 0x00;
	
	// check all 3 TX-Buffers
	for (i=0; i<MCP_N_TXBUFFERS; i++) {
		ctrlval = MCP2515_ReadRegister( ctrlregs[i] );
		if ((ctrlval & MCP_TXB_TXREQ_M) == 0) {
#ifdef MCP_DEBUGMODE
			printf("Selected TX-Buffer: %u\n", i+1);
#endif
			*txbuf_n = ctrlregs[i]+1; // return SIDH-address of Buffer
			res = MCP2515_OK;
			return res; /* ! function exit */
		}
	}
	return res;
}

void MCP2515_InitCanBuffers(void) {
	uint8_t i, a1, a2, a3;
	
	// TODO: check why this is needed to receive extended 
	//   and standard frames
	// Mark all filter bits as don't care:
    MCP2515_WriteCanId(MCP_RXM0SIDH, 0, 0);
    MCP2515_WriteCanId(MCP_RXM1SIDH, 0, 0);
    // Anyway, set all filters to 0:
    MCP2515_WriteCanId(MCP_RXF0SIDH, 1, 0); // RXB0: extended 
    MCP2515_WriteCanId(MCP_RXF1SIDH, 0, 0); //       AND standard
    MCP2515_WriteCanId(MCP_RXF2SIDH, 1, 0); // RXB1: extended 
    MCP2515_WriteCanId(MCP_RXF3SIDH, 0, 0); //       AND standard
    MCP2515_WriteCanId(MCP_RXF4SIDH, 0, 0);
    MCP2515_WriteCanId(MCP_RXF5SIDH, 0, 0);
	
	// Clear, deactivate the three transmit buffers
	// TXBnCTRL -> TXBnD7
    a1 = MCP_TXB0CTRL;
	a2 = MCP_TXB1CTRL;
	a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++) { // in-buffer loop
		MCP2515_SetRegister(a1, 0);
		MCP2515_SetRegister(a2, 0);
		MCP2515_SetRegister(a3, 0);
        a1++;
		a2++;
		a3++;
    }
	
    // and clear, deactivate the two receive buffers.
    MCP2515_SetRegister(MCP_RXB0CTRL, 0);
    MCP2515_SetRegister(MCP_RXB1CTRL, 0);
}


// ---

uint8_t MCP2515_Init(const uint8_t canSpeed) {
	uint8_t res;
	
	SPI_Init();		// init SPI-Interface (as "Master")
	
	MCP2515_UNSELECT();
	MCP_CS_DDR |= ( 1 << MCP_CS_BIT );
	
	MCP2515_Reset();
	
	res = MCP2515_SetCanCtrlMode(MODE_CONFIG);
	
	if (res == MCP2515_FAIL) return res;  /* function exit on error */

	res = MCP2515_SetClkout(CLKOUT_PS1);
	
	if (res == MCP2515_FAIL) return res;  /* function exit on error */
	
	res = MCP2515_ConfigRate(canSpeed);
	
	if (res == MCP2515_OK) {
		MCP2515_InitCanBuffers();

#if (DEBUG_RXANY==1)
	#warning DEBUG_RXANY is defined! Bypassing filter.
		// enable both receive-buffers to receive any message
		// and enable rollover
		MCP2515_ModifyRegister(
				MCP_RXB0CTRL,
				MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
				MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
		MCP2515_ModifyRegister(
				MCP_RXB1CTRL,
				MCP_RXB_RX_MASK,
				MCP_RXB_RX_ANY);
#else
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		MCP2515_ModifyRegister(
				MCP_RXB0CTRL,
				MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, 
				MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
		MCP2515_ModifyRegister(
				MCP_RXB1CTRL,
				MCP_RXB_RX_MASK,
				MCP_RXB_RX_STDEXT);
#endif
	}
	
	return res;
}

#ifdef MCP_DEBUGMODE
void MCP2515_DumpExtendedStatus(void) {
	uint8_t tec, rec, eflg;
	
	tec  = MCP2515_ReadRegister(MCP_TEC);
	rec  = MCP2515_ReadRegister(MCP_REC);
	eflg = MCP2515_ReadRegister(MCP_EFLG);
	
	printf("MCP2515 Extended Status:\n");
	printf("MCP Transmit Error Count: %u\n", tec);
	printf("MCP Receiver Error Count: %u\n", rec);
	printf("MCP Error Flag: %u\n", eflg);
	
	if ( (rec>127) || (tec>127) ) {
		printf("Error-Passive or Bus-Off\n");
	}

	if (eflg & MCP_EFLG_RX1OVR) 
		printf("Receive Buffer 1 Overflow\n");
	if (eflg & MCP_EFLG_RX0OVR) 
		printf("Receive Buffer 0 Overflow\n");
	if (eflg & MCP_EFLG_TXBO) 
		printf("Bus-Off\n");
	if (eflg & MCP_EFLG_TXEP) 
		printf("Receive Error Passive\n");
	if (eflg & MCP_EFLG_TXWAR) 
		printf("Transmit Error Warning\n");
	if (eflg & MCP_EFLG_RXWAR) 
		printf("Receive Error Warning\n");
	if (eflg & MCP_EFLG_EWARN ) 
		printf("Receive Error Warning\n");
}
#endif

#if 0 
// read-modify-write - better: Bit Modify Instruction
uint8_t MCP2515_SetCanCtrlMode(uint8_t newmode) {
	uint8_t i;
	
	i = MCP2515_ReadRegister(MCP_CANCTRL);
	i &= ~(MODE_MASK);
	i |= newmode;
	MCP2515_SetRegister(MCP_CANCTRL, i);
	
	// verify as advised in datasheet
	i = MCP2515_ReadRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}
#endif
