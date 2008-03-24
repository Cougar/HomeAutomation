/*********************************************************************
 *
 *	NetBIOS Name Service (NBNS) Server
 *	 -Responds to NBNS name requests to allow human name assignment 
 *	  to the board
 *
 *********************************************************************
 * FileName:        NBNS.c
 * Dependencies:    UDP, ARP, Tick
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     8/01/06	Original
 ********************************************************************/
#define THIS_IS_NET_BIOS_NAME_SERVICE

#include "..\Include\Compiler.h"
#include "..\Include\StackTsk.h"
#include "..\Include\UDP.h"
#include "..\Include\Tick.h"
#include "..\Include\Helpers.h"

#if defined(STACK_USE_NBNS)

#define NBNS_PORT		(137u)

typedef struct _NBNS_HEADER
{
	WORD_VAL TransactionID;
	WORD_VAL Flags;
	WORD_VAL Questions;
	WORD_VAL Answers;
	WORD_VAL AuthoritativeRecords;
	WORD_VAL AdditionalRecords;
} NBNS_HEADER;

static void NBNSPutName(BYTE *String);
static void NBNSGetName(BYTE *String);


/*********************************************************************
 * Function:        void NBNSTask(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Sends responses to NetBIOS name requests
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void NBNSTask(void)
{
	static UDP_SOCKET	MySocket;
	BYTE 				i;
	WORD_VAL			Type, Class;
	NBNS_HEADER			NBNSHeader;
	BYTE				NameString[16];
	static enum
	{
		NBNS_HOME = 0,
		NBNS_OPEN_SOCKET,
		NBNS_LISTEN
	} smNBNS = NBNS_HOME;

	switch(smNBNS)
	{
		case NBNS_HOME:
			smNBNS++;
			break;

		case NBNS_OPEN_SOCKET:
			MySocket = UDPOpen(NBNS_PORT, NULL, NBNS_PORT);
			if(MySocket == INVALID_UDP_SOCKET)
				break;

			smNBNS++;

		case NBNS_LISTEN:
			if(!UDPIsGetReady(MySocket))
				break;

			// Retrieve the NBNS header and de-big-endian it
			UDPGet(&NBNSHeader.TransactionID.v[1]);
			UDPGet(&NBNSHeader.TransactionID.v[0]);
			UDPGet(&NBNSHeader.Flags.v[1]);
			UDPGet(&NBNSHeader.Flags.v[0]);
			UDPGet(&NBNSHeader.Questions.v[1]);
			UDPGet(&NBNSHeader.Questions.v[0]);
			UDPGet(&NBNSHeader.Answers.v[1]);
			UDPGet(&NBNSHeader.Answers.v[0]);
			UDPGet(&NBNSHeader.AuthoritativeRecords.v[1]);
			UDPGet(&NBNSHeader.AuthoritativeRecords.v[0]);
			UDPGet(&NBNSHeader.AdditionalRecords.v[1]);
			UDPGet(&NBNSHeader.AdditionalRecords.v[0]);

			// Remove all questions
			while(NBNSHeader.Questions.Val--)
			{
				NBNSGetName(NameString);
				UDPGet(&i);				// <??> Trailing character on string
				UDPGet(&Type.v[1]);		// Question type
				UDPGet(&Type.v[0]);
				UDPGet(&Class.v[1]);	// Question class
				UDPGet(&Class.v[0]);
				
				if(Type.Val == 0x0020 && Class.Val == 0x0001 && strcmp(NameString, AppConfig.NetBIOSName) == 0)
				{
					while(!UDPIsPutReady(MySocket));

					NBNSHeader.Flags.Val = 0x8400;

					UDPPut(NBNSHeader.TransactionID.v[1]);
					UDPPut(NBNSHeader.TransactionID.v[0]);
					UDPPut(NBNSHeader.Flags.v[1]);
					UDPPut(NBNSHeader.Flags.v[0]);
					UDPPut(0x00);	// 0x0000 Questions
					UDPPut(0x00);
					UDPPut(0x00);	// 0x0001 Answers
					UDPPut(0x01);
					UDPPut(0x00);	// 0x0000 Athoritative records
					UDPPut(0x00);
					UDPPut(0x00);	// 0x0000 Additional records
					UDPPut(0x00);

					NBNSPutName(AppConfig.NetBIOSName);
					UDPPut(0x00);	// 0x0020 Type: NetBIOS
					UDPPut(0x20);
					UDPPut(0x00);	// 0x0001 Class: Internet
					UDPPut(0x01);
					UDPPut(0x00);	// 0x00000000 Time To Live
					UDPPut(0x00);
					UDPPut(0x00);
					UDPPut(0x00);

					UDPPut(0x00);	// 0x0006 Data length
					UDPPut(0x06);	
					UDPPut(0x60);	// 0x6000 Flags: H-node, Unique
					UDPPut(0x00);
					UDPPut(AppConfig.MyIPAddr.v[0]);	// Put out IP address
					UDPPut(AppConfig.MyIPAddr.v[1]);
					UDPPut(AppConfig.MyIPAddr.v[2]);
					UDPPut(AppConfig.MyIPAddr.v[3]);

					UDPFlush();				
				}

			}
			
			UDPDiscard();

			break;
	}
}

static void NBNSPutName(BYTE *String)
{
	BYTE i, j;

	UDPPut(32);	// NetBIOS names are always 32 bytes long (16 decoded bytes)
	for(i = 0; i < 16; i++)
	{
		j = *String++;
		UDPPut((j>>4) + 'A');
		UDPPut((j & 0x0F) + 'A');
	}
	
	UDPPut(0x00);
}

static void NBNSGetName(BYTE *String)
{
	BYTE i, j, k;

	if(String == NULL)
	{
		UDPGet(&i);
		while(i--)
		{
			UDPGet(&j);
		}
	}
	else
	{
		UDPGet(&i);
		while(i--)
		{
			UDPGet(&j);
			j -= 'A';
			k = j<<4;
			i--;
			UDPGet(&j);
			j -= 'A';
			*String++ = k | j;
		}
	}
}


#endif //#if defined(STACK_USE_NBNS)
