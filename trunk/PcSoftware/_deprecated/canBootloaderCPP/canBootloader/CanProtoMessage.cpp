#define STRICT
#include "CanProtoMessage.h"
 

CanProtoMessage::CanProtoMessage() {}

CanProtoMessage::CanProtoMessage(unsigned char funct, unsigned int funcc, unsigned char nid, unsigned int sid, unsigned char data_length, unsigned char data[])
{
	CanProtoMessage::funct=funct;
	CanProtoMessage::funcc=funcc;
	CanProtoMessage::nid=nid;
	CanProtoMessage::sid=sid;
	CanProtoMessage::data_length=data_length;
	for(int i=0;i<8;i++) CanProtoMessage::data[i]=data[i];
}

CanProtoMessage::CanProtoMessage(unsigned char bytes[],int startIndex)
{
	// Constructor
	// 17 bytes, a packet.
	// UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
    /*
	* 000TTTTC CCCCCCCC CNNNNNNS SSSSSSSS
	*
	* <FUNCT> = 00011110 00000000 00000000 00000000 = 0x1E000000	max(0xF, 15)
	* <FUNCC> = 00000001 11111111 10000000 00000000 = 0x01FF8000    max(0x3FF, 1023)
	* <NID>   = 00000000 00000000 01111110 00000000 = 0x00007E00	max(0x3F, 63)
	* <SID>   = 00000000 00000000 00000001 11111111 = 0x000001FF	max(0x1FF, 511)
	* 
	<FUNCT>, 4 bitar = Function Type, funktionstyp.
	<FUNCC>, 10 bitar = Function Code, funktionskod.
	<NID>, 6 bitar = Network ID, nätverksid.
	<SID>, 9 bitar = Sender ID, sändarid.
	*/
	
	// To fix lazy indexing.
	startIndex--;

	CanProtoMessage::funct = ((0x1E & bytes[startIndex+1])>>1); // 00011110 & id[0]
	CanProtoMessage::funcc = ((0x01 & bytes[startIndex+1])<<9) + ((0xFF & bytes[startIndex+2])<<1) + ((0x80 & bytes[startIndex+3])>>7); // 00000001 & id[0]<<16 + 11111111 & id[1]<<8 + 10000000 & id[2]
	CanProtoMessage::nid   = ((0x7E & bytes[startIndex+3])>>1); // 01111110 & id[2]
	CanProtoMessage::sid   = ((0x01 & bytes[startIndex+3])<<8) + (0xFF & bytes[startIndex+4]); // 00000001 & id[2]<<8 + 11111111 & id[3]

	CanProtoMessage::data_length = bytes[startIndex+7];

	for(int i=0;i<8;i++) CanProtoMessage::data[i]=bytes[startIndex+8+i];

}

CanProtoMessage::~CanProtoMessage()
{
	//Destructor
}

bool CanProtoMessage::equals(CanProtoMessage *cpm)
{ 
	unsigned char * bytes;
	
	bytes = cpm->getData();


	for(int i=0;i<8;i++) if (CanProtoMessage::data[i]!=bytes[i]) return false;

	return (CanProtoMessage::funct==cpm->getFunct() && CanProtoMessage::funcc==cpm->getFuncc() && CanProtoMessage::nid==cpm->getNid() && CanProtoMessage::sid==cpm->getSid() && CanProtoMessage::data_length==cpm->getDataLength());

}

unsigned char CanProtoMessage::getFunct(){ return CanProtoMessage::funct; }
unsigned int CanProtoMessage::getFuncc(){ return CanProtoMessage::funcc; }
unsigned char CanProtoMessage::getNid(){ return CanProtoMessage::nid; }
unsigned int CanProtoMessage::getSid(){ return CanProtoMessage::sid; }
unsigned char CanProtoMessage::getDataLength(){ return CanProtoMessage::data_length; }
unsigned char* CanProtoMessage::getData(){ return CanProtoMessage::data; }
void CanProtoMessage::getBytes(unsigned char bytes[])
{

	// 17 bytes, a packet.
	// UART_START_BYTE id[0] id[1] id[2] id[3] extended remote_request data_length d[0] d[1] d[2] d[3] d[4] d[5] d[6] d[7] UART_END_BYTE
    /*
	* 000TTTTC CCCCCCCC CNNNNNNS SSSSSSSS
	*
	* <FUNCT> = 00011110 00000000 00000000 00000000 = 0x1E000000
	* <FUNCC> = 00000001 11111111 10000000 00000000 = 0x01FF8000
	* <NID>   = 00000000 00000000 01111110 00000000 = 0x00007E00
	* <SID>   = 00000000 00000000 00000001 11111111 = 0x000001FF
	* 
	<FUNCT>, 4 bitar = Function Type, funktionstyp.
	<FUNCC>, 10 bitar = Function Code, funktionskod.
	<NID>, 6 bitar = Network ID, nätverksid.
	<SID>, 9 bitar = Sender ID, sändarid.
	*/

	bytes[0]=UART_START_BYTE;
	bytes[1]=((CanProtoMessage::funct & 0x0F)<<1)+((CanProtoMessage::funcc & 0x200)>>9);
	bytes[2]=((CanProtoMessage::funcc & 0x1FE)>>1);
	bytes[3]=((CanProtoMessage::funcc & 0x01)<<7)+((CanProtoMessage::nid & 0x3F)<<1)+((CanProtoMessage::sid & 0x100)>>8);
	bytes[4]=(CanProtoMessage::sid & 0xFF);
	bytes[5]=1;
	bytes[6]=0;
	bytes[7]=CanProtoMessage::data_length;
	for(int i=0;i<8;i++) bytes[i+8]=CanProtoMessage::data[i];
	bytes[16]=UART_END_BYTE;
}