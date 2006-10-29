/**
 * @file	mcp2510.c
 * Low level drivers for the MCP2510 CAN controller. Target MCU is the ATmega8.
 * 
 * @author	Jimmy Myhrman (jimmy@myhrman.org)
 * @date	2005-11-28
 */

/* -----------------------------------------------------------------------------
 * Includes
 * ---------------------------------------------------------------------------*/
#include "mcp2510.h"
#include "spi.h"

/* -----------------------------------------------------------------------------
 * Functions
 * ---------------------------------------------------------------------------*/

/**
 * Initializes and resets the MCP controller.
 */
void MCP_init() {
	SPI_master_init();
	SPI_chip_select();
	SPI_master_send(MCP_OP_RESET);	/* reset instruction */
	SPI_chip_unselect();
	volatile uint8_t counter;
	for (counter=0; counter<128; counter++) {
		/* wait a few cycles for reset to complete */
		__asm("nop");
	}
}


/**
 * Writes a byte to an address in the MCP. The function waits till the byte
 * has been written before it returns.
 * 
 * @param value The data byte.
 * @param address The address to which the data should be written in the MCP.
 */
void MCP_write(uint8_t value, uint8_t address) {
	SPI_chip_select();
	SPI_master_send(MCP_OP_WRITE);	/* write command */
	SPI_master_send(address);	/* the address */
	SPI_master_send(value);		/* the value */
	SPI_chip_unselect();
}


/**
 * Reads a byte from an address in the MCP.
 * 
 * @param address The address from which the data should be read in the MCP.
 * @return The byte read from the given address in the MCP.
 */
uint8_t MCP_read(uint8_t address) {
	SPI_chip_select();
	SPI_master_send(MCP_OP_READ);	/* read command */
	SPI_master_send(address);	/* the address */
	SPI_master_send(0xFF);		/* any byte (reception will occur simultaneously) */
	uint8_t data = SPDR;		/* get the received byte */
	SPI_chip_unselect();
	return data;
}





void mcp_write_can_id(uint8_t address, uint8_t ext, unsigned long can_id);
void mcp_write_can(uint8_t buffer, uint8_t ext, unsigned long can_id,uint8_t dlc, uint8_t rtr, const uint8_t* data);
void mcp_read_can(uint8_t buffer, uint8_t* ext, unsigned long* can_id,uint8_t* dlc, uint8_t* rtr, uint8_t* data);
void mcp_read_can_id(uint8_t mcp_addr, uint8_t* ext, unsigned long* can_id);
void mcp_write(uint8_t MCPaddr, const uint8_t* writedata, uint8_t length);
void mcp_read(uint8_t MCPaddr, uint8_t* readdata, uint8_t length);


#define SIDH        0
#define SIDL        1
#define EID8        2
#define EID0        3

#define TXB_EXIDE_M     0x08    // In TXBnSIDL
#define DLC_MASK        0x0F
#define RTR_MASK        0x40

/*
 ** Read one or more registers in the MCP2510, starting at address
 ** readdata.
 */
void mcp_read(uint8_t MCPaddr, uint8_t* readdata, uint8_t length) {
    uint8_t loopCnt;
	SPI_chip_select();
	// Start reading and set first address
	SPI_master_send(MCP_OP_READ);
	//SPI_mcp_RD_address(MCPaddr);
	for (loopCnt=0; loopCnt < length; loopCnt++) {
		// Get a byte and store at pointer
        *readdata  = SPI_master_send(MCPaddr);
        // Increment the pointers to next location
        // Test++;
        MCPaddr++;
        readdata++;
    }
    SPI_chip_unselect();
}


/*
 ** Write to one or more registers in the MCP2510, starting at address
 ** writedata.
 */
void mcp_write(uint8_t MCPaddr, const uint8_t* writedata, uint8_t length) {
    uint8_t loopCnt;
    SPI_chip_select();
    // Start write and set first address
    //SPI_mcp_WR_address(MCPaddr);
    SPI_master_send(MCPaddr);
    for (loopCnt=0; loopCnt < length; loopCnt++) {
        // Write a byte
        //SPI_putch( *writedata  );
        SPI_master_send(*writedata);
        // Increment the pointer to next location
        writedata++;
    }
    SPI_chip_unselect();
}


void mcp_read_can_id(uint8_t mcp_addr, uint8_t* ext, unsigned long* can_id) {
    uint8_t tbufdata[4];
    *ext = 0;
    *can_id = 0;
    mcp_read( mcp_addr, tbufdata, 4);
    *can_id = (tbufdata[SIDH]<<3) + (tbufdata[SIDL]>>5);
    if ( (tbufdata[SIDL] & TXB_EXIDE_M) ==  TXB_EXIDE_M ) {
        *can_id = (*can_id<<2) + (tbufdata[SIDL] & 0x03);
        *can_id <<= 16;
        *can_id = *can_id +(tbufdata[EID8]<<8) + tbufdata[EID0];
        *ext = 1;
    }
}


// Buffer can be 4..5
void mcp_read_can(uint8_t buffer, uint8_t* ext, unsigned long* can_id,uint8_t* dlc, uint8_t* rtr, uint8_t* data) {
    uint8_t mcp_addr = buffer*16 + 0x21, ctrl;
    mcp_read_can_id( mcp_addr, ext, can_id );
    mcp_read( mcp_addr-1, &ctrl, 1 );
    mcp_read( mcp_addr+4, dlc, 1 );
    if (/*(*dlc & RTR_MASK) || */(ctrl & 0x08)) {
        *rtr = 1;
    } else {
        *rtr = 0;
    }
    *dlc &= DLC_MASK;
    mcp_read( mcp_addr+5, data, *dlc );
}


void mcp_write_can(uint8_t buffer, uint8_t ext, unsigned long can_id,uint8_t dlc, uint8_t rtr, const uint8_t* data) {
    uint8_t mcp_addr = buffer*16 + 0x21;
    mcp_write(mcp_addr+5, data, dlc );  // write data bytes
    mcp_write_can_id( mcp_addr, ext, can_id );  // write CAN id
    if ( rtr == 1)  dlc |= RTR_MASK;  // if RTR set bit in byte
    mcp_write((mcp_addr+4), &dlc, 1 );            // write the RTR and DLC
}


void mcp_write_can_id(uint8_t address, uint8_t ext, unsigned long can_id) {
    unsigned int canid;
    uint8_t tbufdata[4];
    canid = (unsigned int)(can_id & 0x0FFFF);
    if (ext == 1) {
        tbufdata[EID0] = (uint8_t) (canid & 0xFF);
        tbufdata[EID8] = (uint8_t) (canid / 256);
        canid = (unsigned int)(can_id / 0x10000L);
        tbufdata[SIDL] = (uint8_t) (canid & 0x03);
        tbufdata[SIDL] += (uint8_t) ((canid & 0x1C )*8);
        tbufdata[SIDL] |= TXB_EXIDE_M;
        tbufdata[SIDH] = (uint8_t) (canid / 32 );
    }
    else {
        tbufdata[SIDH] = (uint8_t)(canid / 8 );
        tbufdata[SIDL] = (uint8_t)((canid & 0x07 )*32);
        tbufdata[EID0] = 0;
        tbufdata[EID8] = 0;
    }
    mcp_write(address, tbufdata, 4 );
}