#ifndef __COMPILER_H
#define __COMPILER_H

#define UART_START_BYTE 253
#define UART_END_BYTE 250
#define PACKET_LENGTH 17

//<FUNCT>
#define FUNCT_BOOTLOADER 0x0

//<FUNCC> configuration packet
#define FUNCC_BOOT_INIT 0x01 //för att initiera en programmering.
#define FUNCC_BOOT_ADDR 0x02  //för att ändra adress.
#define FUNCC_BOOT_DONE 0x05  //för att avsluta en programmering.

//<FUNCC> program packet
#define FUNCC_BOOT_PGM 0x03 //för att indikera att detta är programdata som ska skrivas.

//<FUNCC> ack packet
#define FUNCC_BOOT_ACK 0x04 //för att indikera att detta är en ACK.

#define MY_ID 0x91 
#define MY_NID 0x00
#define TARGET_ID 0x78

#define ADDRL_INDEX 0
#define ADDRH_INDEX 1
#define ADDRU_INDEX 2
#define RID_INDEX 4
#define ERR_INDEX 4

//<ERR> 8 bitar feltyp
#define ERR_NO_ERROR 0x00 //= inget fel
#define ERR_ERROR 0x01 //= fel

#define TIMEOUT_MS 150

#endif