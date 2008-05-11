/*********************************************************************
 *
 *  Generic TCP Client Example Application
 *   -Implements an example HTTP client and should be used as a basis 
 *	  for creating new TCP client applications
 *
 *********************************************************************
 * FileName:        GenericTCPClient.c
 * Dependencies:    TCP.h, DNS.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33F
 * Complier:        Microchip C18 v3.03 or higher
 * 					Microchip C30 v2.02 or higher
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
#define THIS_IS_TCP_CLIENT_EXAMPLE

#include "..\Include\DNS.h"
#include "..\Include\Helpers.h"
#include "..\Include\TCP.h"
#include "..\Include\ARPTsk.h"
#include "..\Include\UART.h"

#if defined(STACK_USE_GENERIC_TCP_EXAMPLE)

//TODO: Define proper server address here
BYTE ServerName[] =	"johboh.csbnet.se";
WORD ServerPort = 80;

// This is specific to this HTTP Client example
BYTE RemoteURL[] = "/time.php";


typedef enum _GenericT
	{
		SM_HOME = 0,
		SM_NAME_RESOLVE,
		SM_ARP_START_RESOLVE,
		SM_ARP_RESOLVE,
		SM_SOCKET_OBTAIN,
		SM_SOCKET_OBTAINED,
		SM_PROCESS_RESPONSE,
		SM_DISCONNECT,
		SM_DONE
	} 	GenericT;

/*********************************************************************
 * Function:        void GenericTCPClient(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void GenericTCPClient(void)
{
	BYTE 				i;
	BYTE 				*StringPtr;
	static TICK			Timer;
	static TCP_SOCKET	MySocket = INVALID_SOCKET;
	static NODE_INFO	Server;
	static GenericT GenericTCPExampleState = SM_DONE;

	switch(GenericTCPExampleState)
	{
		case SM_HOME:
			// Obtain the IP address associated with the common ServerName
			DNSResolve(ServerName);
			GenericTCPExampleState++;
			break;

		case SM_NAME_RESOLVE:
			// Wait for the DNS server to return the requested IP address
			if(!DNSIsResolved(&Server.IPAddr))
				break;
			GenericTCPExampleState++;
		break;

		case SM_ARP_START_RESOLVE:
			// Obtain the MAC address associated with the server's IP address (either direct MAC address on same subnet, or the MAC address of the Gateway machine)
			ARPResolve(&Server.IPAddr);
			Timer = TickGet();
			GenericTCPExampleState++;
			break;

		case SM_ARP_RESOLVE:
			// Wait for the MAC address to finish being obtained
			if(!ARPIsResolved(&Server.IPAddr, &Server.MACAddr))
			{
				// Time out if too much time is spent in this state
				if(TickGet()-Timer > 1*TICK_SECOND)
				{
					// Retransmit ARP request
					GenericTCPExampleState--;
				}
				break;
			}
			GenericTCPExampleState++;
		break;

		case SM_SOCKET_OBTAIN:
			// Connect a socket to the remote TCP server
			MySocket = TCPConnect(&Server, ServerPort);
			
			// Abort operation if no TCP sockets are available
			// If this ever happens, incrementing MAX_TCP_SOCKETS in 
			// StackTsk.h may help (at the expense of more global memory 
			// resources).
			if(MySocket == INVALID_SOCKET)
				break;

			GenericTCPExampleState++;
			Timer = TickGet();
			break;

		case SM_SOCKET_OBTAINED:
			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(MySocket))
			{
				// Time out if too much time is spent in this state
				if(TickGet()-Timer > 5*TICK_SECOND)
				{
					// Close the socket so it can be used by other modules
					TCPDisconnect(MySocket);
					MySocket = INVALID_SOCKET;
					GenericTCPExampleState--;
				}
				break;
			}

			Timer = TickGet();

			// Make certain the socket can be written to
			if(!TCPIsPutReady(MySocket))
				break;
			
			// Place the application protocol data into the transmit buffer.  For this example, we are connected to an HTTP server, so we'll send an HTTP GET request.
			TCPPut(MySocket, 'G');
			TCPPut(MySocket, 'E');
			TCPPut(MySocket, 'T');
			TCPPut(MySocket, ' ');
			{
				StringPtr = RemoteURL;
				for(i = 0; i < strlen(RemoteURL); i++)
				{
					TCPPut(MySocket, *StringPtr++);
				} 
			}
			TCPPut(MySocket, ' ');
			TCPPut(MySocket, 'H');
			TCPPut(MySocket, 'T');
			TCPPut(MySocket, 'T');
			TCPPut(MySocket, 'P');
			TCPPut(MySocket, '/');
			TCPPut(MySocket, '1');
			TCPPut(MySocket, '.');
			TCPPut(MySocket, '1');
			TCPPut(MySocket, '\r');
			TCPPut(MySocket, '\n');
			TCPPut(MySocket, 'H');
			TCPPut(MySocket, 'o');
			TCPPut(MySocket, 's');
			TCPPut(MySocket, 't');
			TCPPut(MySocket, ':');
			TCPPut(MySocket, ' ');
			{
				StringPtr = ServerName;
				
				for(i = 0; i < strlen(ServerName); i++)
				{
					TCPPut(MySocket, *StringPtr++);
				} 
			}
			TCPPut(MySocket, '\r');
			TCPPut(MySocket, '\n');
			TCPPut(MySocket, '\r');
			TCPPut(MySocket, '\n');

			// Send the packet
			TCPFlush(MySocket);
			GenericTCPExampleState++;
		break;

		case SM_PROCESS_RESPONSE:
			// Check to see if the remote node has disconnected from us or sent us any application data
			// If application data is available, write it to the UART
			if(!TCPIsConnected(MySocket))
			{
				GenericTCPExampleState++;
			}
	
			if(!TCPIsGetReady(MySocket))
				break;
	
			// Obtain the server reply
			while(TCPGet(MySocket, &i))
			{
				while(BusyUART());
				WriteUART(i);
			}
	
			break;
	
		case SM_DISCONNECT:
			// Close the socket so it can be used by other modules
			// For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;
			GenericTCPExampleState = SM_DONE;
			break;
	
		case SM_DONE:
			putrsUART("done..\r\n");
			// Do nothing unless the user pushes BUTTON1 and wants to restart the whole connection/download process
			if(BUTTON0_IO == 0)
			{
				GenericTCPExampleState = SM_HOME;
				putrsUART("Go!\r\n");
			}
			break;
	}
}

#endif	//#if defined(STACK_USE_GENERIC_TCP_EXAMPLE)
