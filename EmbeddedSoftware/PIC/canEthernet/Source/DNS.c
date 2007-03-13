/*********************************************************************
 *
 *	DNS Client Module for Microchip TCP/IP Stack
 *	 -Provides DNS resolution (client)
 *
 *********************************************************************
 * FileName:        DNS.c
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
 * Howard Schlunder     7/31/06	Original
 ********************************************************************/
#define THIS_IS_DNS

#include "..\Include\Compiler.h"
#include "..\Include\UDP.h"
#include "..\Include\Tick.h"
#include "..\Include\ARP.h"
#include "..\Include\ARPTsk.h"
#include "..\Include\DNS.h"
#include "..\Include\Helpers.h"

#if defined(STACK_USE_DNS)

#define DNS_PORT		53
#define DNS_TIMEOUT		(TICK_1S*2)


static BYTE *DNSHostName;
static enum
{
	DNS_HOME = 0,
	DNS_RESOLVE_ARP,
	DNS_OPEN_SOCKET,
	DNS_QUERY,
	DNS_GET_RESULT,
	DNS_DONE
} smDNS = DNS_DONE;

typedef struct _DNS_HEADER
{
	WORD_VAL TransactionID;
	WORD_VAL Flags;
	WORD_VAL Questions;
	WORD_VAL Answers;
	WORD_VAL AuthoritativeRecords;
	WORD_VAL AdditionalRecords;
} DNS_HEADER;

typedef struct _DNS_ANSWER_HEADER
{
	WORD_VAL	ResponseName;
	WORD_VAL	ResponseType;
	WORD_VAL	ResponseClass;
	DWORD_VAL	ResponseTTL;
	WORD_VAL	ResponseLen;
} DNS_ANSWER_HEADER;

static void DNSPutString(BYTE *String);
static void DNSGetString(BYTE *String);

/*********************************************************************
 * Function:        void DNSResolve(BYTE *Hostname)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           *Hostname: Null terminated string specifying the 
 *							   host address to resolve to an IP 
 *							   address.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call DNSIsResolved() until the host is resolved.
 *
 * Note:            A UDP socket must be available before this 
 *					function is called.  It is freed at the end of 
 *					the resolution.  MAX_UDP_SOCKETS may need to be 
 *					increased if other modules use UDP sockets.
 *
 *					You must not modify *Hostname until DNSIsResolved() 
 *					returns TRUE.
 ********************************************************************/
void DNSResolve(BYTE *Hostname)
{
	DNSHostName = Hostname;
	smDNS = DNS_HOME;
}


/*********************************************************************
 * Function:        BOOL DNSIsResolved(IP_ADDR *HostIP)
 *
 * PreCondition:    DNSResolve() was called.
 *
 * Input:           HostIP: Pointer to IP_ADDR structure to store the 
 *							returned host IP address when DNS 
 *							resolution is complete.
 *
 * Output:          *HostIP, 4 byte IP address
 *
 * Side Effects:    None
 *
 * Overview:        Call DNSIsResolved() until the host is resolved.
 *
 * Note:            You cannot start two DNS resolution proceedures 
 *					concurrently.
 *
 *					You must not modify *Hostname until DNSIsResolved() 
 *					returns TRUE.
 ********************************************************************/
BOOL DNSIsResolved(IP_ADDR *HostIP)
{
	static UDP_SOCKET	MySocket;
	static NODE_INFO	Remote;
	static TICK			StartTime;
	BYTE 				i;
	WORD_VAL			w;
	DNS_HEADER			DNSHeader;
	DNS_ANSWER_HEADER	DNSAnswerHeader;

	switch(smDNS)
	{
		case DNS_HOME:
			ARPResolve(&AppConfig.PrimaryDNSServer);
			StartTime = tickGet();
			smDNS++;
			break;
		case DNS_RESOLVE_ARP:
			if(!ARPIsResolved(&AppConfig.PrimaryDNSServer, &Remote.MACAddr))
			{
				if(tickGet() - StartTime > DNS_TIMEOUT)
				{
					smDNS--;
				}
				break;
			}
			Remote.IPAddr.Val = AppConfig.PrimaryDNSServer.Val;
			smDNS++;
			// No need to break, we can immediately start resolution

		case DNS_OPEN_SOCKET:
			MySocket = UDPOpen(0, &Remote, DNS_PORT);
			if(MySocket == INVALID_UDP_SOCKET)
				break;

			smDNS++;
			// No need to break, we can immediately start resolution
			
		case DNS_QUERY:
			if(!UDPIsPutReady(MySocket))
				break;
			
			// Put DNS query here
			UDPPut(0x12);		// User chosen ID
			UDPPut(0x34);
			UDPPut(0x01);		// Standard query with recursion
			UDPPut(0x00);	
			UDPPut(0x00);		// 0x0001 questions
			UDPPut(0x01);
			UDPPut(0x00);		// 0x0000 answers
			UDPPut(0x00);
			UDPPut(0x00);		// 0x0000 name server resource records
			UDPPut(0x00);
			UDPPut(0x00);		// 0x0000 additional records
			UDPPut(0x00);

			// Put hostname string to resolve
			DNSPutString(DNSHostName);

			UDPPut(0x00);		// Type: A (host address)
			UDPPut(0x01);
			UDPPut(0x00);		// Class: IN (Internet)
			UDPPut(0x01);

			UDPFlush();
			StartTime = tickGet();
			smDNS++;
			break;

		case DNS_GET_RESULT:
			if(!UDPIsGetReady(MySocket))
			{
				if(tickGet() - StartTime > DNS_TIMEOUT)
				{
					smDNS--;
				}
				break;
			}

			// Retrieve the DNS header and de-big-endian it
			UDPGet(&DNSHeader.TransactionID.v[1]);
			UDPGet(&DNSHeader.TransactionID.v[0]);
			UDPGet(&DNSHeader.Flags.v[1]);
			UDPGet(&DNSHeader.Flags.v[0]);
			UDPGet(&DNSHeader.Questions.v[1]);
			UDPGet(&DNSHeader.Questions.v[0]);
			UDPGet(&DNSHeader.Answers.v[1]);
			UDPGet(&DNSHeader.Answers.v[0]);
			UDPGet(&DNSHeader.AuthoritativeRecords.v[1]);
			UDPGet(&DNSHeader.AuthoritativeRecords.v[0]);
			UDPGet(&DNSHeader.AdditionalRecords.v[1]);
			UDPGet(&DNSHeader.AdditionalRecords.v[0]);

			// Remove all questions
			while(DNSHeader.Questions.Val--)
			{
				DNSGetString(NULL);
				UDPGet(&w.v[1]);		// Question type
				UDPGet(&w.v[0]);
				UDPGet(&w.v[1]);		// Question class
				UDPGet(&w.v[0]);
			}
			
			// Scan through answers
			while(DNSHeader.Answers.Val--)
			{
				UDPGet(&DNSAnswerHeader.ResponseName.v[1]);		// Response name
				UDPGet(&DNSAnswerHeader.ResponseName.v[0]);
				UDPGet(&DNSAnswerHeader.ResponseType.v[1]);		// Response type
				UDPGet(&DNSAnswerHeader.ResponseType.v[0]);
				UDPGet(&DNSAnswerHeader.ResponseClass.v[1]);	// Response class
				UDPGet(&DNSAnswerHeader.ResponseClass.v[0]);
				UDPGet(&DNSAnswerHeader.ResponseTTL.v[3]);		// Time to live
				UDPGet(&DNSAnswerHeader.ResponseTTL.v[2]);
				UDPGet(&DNSAnswerHeader.ResponseTTL.v[1]);
				UDPGet(&DNSAnswerHeader.ResponseTTL.v[0]);
				UDPGet(&DNSAnswerHeader.ResponseLen.v[1]);		// Response length
				UDPGet(&DNSAnswerHeader.ResponseLen.v[0]);

				// Make sure that this is a 4 byte IP address, response type A, class 1
				// Check if this is Type A
				if(	DNSAnswerHeader.ResponseType.Val	== 0x0001 && 
					DNSAnswerHeader.ResponseClass.Val	== 0x0001 && // Internet class
					DNSAnswerHeader.ResponseLen.Val		== 0x0004)
				{
					UDPGet(&HostIP->v[0]);
					UDPGet(&HostIP->v[1]);
					UDPGet(&HostIP->v[2]);
					UDPGet(&HostIP->v[3]);
					break;
				}
				else
				{
					while(DNSAnswerHeader.ResponseLen.Val--)
					{
						UDPGet(&i);
					}
				}
			}

			UDPDiscard();
			UDPClose(MySocket);
			MySocket = INVALID_UDP_SOCKET;
			smDNS++;
			// No need to break, we are done and need to return TRUE

		case DNS_DONE:
			return TRUE;
	}
	
	return FALSE;
}

static void DNSPutString(BYTE *String)
{
	BYTE *RightPtr;
	BYTE i;
	BYTE Len;

	RightPtr = String;

	while(1)
	{
		do
		{
			i = *RightPtr++;
		} while((i != 0x00) && (i != '.') && (i != '/'));
	
		// Put the length parameter
		Len = (BYTE)(RightPtr-String-1);
		UDPPut(Len);
		while(Len--)
		{
			UDPPut(*String++);
		}
		if(i == 0x00 || i == '/')
			break;

		// Skip over the '.' in the input string
		String++;
	}
	
	// Put the string terminator character
	UDPPut(0x00);
}

static void DNSGetString(BYTE *String)
{
	BYTE i;
	BYTE j;

	if(String == NULL)
	{
		UDPGet(&i);
		while(i)
		{
			while(i--)
			{
				UDPGet(&j);
			}
			UDPGet(&i);
		}
	}
	else
	{
		UDPGet(&i);
		while(i)
		{
			while(i--)
			{
				UDPGet(String);
				String++;
			}
			UDPGet(&i);
		}
	}
}


#endif	//#if defined(STACK_USE_DNS)
