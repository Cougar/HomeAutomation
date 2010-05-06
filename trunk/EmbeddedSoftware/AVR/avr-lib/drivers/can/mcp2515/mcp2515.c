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
//#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <drivers/can/mcp2515/mcp2515.h>
#include <drivers/can/mcp2515/mcp2515_defs.h>
#include <drivers/can/mcp2515/mcp2515_bittime.h>

/* Hack to fix lack of PBx, PCx defines in /usr/lib/avr/include/avr/portpins.h */
/* http://savannah.nongnu.org/bugs/?25930 http://www.mail-archive.com/avr-libc-dev@nongnu.org/msg03306.html */
#include <drivers/mcu/portpins.h>

#include <vectors.h>

#ifdef MCP_USART_SPI_MODE
#define SPI_PORT	PORTD
#define SPI_PIN		PIND
#define SPI_DDR		DDRD
#else 
#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_DDR		DDRB
#endif

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
	#ifdef MCP_USART_SPI_MODE
		#define SPI_SCK   PD4
		#define SPI_MISO  PD0
		#define SPI_MOSI  PD1
	#else 
		#define SPI_SCK   PB5
		#define SPI_MISO  PB4
		#define SPI_MOSI  PB3
	#endif
#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
	#define SPI_SCK   PB7
	#define SPI_MISO  PB6 
	#define SPI_MOSI  PB5  
#else
	#error "SPI pins undefined for this target (see spi.h)"
#endif

static void SPI_Init(void);
static uint8_t SPI_ReadWrite(uint8_t data);
static uint8_t SPI_Read(void);

#define SPI_DONTCARE (0x00)

#ifdef MCP_USART_SPI_MODE
static void SPI_Init(void) {
	//cli();
	UBRR0 = 0;
	SPI_DDR |= (1<<SPI_SCK) | (1<<SPI_MOSI); // xck (sck) output
	SPI_DDR &= ~(1<<SPI_MISO);
//	TXD_DDR |= (1<<TXD); // txd (mosi) output
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0);		//select MSPIM, SPI-mode 0
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);									//enable receiver and transmitter
	UBRR0 = 0;														//set "baudrate" to maximum ( BAUD=Fosc/2(UBRR0+1) )
	//sei();
}

static uint8_t SPI_ReadWrite(uint8_t data) {
	//while ( !( UCSR0A & (1<<UDRE0)) );
	//uint8_t dummy = UDR0;
	UDR0 = data;
	while(!(UCSR0A&(1<<RXC0)));
	
	return UDR0;
}
#else
// init as SPI-Master
static void SPI_Init(void) {
	// SCK, SS!!, MOSI as outputs
	SPI_DDR |= (1<<SPI_SCK) | (1<<SPI_MOSI);
	// MISO as input
	SPI_DDR &= ~(1<<SPI_MISO);
	// INIT interface, Master, set clock rate fck/4
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);
	// enable double rate
	SPSR = (1<<SPI2X); // we will now gain fck/2 instead of fck/4
}

static uint8_t SPI_ReadWrite(uint8_t data) {
	// set data to send into SPI data register
	SPDR = data;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// return data read from SPI (if any)
	return SPDR;
}
#endif

static uint8_t SPI_Read(void) {
	return SPI_ReadWrite(SPI_DONTCARE);
}

static Can_Message_t msgbuf;

static void MCP2515_Reset(void) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_RESET);
	MCP2515_UNSELECT();
	// delay at least 128 MCP clock cycles
	volatile int16_t i;
	for (i=0; i<32000; i++) {
		if (i < 0) break;
	}
}

static uint8_t MCP2515_ReadRegister(const uint8_t address) {
	uint8_t ret;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ);
	SPI_ReadWrite(address);
	ret = SPI_Read();
	MCP2515_UNSELECT();
	
	return ret;
}

/*
static void MCP2515_ReadRXBuf(const uint8_t buf, uint8_t* data) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ_RX0 + ((buf & 0x1) << 2));
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<13; i++) {
		data[i] = SPI_Read();
	}
	MCP2515_UNSELECT();
}*/

static void MCP2515_SetRegister(const uint8_t address, const uint8_t value) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(address);
	SPI_ReadWrite(value);
	MCP2515_UNSELECT();
}

/*
static void MCP2515_SetRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(address);
	// mcp2515 has auto-increment of address-pointer
	for (i=0; i<n; i++) {
		SPI_ReadWrite(values[i]);
	}
	MCP2515_UNSELECT();
}*/

static void MCP2515_ModifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_BITMOD);
	SPI_ReadWrite(address);
	SPI_ReadWrite(mask);
	SPI_ReadWrite(data);
	MCP2515_UNSELECT();
}

static uint8_t MCP2515_ReadStatus(void) {
	uint8_t i;
	
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ_STATUS);
	i = SPI_Read();
	MCP2515_UNSELECT();
	
	return i;
}
	
/*
static uint8_t MCP2515_ReadStatus(void) {
	return MCP2515_ReadXXStatus_Helper(MCP_READ_STATUS);
}*/

/*
static uint8_t MCP2515_RxStatus(void) {
	return MCP2515_ReadXXStatus_Helper(MCP_RX_STATUS);
}*/

static uint8_t MCP2515_SetCanCtrl(const uint8_t newmask, const uint8_t newmode) {
	uint8_t i;
	MCP2515_ModifyRegister(MCP_CANCTRL, newmask, newmode);
	// verify as advised in datasheet
	i = MCP2515_ReadRegister(MCP_CANCTRL);
	i &= newmask;
	if ( i == newmode ) {
		return MCP2515_OK; 
	}
	else {
		return MCP2515_FAIL;
	}
}

/*
static uint8_t MCP2515_SetClkout(const uint8_t newmode) {
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
}*/

static inline void MCP2515_ConfigRate(void) {
	MCP2515_SetRegister(MCP_CNF1, MCP_BITRATE_CFG1);
	MCP2515_SetRegister(MCP_CNF2, MCP_BITRATE_CFG2);
	MCP2515_SetRegister(MCP_CNF3, MCP_BITRATE_CFG3);
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
static void MCP2515_ReadCanMsg(const uint8_t buffer, Can_Message_t* msg) {
	uint8_t id[4];
	uint8_t i;

	MCP2515_SELECT();
	SPI_ReadWrite(MCP_READ_RX0 + ((buffer & 0x1) << 2));
	// mcp2515 has auto-increment of address-pointer
	
	// read ID
	for (i=0; i<4; i++) {
		id[i] = SPI_Read();
	}
	
	// extract SID10-SID0
	uint16_t stdid = (((uint16_t)(id[0])) << 3) | (((uint16_t)(id[1] & 0xE0)) >> 5);
	
	if (id[1] & MCP_TXB_EXIDE_M) {
		// extended message
		msg->ExtendedFlag = 1;
		// SID10-SID0 are still most significant, so shift them up
		// and make room for 18 extended least significant EID-bits
		msg->Id = ((uint32_t)stdid) << 18;
		// exctract EID17-EID16
		msg->Id |= (((uint32_t)(id[1] & 0x03))<<16);
		// extract EID15-EID8
		msg->Id |= (((uint32_t)id[2])<<8);
		// extract EID7-EID0
		msg->Id |= ((uint32_t)id[3]);
    }
    else {
    	// standard message
    	msg->ExtendedFlag = 0;
    	msg->Id = stdid;
    }
    
    // extract DLC
	uint8_t rtrdlc = SPI_Read();
    msg->DataLength = rtrdlc & 0x0F;
    if (msg->DataLength > 8) msg->DataLength = 8;	// saturate DLC to guarantee 0-8 range
    
    // check RTR flag
    msg->RemoteFlag = (rtrdlc & 0x40) > 0;
    
    // read data
    for (i=0; i<msg->DataLength; i++) {
    	msg->Data.bytes[i] = SPI_Read();
    }
	MCP2515_UNSELECT();
}


static void MCP2515_SPIWriteCanId(const uint8_t ext, const uint32_t can_id) {
    uint16_t canid = can_id & 0xFFFF;
    uint8_t tbufdata[4];
	uint8_t i;
    
	if (ext) {
        tbufdata[3] = canid & 0xFF;
        tbufdata[2] = canid >> 8;
        canid = can_id >> 16;
        tbufdata[1] = canid & 0x03;
        tbufdata[1] += (canid & 0x1C ) << 3;
        tbufdata[1] |= MCP_TXB_EXIDE_M;
        tbufdata[0] = canid >> 5;
    }
    else {
        tbufdata[0] = canid >> 3;
        tbufdata[1] = (canid & 0x07 ) << 5;
        tbufdata[2] = 0;
        tbufdata[3] = 0;
    }
    for (i=0; i<4; i++) {
    	SPI_ReadWrite(tbufdata[i]);
    }
}


static void MCP2515_WriteCanId(const uint8_t mcp_addr, const uint8_t ext, const uint32_t can_id) {
	MCP2515_SELECT();
	SPI_ReadWrite(MCP_WRITE);
	SPI_ReadWrite(mcp_addr);
	MCP2515_SPIWriteCanId(ext, can_id);
	MCP2515_UNSELECT();
}

// Buffer can be 0, 1 or 2
static void MCP2515_WriteCanMsg( const uint8_t buffer, const Can_Message_t* msg) {
	uint8_t rtrdlc = msg->DataLength;
	uint8_t i;
	
	MCP_INT_DISABLE();
    MCP2515_SELECT();
    SPI_ReadWrite(MCP_LOAD_TX0 | (buffer << 1));
    MCP2515_SPIWriteCanId(msg->ExtendedFlag, msg->Id);
    if (msg->RemoteFlag == 1)  rtrdlc |= MCP_RTR_MASK;  // if RTR set bit in byte
    SPI_ReadWrite(rtrdlc);
    //MCP2515_UNSELECT();
    //MCP_INT_ENABLE();

	//MCP_INT_DISABLE();
   	//MCP2515_SELECT();
    for (i=0; i<msg->DataLength; i++) {
    	SPI_ReadWrite(msg->Data.bytes[i]);
    }
   	MCP2515_UNSELECT();
    MCP_INT_ENABLE();
}


// Start the transmission from one of the tx buffers.
//
// Buffer can be 0, 1 or 2
static void MCP2515_StartTransmit(const uint8_t buffer) {
	MCP_INT_DISABLE();
    MCP2515_SELECT();
    SPI_ReadWrite(MCP_RTS_TX | (1 << buffer));
    MCP2515_UNSELECT();
    MCP_INT_ENABLE();
}

static uint8_t MCP2515_GetNextFreeTXBuf(void) {
	uint8_t stat;
	
	// check all 3 TX-Buffers
	MCP_INT_DISABLE();
	stat = MCP2515_ReadStatus();
    MCP_INT_ENABLE();
	if (!(stat & MCP_STAT_TX0REQ)) return 0;
	if (!(stat & MCP_STAT_TX1REQ)) return 1;
	if (!(stat & MCP_STAT_TX2REQ)) return 2;
	return MCP_ALLTXBUSY;
}

static void MCP2515_InitCanBuffers(void) {
	uint8_t i;
	
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
    for (i = 0; i < 14; i++) { // in-buffer loop
		MCP2515_SetRegister(MCP_TXB0CTRL + i, 0);
		MCP2515_SetRegister(MCP_TXB1CTRL + i, 0);
		MCP2515_SetRegister(MCP_TXB2CTRL + i, 0);
    }
	
    // and clear, deactivate the two receive buffers.
    MCP2515_SetRegister(MCP_RXB0CTRL, 0);
    MCP2515_SetRegister(MCP_RXB1CTRL, 0);
}


static void MCP2515_InitRXInterrupts(void) {
	MCP2515_SetRegister(MCP_CANINTF, 0);
	MCP2515_SetRegister(MCP_CANINTE, MCP_RX_INT);
	MCP_INT_ENABLE();
}

// ---

static uint8_t MCP2515_Init(void) {
	uint8_t res;
	
#ifndef MCP_USART_SPI_MODE
	#if MCP_CS_BIT != SS
		/* If slave select is not set as output it might change SPI hw to slave
		 * See ch 18.3.2 (18.3 SS Pin Functionality) in ATmega48/88/168-datasheet */
		DDRSS |= (1<<SS);
	#endif
#endif

	SPI_Init();		// init SPI-Interface (as "Master")
	
	MCP2515_UNSELECT();
#ifdef MCP_USART_SPI_MODE
	MCP_CS_DDR_USART |= ( 1 << MCP_CS_BIT_USART );
#else
	MCP_CS_DDR |= ( 1 << MCP_CS_BIT );
#endif
	
	MCP2515_Reset();
	
	res = MCP2515_SetCanCtrl(MODE_MASK, MODE_CONFIG);
	
	if (res == MCP2515_FAIL) return res;  /* function exit on error */

	/* Set prescaler for clock output */
	res = MCP2515_SetCanCtrl(CLK_MASK, MCP_PRESCALER);
	
	if (res == MCP2515_FAIL) return res;  /* function exit on error */
	
	MCP2515_ConfigRate();
	
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

	MCP2515_InitRXInterrupts();
	
	return res;
}

Can_Return_t Can_Receive(Can_Message_t *msg);

ISR(MCP_INT_VECTOR) {
	// Get first available message from controller and pass it to
	// application handler. If both RX buffers contain messages
	// we will get another interrupt as soon as this one returns.
	if (Can_Receive(&msgbuf) == CAN_OK) {
		// Callbacks are run with global interrupts disabled but
		// with controller flag cleared so another msg can be
		// received while this one is processed.
#if MCP_CAN_PROCESS_RENAMED == 1
		// when running a gateway application which uses stdcan
		Can_Process_USART(&msgbuf);
#else
		Can_Process(&msgbuf);
#endif
	}
}

/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the CAN interface. Edit can_cfg.h to choose bitrate.
 * 
 * @return
 *		CAN_OK if initialization was successful.
 * 		CAN_INIT_FAIL_SET_BITRATE if bitrate could not be set correctly.
 * 		CAN_INIT_FAIL_SET_MODE if the controller could not be set to normal operation mode.
 * 		CAN_INIT_FAIL in case of general error.
 */
Can_Return_t Can_Init(void) {
		/*
		 * Initialize MCP2515 device.
		 */
		if (MCP2515_Init() != MCP2515_OK) {
			return CAN_FAIL;
		}
		if (MCP2515_SetCanCtrl(MODE_MASK, MODE_NORMAL) != MCP2515_OK) {
			return CAN_FAIL;
		}
		return CAN_OK;
	
	return CAN_FAIL;
}

/**
 * Sends a CAN message immediately with the controller hardware. If the CAN
 * controller is busy, the function will return CAN_FAIL.
 *
 * @param msg
 *		Pointer to the CAN message storage buffer.
 * 
 * @return
 *		CAN_OK if the message was successfully sent to the controller.
 *		CAN_FAIL if the controller is busy.
 */
Can_Return_t Can_Send(Can_Message_t *msg) {
		/*
		 * Send with MCP2515 device.
		 */
		uint8_t txbuf_n;
		txbuf_n = MCP2515_GetNextFreeTXBuf();
		if (txbuf_n == MCP_ALLTXBUSY) {
			// TODO: Maybe we should block until a buffer is available?
			return CAN_FAIL;
		}
		MCP2515_WriteCanMsg(txbuf_n, msg);
		MCP2515_StartTransmit(txbuf_n);
		return CAN_OK;
}


/**
 * Receives a CAN message from the CAN controller hardware.
 * 
 * @param msg
 *		Pointer to the message storage buffer into which the message should be copied.
 *
 * @return
 *		CAN_OK if a received message was successfully copied into the buffer.
 *		CAN_NO_MSG_AVAILABLE if there is no message available in the controller.
 */
Can_Return_t Can_Receive(Can_Message_t *msg) {
	/*
	 * Receive from MCP2515 device.
	 */
	uint8_t stat;
	static uint8_t buffer = 0;
	
	stat = MCP2515_ReadStatus() & MCP_STAT_RXIF_MASK;
	
	if (stat > 2) {
		// Both buffers contain data, read the one previously not read.
		// (Bit 0 in buffer selects which buffer to read, no need to take modulus 2)
		MCP2515_ReadCanMsg(buffer++, msg);
	} else if (stat > 0) {
		// One of the buffers contain data, read it
		MCP2515_ReadCanMsg(stat-1, msg);
	} else {
		// Nothing available
		return CAN_NO_MSG_AVAILABLE;
	}
	return CAN_OK;
}
