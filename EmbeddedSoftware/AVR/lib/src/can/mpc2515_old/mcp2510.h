#ifndef MCP2510_H_
#define MCP2510_H_

/* -----------------------------------------------------------------------------
 * Includes
 * ---------------------------------------------------------------------------*/
#include <inttypes.h>

/* -----------------------------------------------------------------------------
 * Function prototypes
 * ---------------------------------------------------------------------------*/
void MCP_init(void);
void MCP_write(uint8_t value, uint8_t address);
uint8_t MCP_read(uint8_t address);


/* -----------------------------------------------------------------------------
 * "OP-codes" used when communicating with the MCP. All communication starts
 * with one of these bytes.
 * ---------------------------------------------------------------------------*/
#define MCP_OP_RESET	0xC0	/**< Reset operation. Resets internal registers to default state. */
#define MCP_OP_READ		0x03	/**< Read operation. Reads data from register beginning at selected address. */
#define MCP_OP_WRITE	0x02	/**< Write operation. Writes data to register beginning at selected address. */
#define MCP_OP_BITMOD	0X05	/**< Bit modify operation. Writes data to certain bits in a register. */



/* -----------------------------------------------------------------------------
 * Macros for register operation
 * ---------------------------------------------------------------------------*/

#define MCP_REG_OPEN(reg) {\
		uint8_t __tmpdata = MCP_read(reg);	/* save reg contents */ \
		uint8_t __tmpaddr = reg;			/* save reg address */
		

#define MCP_REG_CLOSE()\
		MCP_write(__tmpdata,__tmpaddr);		/* write back to reg */ \
		}

/**
 * Writes a value to a bitmask in a register. The macro will automatically read
 * the current contents of the register into a temporary variable. The bitfield
 * will then be set in this variable, after which the variable is written back
 * to the register again.
 * 
 * @param reg The register address, for example RXB0CTRL.
 * @param field The bitfield position, for example RXB0CTRL_RXM.
 * @value value The new bitfield value.
 */
#define MCP_WRITE_B(reg,field,value)\
	{ \
		uint8_t __tmpreg = MCP_read(reg); /* read current register contents */ \
		__tmpreg &= ~((_##field##_M)<<(_##field##_B)); /* clear all bits in field */ \
		__tmpreg |= (((value)<<(_##field##_B))&_##field##_M); /* shift in new value */ \
		MCP_write(__tmpreg,reg); /* write back to reg */ \
	}

/**
 * Writes a new value to a register.
 * 
 * @param reg The register address.
 * @param value The new register contents.
 */
#define MCP_WRITE_R(reg,value) MCP_write(value,reg)


/* -----------------------------------------------------------------------------
 * Register definitions (addresses, bitfield positions, bitfield masks).
 * These definitions are used by the macros above. The register numbers are
 * references to the MCP2510 data sheet.
 * ---------------------------------------------------------------------------*/

/* reg 3-1 */
#define TXBnCTRL(n)				(0x30+n*0x10)
#define TXBnCTRL_ABTF
#define _TXBnCTRL_ABTF_B		6
#define _TXBnCTRL_ABTF_M		0x40
#define TXBnCTRL_MLOA
#define _TXBnCTRL_MLOA_B		5
#define _TXBnCTRL_MLOA_M		0x20
#define TXBnCTRL_TXERR
#define _TXBnCTRL_TXERR_B		4
#define _TXBnCTRL_TXERR_M		0x10
#define TXBnCTRL_TXREQ
#define _TXBnCTRL_TXREQ_B		3
#define _TXBnCTRL_TXREQ_M		0x08
#define TXBnCTRL_TXP
#define _TXBnCTRL_TXP_B			0
#define _TXBnCTRL_TXP_M			0x03

/* reg 3-2 */
#define TXRTSCTRL				(0x0D)
#define TXRTSCTRL_B2RTS
#define _TXRTSCTRL_B2RTS_B		5
#define _TXRTSCTRL_B2RTS_M		0x20
#define TXRTSCTRL_B1RTS
#define _TXRTSCTRL_B1RTS_B		4
#define _TXRTSCTRL_B1RTS_M		0x10
#define TXRTSCTRL_B0RTS
#define _TXRTSCTRL_B0RTS_B		3
#define _TXRTSCTRL_B0RTS_M		0x08
#define TXRTSCTRL_B2RTSM
#define _TXRTSCTRL_B2RTSM_B 	2
#define _TXRTSCTRL_B2RTSM_M		0x04
#define TXRTSCTRL_B1RTSM
#define _TXRTSCTRL_B1RTSM_B		1
#define _TXRTSCTRL_B1RTSM_M		0x02
#define TXRTSCTRL_B0RTSM
#define _TXRTSCTRL_B0RTSM_B		0
#define _TXRTSCTRL_B0RTSM_M		0x01

/* reg 3-3 */
#define TXBnSIDH(n)				(0x31+n*0x10)
#define TXBnSIDH_SID
#define _TXBnSIDH_SID_B			0
#define _TXBnSIDH_SID_M			0xff

/* reg 3-4 */
#define TXBnSIDL(n)				(0x32+n*0x10)
#define TXBnSIDL_SID
#define _TXBnSIDL_SID_B			5
#define _TXBnSIDL_SID_M			0xe0
#define TXBnSIDL_EXIDE
#define _TXBnSIDL_EXIDE_B		3
#define _TXBnSIDL_EXIDE_M		0x08
#define TXBnSIDL_EID
#define _TXBnSIDL_EID_B			0
#define _TXBnSIDL_EID_M			0x03

/* reg 3-5 */
#define TXBnEID8(n)				(0x33+n*0x10)
#define TXBnEID8_EID
#define _TXBnEID8_EID_B			0
#define _TXBnEID8_EID_M			0xff

/* reg 3-6 */
#define TXBnEID0(n)				(0x34+n*0x10)
#define TXBnEID0_EID
#define _TXBnEID0_EID_B			0
#define _TXBnEID0_EID_M			0xff

/* reg 3-7 */
#define TXBnDLC(n)				(0x35+n*0x10)
#define TXBnDLC_RTR
#define _TXBnDLC_RTR_B			6
#define _TXBnDLC_RTR_M			0x40
#define TXBnDLC_DLC
#define _TXBnDLC_DLC_B			0
#define _TXBnDLC_DLC_M			0x0f

/* regs 3-8 are just data bytes */
#define TXBnDm(n,m)				((0x36+n*0x10)+m*0x01)	/* txbuf n, byte m */

/* reg 4-1 */
#define RXB0CTRL				(0x60)
#define RXB0CTRL_RXM
#define _RXB0CTRL_RXM_B			5
#define _RXB0CTRL_RXM_M			0x60
#define RXB0CTRL_RXRTR
#define _RXB0CTRL_RXRTR_B		3
#define _RXB0CTRL_RXRTR_M		0x08
#define RXB0CTRL_BUKT
#define _RXB0CTRL_BUKT_B		2
#define _RXB0CTRL_BUKT_M		0x04
#define RXB0CTRL_FILHIT
#define _RXB0CTRL_FILHIT_B		0
#define _RXB0CTRL_FILHIT_M		0x01

/* reg 4-2 */
#define RXB1CTRL				(0x70)
#define RXB1CTRL_RXM
#define _RXB1CTRL_RXM_B			5
#define _RXB1CTRL_RXM_M			0x60
#define RXB1CTRL_RXRTR
#define _RXB1CTRL_RXRTR_B		3
#define _RXB1CTRL_RXRTR_M		0x08
#define RXB1CTRL_FILHIT
#define _RXB1CTRL_FILHIT_B		0
#define _RXB1CTRL_FILHIT_M		0x07

/* reg 4-3 */
#define BFPCTRL					(0x0C)
#define BFPCTRL_B1BFS
#define _BFPCTRL_B1BFS_B		5
#define _BFPCTRL_B1BFS_M		0x20
#define BFPCTRL_B0BFS
#define _BFPCTRL_B0BFS_B		4
#define _BFPCTRL_B0BFS_M		0x10
#define BFPCTRL_B1BFE
#define _BFPCTRL_B1BFE_B		3
#define _BFPCTRL_B1BFE_M		0x08
#define BFPCTRL_B0BFE
#define _BFPCTRL_B0BFE_B		2
#define _BFPCTRL_B0BFE_M		0x04
#define BFPCTRL_B1BFM
#define _BFPCTRL_B1BFM_B		1
#define _BFPCTRL_B1BFM_M		0x02
#define BFPCTRL_B0BFM
#define _BFPCTRL_B0BFM_B		0
#define _BFPCTRL_B0BFM_M		0x01

/* reg 4-4 */
#define RXBnSIDH(n)				(0x61+n*0x10)
#define RXBnSIDH_SID
#define _RXBnSIDH_SID_B			0
#define _RXBnSIDH_SID_M			0xff

/* reg 4-5 */
#define RXBnSIDL(n)				(0x62+n*0x10)
#define RXBnSIDL_SID
#define _RXBnSIDL_SID_B			5
#define _RXBnSIDL_SID_M			0xe0
#define RXBnSIDL_SRR
#define _RXBnSIDL_SRR_B			4
#define _RXBnSIDL_SRR_M			0x10
#define RXBnSIDL_IDE
#define _RXBnSIDL_IDE_B			3
#define _RXBnSIDL_IDE_M			0x08
#define RXBnSIDL_EID
#define _RXBnSIDL_EID_B			0
#define _RXBnSIDL_EID_M			0x03

/* reg 4-6 */
#define RXBnEID8(n)				(0x63+n*0x10)
#define RXBnEID8_EID
#define _RXBnEID8_EID_B			0
#define _RXBnEID8_EID_M			0xff

/* reg 4-7 */
#define RXBnEID0(n)				(0x64+n*0x10)
#define RXBnEID0_EID
#define _RXBnEID0_EID_B			0
#define _RXBnEID0_EID_M			0xff

/* reg 4-8 */
#define RXBnDLC(n)				(0x65+n*0x10)
#define RXBnDLC_RTR
#define _RXBnDLC_RTR_B			6
#define _RXBnDLC_RTR_M			0x40
#define RXBnDLC_RB
#define _RXBnDLC_RB_B			4
#define _RXBnDLC_RB_M			0x30
#define RXBnDLC_DLC
#define _RXBnDLC_DLC_B			0
#define _RXBnDLC_DLC_M			0x0f

/* regs 4-9 are just data bytes */
#define RXBnDm(n,m)				((0x66+n*0x10)+m*0x01)	/* rxbuf n, byte m */

/* reg 4-10 */
#define RXF0SIDH				(0x00)
#define RXF1SIDH				(0x04)
#define RXF2SIDH				(0x08)
#define RXF3SIDH				(0x10)
#define RXF4SIDH				(0x14)
#define RXF5SIDH				(0x18)
#define RXFnSIDH_SID
#define _RXFnSIDH_SID_B			0
#define _RXFnSIDH_SID_M			0xffffffff

/* reg 4-11 */
#define RXF0SIDL				(0x01)
#define RXF1SIDL				(0x05)
#define RXF2SIDL				(0x09)
#define RXF3SIDL				(0x11)
#define RXF4SIDL				(0x15)
#define RXF5SIDL				(0x19)
#define RXFnSIDL_SID
#define _RXFnSIDL_SID_B			5
#define _RXFnSIDL_SID_M			0xe0
#define RXFnSIDL_EXIDE
#define _RXFnSIDL_EXIDE_B		3
#define _RXFnSIDL_EXIDE_M		0x08
#define RXFnSIDL_EID
#define _RXFnSIDL_EID_B			0
#define _RXFnSIDL_EID_M			0x03

/* reg 4-12 */
#define RXF0EID8				(0x02)
#define RXF1EID8				(0x06)
#define RXF2EID8				(0x0A)
#define RXF3EID8				(0x12)
#define RXF4EID8				(0x16)
#define RXF5EID8				(0x1A)
#define RXFnEID8_EID
#define _RXFnEID8_EID_B			0
#define _RXFnEID8_EID_M			0xff

/* reg 4-13 */
#define RXF0EID0				(0x03)
#define RXF1EID0				(0x07)
#define RXF2EID0				(0x0B)
#define RXF3EID0				(0x13)
#define RXF4EID0				(0x17)
#define RXF5EID0				(0x1B)
#define RXFnEID0_EID
#define _RXFnEID0_EID_B			0
#define _RXFnEID0_EID_M			0xff

/* reg 4-14 */
#define RXMnSIDH(n)				(0x20+n*0x04)
#define RXMnSIDH_SID
#define _RXMnSIDH_SID_B			0
#define _RXMnSIDH_SID_M			0xff

/* reg 4-15 */
#define RXMnSIDL(n)				(0x21+n*0x04)
#define RXMnSIDL_SID
#define _RXMnSIDL_SID_B			5
#define _RXMnSIDL_SID_M			0xe0
#define RXMnSIDL_EID
#define _RXMnSIDL_EID_B			0
#define _RXMnSIDL_EID_M			0x03

/* reg 4-16 */
#define RXMnEID8(n)				(0x22+n*0x04)
#define RXMnEID8_EID
#define _RXMnEID8_EID_B			0
#define _RXMnEID8_EID_M			0xff

/* reg 4-18 */
#define RXMnEID0(n)				(0x23+n*0x04)
#define RXMnEID0_EID
#define _RXMnEID0_EID_B			0
#define _RXMnEID0_EID_M			0xff

/* reg 5-1 */
#define CNF1					(0x2A)
#define CNF1_SJW
#define _CNF1_SJW_B				6
#define _CNF1_SJW_M				0xc0
#define CNF1_BRP
#define _CNF1_BRP_B				0
#define _CNF1_BRP_M				0x3f

/* reg 5-2 */
#define CNF2					(0x29)
#define CNF2_BTLMODE
#define _CNF2_BTLMODE_B			7
#define _CNF2_BTLMODE_M			0x80
#define CNF2_SAM
#define _CNF2_SAM_B				6
#define _CNF2_SAM_M				0x40
#define CNF2_PHSEG1
#define _CNF2_PHSEG1_B			3
#define _CNF2_PHSEG1_M			0x38
#define CNF2_PRSEG
#define _CNF2_PRSEG_B			0
#define _CNF2_PRSEG_M			0x07

/* reg 5-3 */
#define CNF3					(0x28)
#define CNF3_WAKFIL
#define _CNF3_WAKFIL_B			6
#define _CNF3_WAKFIL_M			0x40
#define CNF3_PHSEG2
#define _CNF3_PHSEG2_B			0
#define _CNF3_PHSEG2_M			0x07

/* reg 6-1 */
#define TEC						(0x1C)
#define TEC_TEC
#define _TEC_TEC_B				0
#define _TEC_TEC_M				0xff

/* reg 6-2 */
#define REC						(0x1D)
#define REC_REC
#define _REC_REC_B				0
#define _REC_REC_M				0xff

/* reg 6-3 */
#define EFLG					(0x2D)
#define EFLG_RX1OVR
#define _EFLG_RX1OVR_B			7
#define _EFLG_RX1OVR_M			0x80
#define EFLG_RX0OVR
#define _EFLG_RX0OVR_B			6
#define _EFLG_RX0OVR_M			0x40
#define EFLG_TXBO
#define _EFLG_TXBO_B			5
#define _EFLG_TXBO_M			0x20
#define EFLG_TXEP
#define _EFLG_TXEP_B			4
#define _EFLG_TXEP_M			0x10
#define EFLG_RXEP
#define _EFLG_RXEP_B			3
#define _EFLG_RXEP_M			0x08
#define EFLG_TXWAR
#define _EFLG_TXWAR_B			2
#define _EFLG_TXWAR_M			0x04
#define EFLG_RXWAR
#define _EFLG_RXWAR_B			1
#define _EFLG_RXWAR_M			0x02
#define EFLG_EWARN
#define _EFLG_EWARN_B			0
#define _EFLG_EWARN_M			0xb01

/* reg 7-1 */
#define CANINTE					(0x2B)
#define CANINTE_MERRE
#define _CANINTE_MERRE_B		7
#define _CANINTE_MERRE_M		0x80
#define CANINTE_WAKIE
#define _CANINTE_WAKIE_B		6
#define _CANINTE_WAKIE_M		0x40
#define CANINTE_ERRIE
#define _CANINTE_ERRIE_B		5
#define _CANINTE_ERRIE_M		0x20
#define CANINTE_TX2IE
#define _CANINTE_TX2IE_B		4
#define _CANINTE_TX2IE_M		0x10
#define CANINTE_TX1IE
#define _CANINTE_TX1IE_B		3
#define _CANINTE_TX1IE_M		0x08
#define CANINTE_TX0IE
#define _CANINTE_TX0IE_B		2
#define _CANINTE_TX0IE_M		0x04
#define CANINTE_RX1IE
#define _CANINTE_RX1IE_B		1
#define _CANINTE_RX1IE_M		0x02
#define CANINTE_RX0IE
#define _CANINTE_RX0IE_B		0
#define _CANINTE_RX0IE_M		0x01

/* reg 7-2 */
#define CANINTF					(0x2C)
#define CANINTF_MERRF
#define _CANINTF_MERRF_B		7
#define _CANINTF_MERRF_M		0x80
#define CANINTF_WAKIF
#define _CANINTF_WAKIF_B		6
#define _CANINTF_WAKIF_M		0x40
#define CANINTF_ERRIF
#define _CANINTF_ERRIF_B		5
#define _CANINTF_ERRIF_M		0x20
#define CANINTF_TX2IF
#define _CANINTF_TX2IF_B		4
#define _CANINTF_TX2IF_M		0x10
#define CANINTF_TX1IF
#define _CANINTF_TX1IF_B		3
#define _CANINTF_TX1IF_M		0x08
#define CANINTF_TX0IF
#define _CANINTF_TX0IF_B		2
#define _CANINTF_TX0IF_M		0x04
#define CANINTF_RX1IF
#define _CANINTF_RX1IF_B		1
#define _CANINTF_RX1IF_M		0x02
#define CANINTF_RX0IF
#define _CANINTF_RX0IF_B		0
#define _CANINTF_RX0IF_M		0x01

/* reg 9-1 */
#define CANCTRL					(0x0F)
#define CANCTRL_REQOP
#define _CANCTRL_REQOP_B		5
#define _CANCTRL_REQOP_M		0xe0
#define CANCTRL_ABAT
#define _CANCTRL_ABAT_B			4
#define _CANCTRL_ABAT_M			0x10
#define CANCTRL_CLKEN
#define _CANCTRL_CLKEN_B		2
#define _CANCTRL_CLKEN_M		0x04
#define CANCTRL_CLKPRE
#define _CANCTRL_CLKPRE_B		0
#define _CANCTRL_CLKPRE_M		0x03

/* reg 9-2 */
#define CANSTAT					(0x0E)
#define CANSTAT_OPMOD
#define _CANSTAT_OPMOD_B		5
#define _CANSTAT_OPMOD_M		0xe0
#define CANSTAT_ICOD
#define _CANSTAT_ICOD_B			1
#define _CANSTAT_ICOD_M			0x0e


/* -----------------------------------------------------------------------------
 * Various other useful definitions.
 * ---------------------------------------------------------------------------*/

#define REQOP_NORMAL		0
#define REQOP_SLEEP			1
#define REQOP_LOOPBACK		2
#define REQOP_LISTENONLY	3
#define REQOP_CONFIG		4


#endif /*MCP2510_H_*/
