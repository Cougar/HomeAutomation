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

#if defined(__AVR_ATmega8__)
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

void spi_init(void);
uint8_t spi_readwrite(uint8_t data);
uint8_t spi_read(void);

#define SPI_DONTCARE (0x00)

/* init as SPI-Master */
void spi_init(void) {
	/* SCK, SS!!, MOSI as outputs */
	SPI_DDR |= (1<<SPI_SCK) | (1<<SPI_SS) | (1<<SPI_MOSI);
	/* MISO as input */
	SPI_DDR &= ~(1<<SPI_MISO);
	/* INIT interface, Master, set clock rate fck/128 TODO: check prescaler */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
}

uint8_t spi_readwrite(uint8_t data) {
	/* set data to send into SPI data register */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	/* return data read from SPI (if any) */
	return SPDR;
}

uint8_t spi_read(void) {
	return spi_readwrite(SPI_DONTCARE);
}


void mcp2515_reset(void) {
	MCP2515_SELECT();
	spi_readwrite(MCP_RESET);
	MCP2515_UNSELECT();
	/* delay at least 128 MCP clock cycles */
	volatile int16_t i;
	for (i=0; i<32000; i++) {
		if (i < 0) break;
	}
}

uint8_t mcp2515_readRegister(const uint8_t address) {
	uint8_t ret;
	
	MCP2515_SELECT();
	spi_readwrite(MCP_READ);
	spi_readwrite(address);
	ret = spi_read();
	MCP2515_UNSELECT();
	
	return ret;
}

void mcp2515_readRegisterS(const uint8_t address, uint8_t values[], const uint8_t n) {
	uint8_t i;
	
	MCP2515_SELECT();
	spi_readwrite(MCP_READ);
	spi_readwrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		values[i] = spi_read();
	}
	MCP2515_UNSELECT();
}

void mcp2515_setRegister(const uint8_t address, const uint8_t value) {
	MCP2515_SELECT();
	spi_readwrite(MCP_WRITE);
	spi_readwrite(address);
	spi_readwrite(value);
	MCP2515_UNSELECT();
}

void mcp2515_setRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n) {
	uint8_t i;
	
	MCP2515_SELECT();
	spi_readwrite(MCP_WRITE);
	spi_readwrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		spi_readwrite(values[i]);
	}
	MCP2515_UNSELECT();
}

void mcp2515_modifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data) {
	MCP2515_SELECT();
	spi_readwrite(MCP_BITMOD);
	spi_readwrite(address);
	spi_readwrite(mask);
	spi_readwrite(data);
	MCP2515_UNSELECT();
}

static uint8_t mcp2515_readXXStatus_helper(const uint8_t cmd) {
	uint8_t i;
	
	MCP2515_SELECT();
	spi_readwrite(cmd);
	i = spi_read();
	MCP2515_UNSELECT();
	
	return i;
}
	
uint8_t mcp2515_readStatus(void) {
	return mcp2515_readXXStatus_helper(MCP_READ_STATUS);
}

uint8_t mcp2515_RXStatus(void) {
	return mcp2515_readXXStatus_helper(MCP_RX_STATUS);
}

uint8_t mcp2515_setCANCTRL_Mode(const uint8_t newmode) {
	uint8_t i;
	mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);
	// verify as advised in datasheet
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}


uint8_t mcp2515_configRate(const CanBitrate_t canBitrate) {
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
		mcp2515_setRegister(MCP_CNF1, cfg1);
		mcp2515_setRegister(MCP_CNF2, cfg2);
		mcp2515_setRegister(MCP_CNF3, cfg3);
		return MCP2515_OK;
	}
	else {
		return MCP2515_FAIL;
	}
} 

// ---

void mcp2515_read_can_id(const uint8_t mcp_addr, uint8_t* ext, uint32_t* can_id) {
    uint8_t tbufdata[4];
	
    *ext = 0;
    *can_id = 0;
    
	mcp2515_readRegisterS( mcp_addr, tbufdata, 4 );
    
	*can_id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);
	
    if ((tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M) {
		// extended id
        *can_id = (*can_id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *can_id <<= 16;
        *can_id = *can_id +(tbufdata[MCP_EID8]<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

// Buffer can be MCP_RXBUF_0 or MCP_RXBUF_1
void mcp2515_read_canMsg(const uint8_t buffer_sidh_addr, CanMessage_t* msg) {
	uint8_t mcp_addr, ctrl;
	mcp_addr = buffer_sidh_addr;
	mcp2515_read_can_id(mcp_addr, &(msg->ExtendedFlag), &(msg->Id));
	
	ctrl = mcp2515_readRegister(mcp_addr-1);
    msg->DataLength = mcp2515_readRegister(mcp_addr+4);
    
    if (/*(*dlc & RTR_MASK) || */(ctrl & 0x08)) {
    	msg->RemoteFlag = 1;
    } else {
        msg->RemoteFlag = 0;
    }
    
    msg->DataLength &= MCP_DLC_MASK;
    mcp2515_readRegisterS(mcp_addr+5, &(msg->Data.bytes[0]), msg->DataLength);
}


void mcp2515_write_can_id(const uint8_t mcp_addr, const uint8_t ext, const uint32_t can_id) {
    uint16_t canid;
    uint8_t tbufdata[4];
	canid = (uint16_t)(can_id & 0x0FFFF);
    
	if (ext == 1) {
        tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
        tbufdata[MCP_EID8] = (uint8_t) (canid / 256);
        canid = (uint16_t)( can_id / 0x10000L );
        tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
        tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C )*8);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (uint8_t) (canid / 32 );
    }
    else {
        tbufdata[MCP_SIDH] = (uint8_t) (canid / 8 );
        tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07 )*32);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }
	mcp2515_setRegisterS(mcp_addr, tbufdata, 4);
}

// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_write_canMsg( const uint8_t buffer_sidh_addr, const CanMessage_t* msg) {
    uint8_t mcp_addr, dlc;
	mcp_addr = buffer_sidh_addr;
	dlc = msg->DataLength;
	mcp2515_setRegisterS(mcp_addr+5, &(msg->Data.bytes[0]), dlc);  // write data bytes
    mcp2515_write_can_id(mcp_addr, msg->ExtendedFlag, msg->Id);  // write CAN id
    if (msg->RemoteFlag == 1)  dlc |= MCP_RTR_MASK;  // if RTR set bit in byte
    mcp2515_setRegister((mcp_addr+4), dlc);  // write the RTR and DLC
}

/*
 ** Start the transmission from one of the tx buffers.
 */
// Buffer can be MCP_TXBUF_0 MCP_TXBUF_1 or MCP_TXBUF_2
void mcp2515_start_transmit(const uint8_t buffer_sidh_addr) {
	// TXBnCTRL_addr = TXBnSIDH_addr - 1
    mcp2515_modifyRegister( buffer_sidh_addr-1 , MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M );
}

uint8_t mcp2515_getNextFreeTXBuf(uint8_t *txbuf_n) {
	uint8_t res, i, ctrlval;
	uint8_t ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };
	
	res = MCP_ALLTXBUSY;
	*txbuf_n = 0x00;
	
	// check all 3 TX-Buffers
	for (i=0; i<MCP_N_TXBUFFERS; i++) {
		ctrlval = mcp2515_readRegister( ctrlregs[i] );
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

void mcp2515_initCANBuffers(void) {
	uint8_t i, a1, a2, a3;
	
	// TODO: check why this is needed to receive extended 
	//   and standard frames
	// Mark all filter bits as don't care:
    mcp2515_write_can_id(MCP_RXM0SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXM1SIDH, 0, 0);
    // Anyway, set all filters to 0:
    mcp2515_write_can_id(MCP_RXF0SIDH, 1, 0); // RXB0: extended 
    mcp2515_write_can_id(MCP_RXF1SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF2SIDH, 1, 0); // RXB1: extended 
    mcp2515_write_can_id(MCP_RXF3SIDH, 0, 0); //       AND standard
    mcp2515_write_can_id(MCP_RXF4SIDH, 0, 0);
    mcp2515_write_can_id(MCP_RXF5SIDH, 0, 0);
	
	// Clear, deactivate the three transmit buffers
	// TXBnCTRL -> TXBnD7
    a1 = MCP_TXB0CTRL;
	a2 = MCP_TXB1CTRL;
	a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++) { // in-buffer loop
		mcp2515_setRegister(a1, 0);
		mcp2515_setRegister(a2, 0);
		mcp2515_setRegister(a3, 0);
        a1++;
		a2++;
		a3++;
    }
	
    // and clear, deactivate the two receive buffers.
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}


// ---

uint8_t mcp2515_init(const uint8_t canSpeed) {
	uint8_t res;
	
	spi_init();		// init SPI-Interface (as "Master")
	
	MCP2515_UNSELECT();
	MCP_CS_DDR |= ( 1 << MCP_CS_BIT );
	
	mcp2515_reset();
	
	res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
	
	if (res == MCP2515_FAIL) return res;  /* function exit on error */
	
	res = mcp2515_configRate(canSpeed);
	
	if (res == MCP2515_OK) {
		mcp2515_initCANBuffers();

#if (DEBUG_RXANY==1)
	#warning DEBUG_RXANY is defined! Bypassing filter.
		// enable both receive-buffers to receive any message
		// and enable rollover
		mcp2515_modifyRegister(
				MCP_RXB0CTRL,
				MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
				MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
		mcp2515_modifyRegister(
				MCP_RXB1CTRL,
				MCP_RXB_RX_MASK,
				MCP_RXB_RX_ANY);
#else
		// enable both receive-buffers to receive messages
		// with std. and ext. identifiers
		// and enable rollover
		mcp2515_modifyRegister(
				MCP_RXB0CTRL,
				MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, 
				MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
		mcp2515_modifyRegister(
				MCP_RXB1CTRL,
				MCP_RXB_RX_MASK,
				MCP_RXB_RX_STDEXT);
#endif
	}
	
	return res;
}

#ifdef MCP_DEBUGMODE
void mcp2515_dumpExtendedStatus(void) {
	uint8_t tec, rec, eflg;
	
	tec  = mcp2515_readRegister(MCP_TEC);
	rec  = mcp2515_readRegister(MCP_REC);
	eflg = mcp2515_readRegister(MCP_EFLG);
	
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
uint8_t mcp2515_setCANCTRL_Mode(uint8_t newmode) {
	uint8_t i;
	
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= ~(MODE_MASK);
	i |= newmode;
	mcp2515_setRegister(MCP_CANCTRL, i);
	
	// verify as advised in datasheet
	i = mcp2515_readRegister(MCP_CANCTRL);
	i &= MODE_MASK;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}
#endif
