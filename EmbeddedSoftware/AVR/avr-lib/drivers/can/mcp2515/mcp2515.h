#ifndef MCP2515_H_
#define MCP2515_H_

#include <drivers/can/mcp2515/mcp2515_defs.h>
#include <drivers/can/mcp2515/mcp2515_cfg.h>
#include <drivers/can/can.h>

#define MCP_N_TXBUFFERS (3)

#define MCP_RXBUF_0 (MCP_RXB0SIDH)
#define MCP_RXBUF_1 (MCP_RXB1SIDH)

#define MCP_TXBUF_0 (MCP_TXB0SIDH)
#define MCP_TXBUF_1 (MCP_TXB1SIDH)
#define MCP_TXBUF_2 (MCP_TXB2SIDH)

// #define MCP2515_SELECT() (SPI_SS_LOW())
// #define MCP2515_UNSELECT() (SPI_SS_HIGH())

#define MCP2515_SELECT()   ( MCP_CS_PORT &= ~(1<<MCP_CS_BIT) )
#define MCP2515_UNSELECT() ( MCP_CS_PORT |=  (1<<MCP_CS_BIT) )

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)
#define MCP_ALLTXBUSY      (255)

#if 0
void MCP2515_Reset(void);

uint8_t MCP2515_ReadRegister(const uint8_t address);
void MCP2515_SetRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n);

void MCP2515_SetRegister(const uint8_t address, const uint8_t value);
void MCP2515_SetRegisterS(const uint8_t address, const uint8_t values[], const uint8_t n);
void MCP2515_ModifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data);

uint8_t MCP2515_ReadStatus(void);
uint8_t MCP2515_RxStatus(void);

uint8_t MCP2515_SetCanCtrlMode(const uint8_t newmode);
uint8_t MCP2515_SetClkout(const uint8_t newmode);

inline void MCP2515_ConfigRate(void);
uint8_t MCP2515_Init(void);

void MCP2515_WriteCanId(const uint8_t mcp_addr, const uint8_t ext, const uint32_t can_id);
void MCP2515_ReadCanId(const uint8_t mcp_addr, uint8_t* ext, uint32_t* can_id );
void MCP2515_WriteCanMsg(const uint8_t buffer_sidh_addr, const Can_Message_t* msg);
void MCP2515_ReadCanMsg(const uint8_t buffer_sidh_addr, Can_Message_t* msg);

void MCP2515_StartTransmit(const uint8_t mcp_addr);

uint8_t MCP2515_GetNextFreeTXBuf(uint8_t *txbuf_n);

#ifdef MCP_DEBUGMODE
void MCP2515_DumpExtendedStatus(void);
#endif
#endif

Can_Return_t Can_Init(void);
Can_Return_t Can_Send(Can_Message_t* msg);
void Can_Process(Can_Message_t* msg); // Callback from ISR to application code 
//Can_Return_t Can_Receive(Can_Message_t *msg);

#endif
