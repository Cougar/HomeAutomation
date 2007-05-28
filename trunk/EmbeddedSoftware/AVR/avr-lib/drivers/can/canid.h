#ifndef CANID_H_
#define CANID_H_

//---------------------------------------------------------------------------
// CAN ID definitions

#define CAN_MASK_CLASS		0x1E000000
#define CAN_SHIFT_CLASS		25
//------------------------------------
#define CAN_NMT				0x00UL
#define CAN_MASK_NMT_TYPE	0x00FF0000
#define CAN_SHIFT_NMT_TYPE	16
#define CAN_MASK_NMT_SID	0x0000FF00
#define CAN_SHIFT_NMT_SID	8
#define CAN_MASK_NMT_RID	0x000000FF
#define CAN_SHIFT_NMT_RID	0
#define CAN_NMT_TIME		0x00UL	//D0:3 UNIX_TIME, D4:7 {YY:6, MM:4, DD:5, hh:5, mm:6, ss:6}
#define CAN_NMT_RESET		0x04UL
#define CAN_NMT_BIOS_START	0x08UL	//D0: VER7:0, D1:VER15:8, D2: 0 if no app, >0 if app
#define CAN_NMT_PGM_START	0x0CUL	//D0: ADR7:0, D1:ADR15:8, D2:ADR23:16, D3:ADR31:24
#define CAN_NMT_PGM_DATA	0x10UL	//D0: OFS7:0, D1: OFS15:8, D(n+2): DATAn7:0 [n=0..x] [x=0..5]
#define CAN_NMT_PGM_END		0x14UL	//D0: CRC7:0, D1: CRC15:8
#define CAN_NMT_PGM_COPY	0x18UL	//D0: SRC7:0, D1:SRC15:8, D0: DST7:0, D1:DST15:8, D0: LEN7:0, D1:LEN15:8
#define CAN_NMT_PGM_ACK		0x1CUL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_PGM_NACK	0x20UL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_START_APP	0x24UL
#define CAN_NMT_APP_START	0x28UL	//D0: ID7:0, D1:ID15:8, D2: VER7:0, D3:VER15:8
#define CAN_NMT_HEARTBEAT	0x2CUL
//------------------------------------
#define CAN_SNS				0x02UL
#define CAN_MASK_SNS_TYPE	0x01FF8000
#define CAN_SHIFT_SNS_TYPE	15
#define CAN_MASK_SNS_ID		0x00007E00
#define CAN_SHIFT_SNS_ID	9
#define CAN_MASK_SNS_SID	0x000001FF
#define CAN_SHIFT_SNS_SID	0
//------------------------------------
#define CAN_ACT				0x04UL
#define CAN_MASK_ACT_TYPE	0x01FF8000
#define CAN_SHIFT_ACT_TYPE	15
#define CAN_MASK_ACT_ID		0x00007E00
#define CAN_SHIFT_ACT_ID	9
#define CAN_MASK_ACT_SID	0x000001FF
#define CAN_SHIFT_ACT_SID	0
//------------------------------------
#define CAN_PKT				0x06UL
//------------------------------------
#define CAN_CON				0x08UL
//------------------------------------
#define CAN_TST				0x0FUL
//------------------------------------

//---------------------------------------------------------------------------
// CAN ID definitions for NMT messages

#define CAN_ID_NMT_BIOS_START  ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_BIOS_START << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))
#define CAN_ID_NMT_PGM_ACK     ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_ACK << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))
#define CAN_ID_NMT_PGM_NACK    ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_NACK << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))

#define CAN_APPTYPES_IRRECEIVER     0xf010
#define CAN_APPTYPES_RELAY          0xf015
#define CAN_APPTYPES_SERVO          0xf020

// CAN ID definitions for SNS messages
#define SNS_TYPE_RELAY_STATUS         0x0BL
#define SNS_TYPE_SERVO_STATUS         0x0dL
#define SNS_TYPE_CAN_IR               0x12L

//#define CAN_ID_SNS_IRDATA     ((CAN_SNS << CAN_SHIFT_CLASS) \
//                              | (SNS_FUNCC_CAN_IR << CAN_SHIFT_SNS_FUNCC) \
//                              | (NODE_ID << CAN_SHIFT_SNS_SID))

//#define CAN_ID_SNS_RELAYDATA  ((CAN_SNS << CAN_SHIFT_CLASS) \
//                              | (SNS_RELAY_STATUS << CAN_SHIFT_SNS_FUNCC) \
//                              | (NODE_ID << CAN_SHIFT_SNS_SID))

// CAN ID definitions for ACT messages
#define ACT_TYPE_RELAY         0x01L

//#define CAN_ID_ACT_RELAY     ((CAN_ACT << CAN_SHIFT_CLASS) \
//                              | (CAN_RELAY << CAN_SHIFT_ACT_FUNCC) \
//                              | (NODE_ID << CAN_SHIFT_ACT_SID))

#define ACT_TYPE_SERVO         0x02L

//#define CAN_ID_ACT_SERVO     ((CAN_ACT << CAN_SHIFT_CLASS) \
//                              | (CAN_SERVO << CAN_SHIFT_ACT_FUNCC) \
//                              | (NODE_ID << CAN_SHIFT_ACT_SID))

#endif /*CANID_H_*/
