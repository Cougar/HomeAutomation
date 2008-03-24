namespace canConfigurator{
	public class td{
public enum PACKET_TYPE : byte { ptBOOT=0x0, ptPGM=0x1 };
public enum BOOT_TYPE : byte { btADDR = 0x0, btPGM = 0x1, btDONE = 0x2, btACK = 0x4, btNACK = 0x5, btCRC = 0x6, btCHANGEID = 0x7 };
public enum PGM_CLASS : byte { pcCTRL=0x0, pcSENSOR=0x1, pcACTUATOR=0x2 };

/*Control like heartbeat, eeprom, etc.*/
public enum PGM_CTRL_TYPE : ushort {pctBOOTBOOT=0x0000};
/*BOOTBOOT
data[2] = ..
data[1] = ..*/


/*Sensors.*/
public enum PGM_SENSOR_TYPE : ushort {};


/*actuators.*/
public enum PGM_ACTUATOR_TYPE : ushort {patACT1=0x0000};
/*ACT1
tja!
hej!*/


	}
}