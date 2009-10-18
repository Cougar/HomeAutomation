#ifndef __CANPROTOMESSAGE_H
#define __CANPROTOMESSAGE_H

#include <tchar.h>
#include <windows.h>
#include "compiler.h"

class CanProtoMessage
{
public:
	CanProtoMessage();
	CanProtoMessage(unsigned char funct, unsigned int funcc, unsigned char nid, unsigned int sid, unsigned char data_length, unsigned char data[]);
	CanProtoMessage(unsigned char bytes[],int startIndex);
	~CanProtoMessage();
	bool equals(CanProtoMessage *cpm);
	unsigned char getFunct(void);
	unsigned int getFuncc(void);
	unsigned char getNid(void);
	unsigned int getSid(void);
	unsigned char getDataLength(void);
	unsigned char* getData(void);
	void getBytes(unsigned char bytes[]);
protected:
	unsigned char funct;
	unsigned int funcc;
	unsigned char nid;
	unsigned int sid;
	unsigned char data_length;
    unsigned char data[8];
};


#endif