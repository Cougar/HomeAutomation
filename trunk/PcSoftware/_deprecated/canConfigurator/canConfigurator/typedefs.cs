namespace canConfigurator{
	public class td{
public enum PACKET_TYPE : byte { ptBOOT=0x0, ptPGM=0x1 };
public enum BOOT_TYPE : byte { btADDR = 0x0, btPGM = 0x1, btDONE = 0x2, btACK = 0x4, btNACK = 0x5, btCRC = 0x6, btCHANGEID = 0x7 };
public enum PGM_CLASS : byte { pcCTRL=0x0, pcSENSOR=0x1, pcACTUATOR=0x2 };

/*Control like heartbeat, eeprom, etc.*/
public enum PGM_CTRL_TYPE : ushort {pctBOOTBOOT=0x0000, pctAPPBOOT=0x0001, pctEE_START=0x0002, pctEE_PGM=0x0003, pctEE_CRC=0x0004, pctEE_ACK=0x0005, pctEE_NACK=0x0006, pctEE_DONE=0x0007, pctHEARTBEAT=0x0008};
/*
pctBOOTBOOT
data[2] = startup reason data[1] = Bootloader version high byte.
data[0] = Bootloader version low byte.
*/
/*
pctAPPBOOT
data[1] = Bootloader version high byte.
data[0] = Bootloader version low byte.
*/
/*
pctEE_START
data[4] = RID
data[3] = 1 = external, 0 = internal eeprom
data[2] = ADDRU
data[1] = ADDRH
data[0] = ADDRL
*/
/*
pctEE_PGM
data[7] = offset
data[6-0] = DATA
*/
/*
pctEE_CRC
data[4] = CRCH
data[3] = CRCL
data[2] = ADDRU
data[1] = ADDRH
data[0] = ADDRL
*/
/*
pctEE_ACK

*/
/*
pctEE_NACK

*/
/*
pctEE_DONE
data[2] = number of valid bytes U
data[1] = number of valid bytes H
data[0] = number of valid bytes L
*/
/*
pctHEARTBEAT

*/


/*Sensors.*/
public enum PGM_SENSOR_TYPE : ushort {pstIR=0x0000, pstTEMP_INSIDE=0x0001, pstTEMP_OUTSIDE=0x0002, pstTEMP_FREEZER=0x0003, pstTEMP_REFRIGERATOR=0x0004, pstTEMP_DOOR_OUTSIDE=0x0005, pstDOOR_FREEZER=0x0006, pstDOOR_REFRIGERATOR=0x0007, pstDOOR_OUTSIDE=0x0008, pstBLIND_TR=0x0009};
/*
pstIR
data[3] = type
data[2] = toogle
data[1] = addr
data[0] = data
*/
/*
pstTEMP_INSIDE
data[1] = integer value -127 to 127
data[0] = decimal value 0-9
*/
/*
pstTEMP_OUTSIDE
data[1] = integer value -127 to 127
data[0] = decimal value 0-9
*/
/*
pstTEMP_FREEZER
data[1] = integer value -127 to 127
data[0] = decimal value 0-9
*/
/*
pstTEMP_REFRIGERATOR
data[1] = integer value -127 to 127
data[0] = decimal value 0-9
*/
/*
pstTEMP_DOOR_OUTSIDE
data[1] = integer value -127 to 127
data[0] = decimal value 0-9
*/
/*
pstDOOR_FREEZER
data[0] = 0 closed, 1 open
*/
/*
pstDOOR_REFRIGERATOR
data[0] = 0 closed, 1 open
*/
/*
pstDOOR_OUTSIDE
data[0] = 0 closed, 1 open
*/
/*
pstBLIND_TR
data[0] = Precent value 0-100
*/


/*actuators.*/
public enum PGM_ACTUATOR_TYPE : ushort {patBLIND_TR=0x0000};
/*
patBLIND_TR
data[1]=0 = absoulte, 1 = relative
When 0 absolute = data[0] = Precent value 0-100
When 1  relative =  data[0] = signed -100 to 100
*/


	}
}