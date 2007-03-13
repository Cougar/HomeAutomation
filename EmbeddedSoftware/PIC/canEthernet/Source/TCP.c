/*********************************************************************
*
*                  TCP Module for Microchip TCP/IP Stack
*					Based on RFC 793
*
*********************************************************************
* FileName:        TCP.C
* Dependencies:    string.h
*                  StackTsk.h
*                  Helpers.h
*                  IP.h
*                  MAC.h
*                  ARP.h
*                  Tick.h
*                  TCP.h
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
 * Nilesh Rajbharti     5/8/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     11/1/02 Fixed TCPTick() SYN Retry bug.
 * Nilesh Rajbharti     12/5/02	Modified TCPProcess()
 *                              to include localIP as third param.
 *                              This was done to allow this function
 *                              to calculate checksum correctly.
 * Roy Schofield		10/1/04	TCPConnect() startTick bug fix.
 * Howard Schlunder		1/3/05	Fixed HandleTCPSeg() unexpected 
 * 								discard problem identified by Richard
 *				 				Shelquist.
 * Howard Schlunder		1/16/06	Fixed an imporbable RX checksum bug 
 *								when using a Microchip Ethernet controller)
 * Howard Schlunder		5/10/06	Revised TCP state machine, add TCP_FIN_2
 * Howard Schlunder		8/01/06 Adjusted response to ACK only in TCP_SYN_SENT state
 * Howard Schlunder		8/03/06 Fixed checksum comparison check 
 *								reported by DouglasPunch on Microchip Forum.
 * Howard Schlunder		8/11/06 Fixed a resource leak causing MAC TX 
 *								Buffers to be obtained but not 
 *								released when many web requests were 
 *								received concurrently.
********************************************************************/
#define THIS_IS_TCP

#include <string.h>

#include "..\Include\StackTsk.h"
#include "..\Include\Helpers.h"
#include "..\Include\IP.h"
#include "..\Include\MAC.h"
#include "..\Include\Tick.h"
#include "..\Include\TCP.h"

#if defined(STACK_USE_TCP)

// Max TCP data length is MAC_TX_BUFFER_SIZE - sizeof(TCP_HEADER) -
// sizeof(IP_HEADER) - sizeof(ETHER_HEADER)
#define MAX_TCP_DATA_LEN		(MAC_TX_BUFFER_SIZE - 54)

// TCP Timeout value to begin with.
#define TCP_START_TIMEOUT_VAL   ((TICK)TICK_1S * (TICK)3)

// TCP Flags defined in RFC
#define FIN     (0x01)
#define SYN     (0x02)
#define RST     (0x04)
#define PSH     (0x08)
#define ACK     (0x10)
#define URG     (0x20)

// TCP Header
typedef struct _TCP_HEADER
{
	WORD    SourcePort;
	WORD    DestPort;
	DWORD   SeqNumber;
	DWORD   AckNumber;

	struct
	{
		unsigned char Reserved3      : 4;
		unsigned char Val            : 4;
	} DataOffset;

	union
	{
		struct
		{
			unsigned char flagFIN    : 1;
			unsigned char flagSYN    : 1;
			unsigned char flagRST    : 1;
			unsigned char flagPSH    : 1;
			unsigned char flagACK    : 1;
			unsigned char flagURG    : 1;
			unsigned char Reserved2  : 2;
		} bits;
		BYTE byte;
	} Flags;

	WORD    Window;
	WORD    Checksum;
	WORD    UrgentPointer;
} TCP_HEADER;

// TCP Options as defined by RFC
#define TCP_OPTIONS_END_OF_LIST     (0x00)
#define TCP_OPTIONS_NO_OP           (0x01)
#define TCP_OPTIONS_MAX_SEG_SIZE    (0x02)
typedef struct _TCP_OPTIONS
{
	BYTE        Kind;
	BYTE        Length;
	WORD_VAL    MaxSegSize;
} TCP_OPTIONS;

#define SwapPseudoTCPHeader(h)  (h.TCPLength = swaps(h.TCPLength))
// IP pseudo header as defined by RFC 793
typedef struct _PSEUDO_HEADER
{
	IP_ADDR SourceAddress;
	IP_ADDR DestAddress;
	BYTE Zero;
	BYTE Protocol;
	WORD TCPLength;
} PSEUDO_HEADER;

#define LOCAL_PORT_START_NUMBER (1024)
#define LOCAL_PORT_END_NUMBER   (5000)


// Local temp port numbers.
#ifdef STACK_CLIENT_MODE
static WORD _NextPort = LOCAL_PORT_START_NUMBER;
#endif

// The TCB array is very large.  With the C18 compiler, one must 
// modify the linker script to make an array that spans more than 
// one memory bank.  To do this, make the necessary changes to your 
// processor's linker script (.lkr).  Here is an example showing 
// gpr11 and 128 bytes of gpr12 being combined into one 384 byte 
// block used exclusively by the TCB_MEM data section:
// ...
// //DATABANK   NAME=gpr11      START=0xB00          END=0xBFF
// //DATABANK   NAME=gpr12      START=0xC00          END=0xCFF
// DATABANK   NAME=gpr11b     START=0xB00          END=0xC7F           PROTECTED
// DATABANK   NAME=gpr12      START=0xC80          END=0xCFF
// ...
// SECTION    NAME=TCB_MEM    RAM=gpr11b
// ...
#pragma udata TCB_MEM
SOCKET_INFO TCB[MAX_SOCKETS];
#pragma udata bla	// Return to any other RAM section

static void HandleTCPSeg(TCP_SOCKET s,
						 NODE_INFO *remote,
						 TCP_HEADER *h,
						 WORD len);

static void TransmitTCP(NODE_INFO *remote,
						TCP_PORT localPort,
						TCP_PORT remotePort,
						DWORD seq,
						DWORD ack,
						BYTE flags,
						BUFFER buffer,
						WORD len);

static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h,
									 NODE_INFO *remote);
static void SwapTCPHeader(TCP_HEADER* header);
static void CloseSocket(SOCKET_INFO* ps);

#define SendTCP(remote, localPort, remotePort, seq, ack, flags)     \
	TransmitTCP(remote, localPort, remotePort, seq, ack, flags, \
	INVALID_BUFFER, 0)


/*********************************************************************
* Function:        void TCPInit(void)
*
* PreCondition:    None
*
* Input:           None
*
* Output:          TCP is initialized.
*
* Side Effects:    None
*
* Overview:        Initialize all socket states
*
* Note:            This function is called only once during lifetime
*                  of the application.
********************************************************************/
void TCPInit(void)
{
	TCP_SOCKET s;
	SOCKET_INFO* ps;

	// Initialize all sockets.
	for(s = 0; s < MAX_SOCKETS; s++)
	{
		ps = &TCB[s];

		ps->smState             = TCP_CLOSED;
		ps->Flags.bServer       = FALSE;
		ps->Flags.bIsPutReady   = TRUE;
		ps->Flags.bFirstRead    = TRUE;
		ps->Flags.bIsTxInProgress = FALSE;
		ps->Flags.bIsGetReady   = FALSE;
		if(ps->TxBuffer != INVALID_BUFFER)
		{
			MACDiscardTx(ps->TxBuffer);
			ps->TxBuffer        = INVALID_BUFFER;
		}
		ps->TimeOut             = TCP_START_TIMEOUT_VAL;
		ps->TxCount				= 0;
	}
}



/*********************************************************************
* Function:        TCP_SOCKET TCPListen(TCP_PORT port)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           port    - A TCP port to be opened.
*
* Output:          Given port is opened and returned on success
*                  INVALID_SOCKET if no more sockets left.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
TCP_SOCKET TCPListen(TCP_PORT port)
{
	TCP_SOCKET s;
	SOCKET_INFO* ps;

	for(s = 0; s < MAX_SOCKETS; s++)
	{
		ps = &TCB[s];

		if(ps->smState == TCP_CLOSED)
		{
			// We have a CLOSED socket.
			// Initialize it with LISTENing state info.
			ps->smState             = TCP_LISTEN;
			ps->localPort           = port;
			ps->remotePort          = 0;

			// There is no remote node IP address info yet.
			ps->remote.IPAddr.Val   = 0x00;

			// If a socket is listened on, it is a SERVER.
			ps->Flags.bServer       = TRUE;

			ps->Flags.bIsGetReady   = FALSE;
			if(ps->TxBuffer != INVALID_BUFFER)
			{
				MACDiscardTx(ps->TxBuffer);
				ps->TxBuffer        = INVALID_BUFFER;
			}
			ps->Flags.bIsPutReady   = TRUE;

			return s;
		}
	}
	return INVALID_SOCKET;
}



/*********************************************************************
* Function:        TCP_SOCKET TCPConnect(NODE_INFO* remote,
*                                      TCP_PORT remotePort)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           remote      - Remote node address info
*                  remotePort  - remote port to be connected.
*
* Output:          A new socket is created, connection request is
*                  sent and socket handle is returned.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            By default this function is not included in
*                  source.  You must define STACK_CLIENT_MODE to
*                  be able to use this function.
********************************************************************/
#ifdef STACK_CLIENT_MODE
TCP_SOCKET TCPConnect(NODE_INFO *remote, TCP_PORT remotePort)
{
	TCP_SOCKET s;
	SOCKET_INFO* ps;
	BOOL lbFound;


	lbFound = FALSE;

	// Find an available socket
	for(s = 0; s < MAX_SOCKETS; s++)
	{
		ps = &TCB[s];
		if(ps->smState == TCP_CLOSED)
		{
			lbFound = TRUE;
			break;
		}
	}

	// If there is no socket available, return error.
	if(!lbFound)
		return INVALID_SOCKET;

	// Each new socket that is opened by this node, gets
	// next sequential port number.
	ps->localPort = ++_NextPort;
	if(_NextPort >= LOCAL_PORT_END_NUMBER)
		_NextPort = LOCAL_PORT_START_NUMBER-1;

	// This is a client socket.
	ps->Flags.bServer = FALSE;

	// This is the port, we are trying to connect to.
	ps->remotePort = remotePort;

	// Each new socket that is opened by this node, will
	// start with next the next seqeuence number (essentially random)
	ps->SND_SEQ++;
	ps->SND_ACK = 0;

	memcpy((BYTE*)&ps->remote, (const void*)remote, sizeof(ps->remote));

	// Send SYN message.
	SendTCP(&ps->remote,
		ps->localPort,
		ps->remotePort,
		ps->SND_SEQ,
		ps->SND_ACK,
		SYN);

	ps->smState = TCP_SYN_SENT;
	ps->SND_SEQ++;

	// Allow TCPTick() to operate properly
	ps->startTick = tickGet(); 	

	return s;
}
#endif



/*********************************************************************
* Function:        BOOL TCPIsConnected(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           s       - Socket to be checked for connection.
*
* Output:          TRUE    if given socket is connected
*                  FALSE   if given socket is not connected.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            A socket is said to be connected if it is not
*                  in LISTEN and CLOSED mode.  Socket may be in
*                  SYN_RCVD or FIN_WAIT_1 and may contain socket
*                  data.
********************************************************************/
BOOL TCPIsConnected(TCP_SOCKET s)
{
	return (TCB[s].smState == TCP_ESTABLISHED);
}



/*********************************************************************
* Function:        void TCPDisconnect(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCPIsPutReady(s) == TRUE
*
* Input:           s       - Socket to be disconnected.
*
* Output:          A disconnect request is sent for given socket.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
void TCPDisconnect(TCP_SOCKET s)
{
	SOCKET_INFO *ps;

	ps = &TCB[s];

	// If socket is not connected, may be it is already closed
	// or in the process of closing.  Since we have called this
	// explicitly, close it forcefully.
	if(ps->smState != TCP_ESTABLISHED && ps->smState != TCP_SYN_RECEIVED)
	{
		CloseSocket(ps);
		return;
	}

	// Discard any outstanding data that is to be read.
	TCPDiscard(s);

	// Send FIN message.
	SendTCP(&ps->remote,
		ps->localPort,
		ps->remotePort,
		ps->SND_SEQ,
		ps->SND_ACK,
		FIN | ACK);
//	DebugPrint(".");

	ps->SND_SEQ++;

	ps->smState = TCP_FIN_WAIT_1;

	return;
}

/*********************************************************************
* Function:        BOOL TCPFlush(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           s       - Socket whose data is to be transmitted.
*
* Output:          All and any data associated with this socket
*                  is marked as ready for transmission.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPFlush(TCP_SOCKET s)
{
	SOCKET_INFO *ps;

	ps = &TCB[s];

	// Make sure that there is TxBuffer assigned to this socket.
	if ( ps->TxBuffer == INVALID_BUFFER )
		return FALSE;

	if ( ps->Flags.bIsPutReady == FALSE )
		return FALSE;

	TransmitTCP(&ps->remote,
		ps->localPort,
		ps->remotePort,
		ps->SND_SEQ,
		ps->SND_ACK,
		ACK + PSH,		// Use PSH to make sure the end application receives the data right away
		ps->TxBuffer,
		ps->TxCount);
	ps->SND_SEQ += (DWORD)ps->TxCount;
	ps->Flags.bIsPutReady       = FALSE;
	ps->Flags.bIsTxInProgress   = FALSE;

#ifdef TCP_NO_WAIT_FOR_ACK
	if(ps->TxBuffer != INVALID_BUFFER)
	{
		MACDiscardTx(ps->TxBuffer);
		ps->TxBuffer        = INVALID_BUFFER;
	}
	ps->Flags.bIsPutReady       = TRUE;
#endif

	return TRUE;
}



/*********************************************************************
* Function:        BOOL TCPIsPutReady(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           s       - socket to test
*
* Output:          TRUE if socket 's' is free to transmit
*                  FALSE if socket 's' is not free to transmit.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            Each socket maintains only transmit buffer.
*                  Hence until a data packet is acknowledeged by
*                  remote node, socket will not be ready for
*                  next transmission.
*                  All control transmission such as Connect,
*                  Disconnect do not consume/reserve any transmit
*                  buffer.
********************************************************************/
BOOL TCPIsPutReady(TCP_SOCKET s)
{
	if(TCB[s].RemoteWindow == 0)
		return FALSE;

	if ( TCB[s].TxBuffer == INVALID_BUFFER )
		return IPIsTxReady(FALSE);
	else
		return TCB[s].Flags.bIsPutReady;
}




/*********************************************************************
* Function:        BOOL TCPPut(TCP_SOCKET s, BYTE byte)
*
* PreCondition:    TCPIsPutReady() == TRUE
*
* Input:           s       - socket to use
*                  byte    - a data byte to send
*
* Output:          TRUE if given byte was put in transmit buffer
*                  FALSE if transmit buffer is full.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPPut(TCP_SOCKET s, BYTE byte)
{
	SOCKET_INFO* ps;

	ps = &TCB[s];

	// Make sure that the remote node is able to accept our data
	if(ps->RemoteWindow == 0)
		return FALSE;

	if(ps->TxBuffer == INVALID_BUFFER)
	{
		ps->TxBuffer = MACGetTxBuffer(FALSE);

		// Check to make sure that we received a TX Buffer
		if(ps->TxBuffer == INVALID_BUFFER)
			return FALSE;

		ps->TxCount = 0;

		IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
	}

	ps->Flags.bIsTxInProgress = TRUE;

	MACPut(byte);
	ps->RemoteWindow--;

	if(++ps->TxCount >= MAX_TCP_DATA_LEN)
		TCPFlush(s);

	return TRUE;
}

/*********************************************************************
* Function:        BOOL TCPDiscard(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           s       - socket
*
* Output:          TRUE if socket received data was discarded
*                  FALSE if socket received data was already
*                          discarded.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPDiscard(TCP_SOCKET s)
{
	SOCKET_INFO* ps;

	ps = &TCB[s];

	// This socket must contain data for it to be discarded.
	if(!ps->Flags.bIsGetReady)
		return FALSE;

	MACDiscardRx();
	ps->Flags.bIsGetReady = FALSE;

	return TRUE;
}




/*********************************************************************
* Function:        WORD TCPGetArray(TCP_SOCKET s, BYTE *buffer,
*                                      WORD count)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCPIsGetReady(s) == TRUE
*
* Input:           s       - socket
*                  buffer  - Buffer to hold received data.
*                  count   - Buffer length
*
* Output:          Number of bytes loaded into buffer.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
WORD TCPGetArray(TCP_SOCKET s, BYTE *buffer, WORD count)
{
	SOCKET_INFO *ps;

	ps = &TCB[s];

	if ( ps->Flags.bIsGetReady )
	{
		if ( ps->Flags.bFirstRead )
		{
			// Position read pointer to begining of TCP data
			IPSetRxBuffer(sizeof(TCP_HEADER));

			ps->Flags.bFirstRead = FALSE;
		}

		ps->Flags.bIsTxInProgress = TRUE;

		return MACGetArray(buffer, count);
	}
	else
		return 0;
}



/*********************************************************************
* Function:        BOOL TCPGet(TCP_SOCKET s, BYTE *byte)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCPIsGetReady(s) == TRUE
*
* Input:           s       - socket
*                  byte    - Pointer to a byte.
*
* Output:          TRUE if a byte was read.
*                  FALSE if byte was not read.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPGet(TCP_SOCKET s, BYTE *byte)
{
	SOCKET_INFO* ps;

	ps = &TCB[s];

	if ( ps->Flags.bIsGetReady )
	{
		if ( ps->Flags.bFirstRead )
		{
			// Position read pointer to begining of correct
			// buffer.
			IPSetRxBuffer(sizeof(TCP_HEADER));

			ps->Flags.bFirstRead = FALSE;

		}

		if ( ps->RxCount == 0 )
		{
			MACDiscardRx();
			ps->Flags.bIsGetReady = FALSE;
			return FALSE;
		}

		ps->RxCount--;
		*byte = MACGet();
		return TRUE;
	}
	return FALSE;
}



/*********************************************************************
* Function:        BOOL TCPIsGetReady(TCP_SOCKET s)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           s       - socket to test
*
* Output:          TRUE if socket 's' contains any data.
*                  FALSE if socket 's' does not contain any data.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPIsGetReady(TCP_SOCKET s)
{
	/*
	* A socket is said to be "Get" ready when it has already
	* received some data.  Sometime, a socket may be closed,
	* but it still may contain data.  Thus in order to ensure
	* reuse of a socket, caller must make sure that it reads
	* a socket, if is ready.
	*/
	return(TCB[s].Flags.bIsGetReady);
}



/*********************************************************************
* Function:        void TCPTick(void)
*
* PreCondition:    TCPInit() is already called.
*
* Input:           None
*
* Output:          Each socket FSM is executed for any timeout
*                  situation.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
void TCPTick(void)
{
	TCP_SOCKET s;
	TICK diffTicks;
	TICK tick;
	SOCKET_INFO* ps;
	DWORD seq;
	BYTE flags;

	flags = 0x00;
	// Periodically all "not closed" sockets must perform timed operations
	for(s = 0; s < MAX_SOCKETS; s++)
	{
		ps = &TCB[s];

		if ( ps->Flags.bIsGetReady || ps->Flags.bIsTxInProgress )
			continue;


		// Closed or Passively Listening socket do not care
		// about timeout conditions.
		if ( (ps->smState == TCP_CLOSED) ||
			(ps->smState == TCP_LISTEN &&
			ps->Flags.bServer == TRUE) )
			continue;

		tick = tickGet();

		// Calculate timeout value for this socket.
		diffTicks = tick-ps->startTick;

		// If timeout has not occured, do not do anything.
		if(diffTicks <= ps->TimeOut)
			continue;

		// Most states require retransmission, so check for transmitter
		// availability right here - common for all.
		if(!IPIsTxReady(TRUE))
			return;

		// Restart timeout reference.
		ps->startTick = tickGet();

		// Update timeout value if there is need to wait longer.
		ps->TimeOut <<= 1;

		// This will be one more attempt.
		ps->RetryCount++;

		// A timeout has occured.  Respond to this timeout condition
		// depending on what state this socket is in.
		switch(ps->smState)
		{
		case TCP_SYN_SENT:
			// Keep sending SYN until we hear from remote node.
			// This may be for infinite time, in that case
			// caller must detect it and do something.
			// Bug Fix: 11/1/02
			flags = SYN;
			break;

		case TCP_SYN_RECEIVED:
			// We must receive ACK before timeout expires.
			// If not, resend SYN+ACK.
			// Abort, if maximum attempts counts are reached.
			if(ps->RetryCount <= MAX_RETRY_COUNTS)
			{
				flags = SYN | ACK;
			}
			else
			{
				if(ps->Flags.bServer)
				{
					ps->smState = TCP_LISTEN;
				}
				else
				{
					flags = SYN;
					ps->smState = TCP_SYN_SENT;
				}
			}
			break;

		case TCP_ESTABLISHED:
#if !defined(TCP_NO_WAIT_FOR_ACK)
			// Don't let this connection idle for very long time.
			// If we did not receive or send any message before timeout
			// expires, close this connection.
			if(ps->RetryCount <= MAX_RETRY_COUNTS)
			{
				if(ps->TxBuffer != INVALID_BUFFER)
				{
					MACSetTxBuffer(ps->TxBuffer, 0);
					MACFlush();
				}
				else
					flags = ACK;
			}
			else
			{
				// Forget about previous transmission.
				if(ps->TxBuffer != INVALID_BUFFER)
				{
					MACDiscardTx(ps->TxBuffer);
					ps->TxBuffer = INVALID_BUFFER;
				}

#endif
				// Request closure.
				flags = FIN | ACK;
//				DebugPrint("!");

				ps->smState = TCP_FIN_WAIT_1;
#if !defined(TCP_NO_WAIT_FOR_ACK)
			}
#endif
			break;

		case TCP_FIN_WAIT_1:
			if(ps->RetryCount <= MAX_RETRY_COUNTS)
			{
					// Send another FIN
					flags = FIN;
			}
			else
			{
				// Close on our own, we can't seem to communicate 
				// with the remote node anymore
				CloseSocket(ps);
			}
			break;

		case TCP_FIN_WAIT_2:
		case TCP_CLOSING:
			// Close on our own, we can't seem to communicate 
			// with the remote node anymore
			CloseSocket(ps);
			break;

		case TCP_TIME_WAIT:
			// Wait around for a while (2MSL) and then goto closed state
			CloseSocket(ps);
			break;
		
		case TCP_CLOSE_WAIT:
			flags = FIN;
			ps->smState = TCP_LAST_ACK;
			break;

		case TCP_LAST_ACK:
			// Send some more FINs or close anyway
			if(ps->RetryCount <= MAX_RETRY_COUNTS)
				flags = FIN;
			else
				CloseSocket(ps);
			break;
		}


		if(flags)
		{
			if(flags & ACK)
				seq = ps->SND_SEQ;
			else
				seq = ps->SND_SEQ++;

			SendTCP(&ps->remote,
				ps->localPort,
				ps->remotePort,
				seq,
				ps->SND_ACK,
				flags);
		}
	}
}



/*********************************************************************
* Function:        BOOL TCPProcess(NODE_INFO* remote,
*                                  IP_ADDR *localIP,
*                                  WORD len)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCP segment is ready in MAC buffer
*
* Input:           remote      - Remote node info
*                  len         - Total length of TCP semgent.
*
* Output:          TRUE if this function has completed its task
*                  FALSE otherwise
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
BOOL TCPProcess(NODE_INFO *remote, IP_ADDR *localIP, WORD len)
{
	TCP_HEADER      TCPHeader;
	PSEUDO_HEADER   pseudoHeader;
	TCP_SOCKET      socket;
	WORD_VAL        checksum1;
	WORD_VAL        checksum2;
	BYTE            optionsSize;

	// Calculate IP pseudoheader checksum.
	pseudoHeader.SourceAddress      = remote->IPAddr;
	pseudoHeader.DestAddress        = *localIP;
	pseudoHeader.Zero               = 0x0;
	pseudoHeader.Protocol           = IP_PROT_TCP;
	pseudoHeader.TCPLength          = len;

	SwapPseudoTCPHeader(pseudoHeader);

	checksum1.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
		sizeof(pseudoHeader));


	// Now calculate TCP packet checksum in NIC RAM - should match
	// pesudo header checksum
	checksum2.Val = CalcIPBufferChecksum(len);

	// Compare checksums.  Note that the endianness is different.
	if(checksum1.v[0] != checksum2.v[1] || checksum1.v[1] != checksum2.v[0])
	{
		MACDiscardRx();
		return TRUE;
	}

	// Retrieve TCP header.
	IPSetRxBuffer(0);
	MACGetArray((BYTE*)&TCPHeader, sizeof(TCPHeader));
	SwapTCPHeader(&TCPHeader);


	// Skip over options and retrieve all data bytes.
	optionsSize = (BYTE)((TCPHeader.DataOffset.Val << 2)-
		sizeof(TCPHeader));
	len = len - optionsSize - sizeof(TCPHeader);

	// Position packet read pointer to start of data area.
	IPSetRxBuffer((TCPHeader.DataOffset.Val << 2));

	// Find matching socket.
	socket = FindMatchingSocket(&TCPHeader, remote);
	if(socket != INVALID_SOCKET)
	{
		HandleTCPSeg(socket, remote, &TCPHeader, len);
	}
	else
	{
		// If this is an unknown socket, or we don't have any 
		// listening sockets available, discard it we can't 
		// process it right now
		MACDiscardRx();
		
//		// Send a RESET to the remote node is it knows that we 
//		// are not available
//		TCPHeader.AckNumber += len;
//		if( TCPHeader.Flags.bits.flagSYN ||
//			TCPHeader.Flags.bits.flagFIN )
//			TCPHeader.AckNumber++;
//		
//		SendTCP(remote,
//			TCPHeader.DestPort,
//			TCPHeader.SourcePort,
//			TCPHeader.AckNumber,
//			TCPHeader.SeqNumber,
//			RST);
	}

	return TRUE;
}


/*********************************************************************
* Function:        static void TransmitTCP(NODE_INFO* remote
*                                          TCP_PORT localPort,
*                                          TCP_PORT remotePort,
*                                          DWORD seq,
*                                          DWORD ack,
*                                          BYTE flags,
*                                          BUFFER buffer,
*                                          WORD len)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCPIsPutReady() == TRUE
*
* Input:           remote      - Remote node info
*                  localPort   - Source port number
*                  remotePort  - Destination port number
*                  seq         - Segment sequence number
*                  ack         - Segment acknowledge number
*                  flags       - Segment flags
*                  buffer      - Buffer to which this segment
*                                is to be transmitted
*                  len         - Total data length for this segment.
*
* Output:          A TCP segment is assembled and put to transmit.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
static void TransmitTCP(NODE_INFO *remote,
						TCP_PORT localPort,
						TCP_PORT remotePort,
						DWORD tseq,
						DWORD tack,
						BYTE flags,
						BUFFER buffer,
						WORD len)
{
	WORD_VAL        checkSum;
	TCP_HEADER      header;
	TCP_OPTIONS     options;
	PSEUDO_HEADER   pseudoHeader;

	//  Make sure that this Tx buffer isn't currently being transmitted
	while( !IPIsTxReady(TRUE) );	//TODO: This may need to be conditionally false

	// Obtain an AutoFree buffer if this packet is a control packet 
	// only (contains no application data in an already allocated 
	// buffer)
	if(buffer == INVALID_BUFFER)
		buffer = MACGetTxBuffer(TRUE);

	if(buffer == INVALID_BUFFER)
		return;

	IPSetTxBuffer(buffer, 0);

	header.SourcePort           = localPort;
	header.DestPort             = remotePort;
	header.SeqNumber            = tseq;
	header.AckNumber            = tack;
	header.Flags.bits.Reserved2 = 0;
	header.DataOffset.Reserved3 = 0;
	header.Flags.byte           = flags;
	// Receive window = MAC Free buffer size - TCP header (20) - IP header (20)
	//                  - ETHERNET header (14 if using NIC) .
	header.Window               = MACGetFreeRxSize();
#if !defined(STACK_USE_SLIP)
	/*
	* Limit one segment at a time from remote host.
	* This limit increases overall throughput as remote host does not
	* flood us with packets and later retry with significant delay.
	*/
	if ( header.Window >= MAC_RX_BUFFER_SIZE )
		header.Window = MAC_RX_BUFFER_SIZE;

	else if ( header.Window > 54 )
	{
		header.Window -= 54;
	}
	else
		header.Window = 0;
#else
	if ( header.Window > 40 )
	{
		header.Window -= 40;
	}
	else
		header.Window = 0;
#endif

	header.Checksum             = 0;
	header.UrgentPointer        = 0;

	SwapTCPHeader(&header);

	len += sizeof(header);

	if ( flags & SYN )
	{
		len += sizeof(options);
		options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
		options.Length = 0x04;

		// Load MSS in already swapped order.
		options.MaxSegSize.v[0]  = (MAC_RX_BUFFER_SIZE >> 8); // 0x05;
		options.MaxSegSize.v[1]  = (MAC_RX_BUFFER_SIZE & 0xff); // 0xb4;

		header.DataOffset.Val   = (sizeof(header) + sizeof(options)) >> 2;
	}
	else
		header.DataOffset.Val   = sizeof(header) >> 2;


	// Calculate IP pseudoheader checksum.
	pseudoHeader.SourceAddress	= AppConfig.MyIPAddr;
	pseudoHeader.DestAddress    = remote->IPAddr;
	pseudoHeader.Zero           = 0x0;
	pseudoHeader.Protocol       = IP_PROT_TCP;
	pseudoHeader.TCPLength      = len;

	SwapPseudoTCPHeader(pseudoHeader);

	header.Checksum = ~CalcIPChecksum((BYTE*)&pseudoHeader,
		sizeof(pseudoHeader));
	checkSum.Val = header.Checksum;

	// Write IP header.
	IPPutHeader(remote, IP_PROT_TCP, len);
	IPPutArray((BYTE*)&header, sizeof(header));

	if ( flags & SYN )
		IPPutArray((BYTE*)&options, sizeof(options));

	IPSetTxBuffer(buffer, 0);

	checkSum.Val = CalcIPBufferChecksum(len);

	// Update the checksum.
	IPSetTxBuffer(buffer, 16);
	MACPut(checkSum.v[1]);
	MACPut(checkSum.v[0]);
	MACSetTxBuffer(buffer, 0);

	MACFlush();

#if !defined(TCP_NO_WAIT_FOR_ACK) && !defined(DEBUG)
	// If we send the packet again, the remote node might think that we timed 
	// out and retransmitted.  It could thus immediately send back an ACK and 
	// dramatically improve throuput.
	while(!IPIsTxReady(TRUE));
	MACFlush();
#endif
}



/*********************************************************************
* Function:        static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h,
*                                      NODE_INFO* remote)
*
* PreCondition:    TCPInit() is already called
*
* Input:           h           - TCP Header to be matched against.
*                  remote      - Node who sent this header.
*
* Output:          A socket that matches with given header and remote
*                  node is searched.
*                  If such socket is found, its index is returned
*                  else INVALID_SOCKET is returned.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h, NODE_INFO *remote)
{
	SOCKET_INFO *ps;
	TCP_SOCKET s;
	TCP_SOCKET partialMatch;

	partialMatch = INVALID_SOCKET;

	for ( s = 0; s < MAX_SOCKETS; s++ )
	{
		ps = &TCB[s];

		if ( ps->smState != TCP_CLOSED )
		{
			if ( ps->localPort == h->DestPort )
			{
				if ( ps->smState == TCP_LISTEN )
					partialMatch = s;

				if ( ps->remotePort == h->SourcePort &&
					ps->remote.IPAddr.Val == remote->IPAddr.Val )
				{
					return s;
				}
			}
		}
	}

	// We are not listening on this port
	if(partialMatch == INVALID_SOCKET)
		return INVALID_SOCKET;

	// Copy the remote node IP/MAC address and source TCP port 
	// number into our TCB and return this socket to the caller
	ps = &TCB[partialMatch];
	memcpy((void*)&ps->remote, (void*)remote, sizeof(*remote));
	ps->remotePort          = h->SourcePort;
	ps->Flags.bIsGetReady   = FALSE;
	if(ps->TxBuffer != INVALID_BUFFER)
	{
		MACDiscardTx(ps->TxBuffer);
		ps->TxBuffer        = INVALID_BUFFER;
	}
	ps->Flags.bIsPutReady   = TRUE;
	
	return partialMatch;
}






/*********************************************************************
* Function:        static void SwapTCPHeader(TCP_HEADER* header)
*
* PreCondition:    None
*
* Input:           header      - TCP Header to be swapped.
*
* Output:          Given header is swapped.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
static void SwapTCPHeader(TCP_HEADER* header)
{
	header->SourcePort      = swaps(header->SourcePort);
	header->DestPort        = swaps(header->DestPort);
	header->SeqNumber       = swapl(header->SeqNumber);
	header->AckNumber       = swapl(header->AckNumber);
	header->Window          = swaps(header->Window);
	header->Checksum        = swaps(header->Checksum);
	header->UrgentPointer   = swaps(header->UrgentPointer);
}



/*********************************************************************
* Function:        static void CloseSocket(SOCKET_INFO* ps)
*
* PreCondition:    TCPInit() is already called
*
* Input:           ps  - Pointer to a socket info that is to be
*                          closed.
*
* Output:          Given socket information is reset and any
*                  buffer held by this socket is discarded.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
static void CloseSocket(SOCKET_INFO* ps)
{
	if(ps->TxBuffer != INVALID_BUFFER)
	{
		MACDiscardTx(ps->TxBuffer);
		ps->TxBuffer            = INVALID_BUFFER;
		ps->Flags.bIsPutReady   = TRUE;
	}

	ps->remote.IPAddr.Val = 0x00;
	ps->remotePort = 0x00;
	if(ps->Flags.bIsGetReady)
	{
		MACDiscardRx();
	}
	ps->Flags.bIsGetReady       = FALSE;
	ps->TimeOut                 = TCP_START_TIMEOUT_VAL;

	ps->Flags.bIsTxInProgress   = FALSE;

	if(ps->Flags.bServer)
	{
		ps->smState = TCP_LISTEN;
	}
	else
	{
		ps->smState = TCP_CLOSED;
	}

	ps->TxCount = 0;

	return;
}



/*********************************************************************
* Function:        static void HandleTCPSeg(TCP_SOCKET s,
*                                      NODE_INFO *remote,
*                                      TCP_HEADER* h,
*                                      WORD len)
*
* PreCondition:    TCPInit() is already called     AND
*                  TCPProcess() is the caller.
*
* Input:           s           - Socket that owns this segment
*                  remote      - Remote node info
*                  h           - TCP Header
*                  len         - Total buffer length.
*
* Output:          TCP FSM is executed on given socket with
*                  given TCP segment.
*
* Side Effects:    None
*
* Overview:        None
*
* Note:            None
********************************************************************/
static void HandleTCPSeg(TCP_SOCKET s,
						 NODE_INFO *remote,
						 TCP_HEADER *h,
						 WORD len)
{
	DWORD ack;
	DWORD seq;
	DWORD prevAck, prevSeq;
	SOCKET_INFO *ps;
	BYTE flags;

	ps = &TCB[s];

	flags = 0x00;

	// Clear timeout info
	ps->RetryCount  = 0;
	ps->startTick   = tickGet();
	ps->TimeOut = TCP_START_TIMEOUT_VAL;

	// Reset FSM, if RST is received.
	if(h->Flags.bits.flagRST)
	{
		MACDiscardRx();
		ps->smState = ps->Flags.bServer ? TCP_LISTEN : TCP_SYN_SENT;
		return;
	}

	seq = ps->SND_SEQ;
	
	// ack is just a temporary variable
	ack = h->Window - (seq - h->AckNumber) - ps->TxCount;
	if((signed long)ack < 0)
		ps->RemoteWindow = 0;
	else
		ps->RemoteWindow = ack;


#ifdef STACK_CLIENT_MODE
	// Handle TCP_SYN_SENT state
	// The TCP_SYN_SENT state occurs when an application 
	// calls TCPConnect().  After an initial SYN is sent,
	// we expect a SYN + ACK before establishing the 
	// connection.
	if(ps->smState == TCP_SYN_SENT)
	{
		// Check if this is a SYN packet.  Unsynchronized, we cannot
		// handle any other packet types.
		if(!h->Flags.bits.flagSYN)
		{
			MACDiscardRx();

			// Send out a RESET if the remote node thinks a connection is already established
		    if(h->Flags.bits.flagACK)
		    {
				flags = RST;
				goto SendTCPControlPacket;
	        }

			return;
		}

		// We now have a sequence number for the remote node
		ps->SND_ACK = h->SeqNumber + len + 1;
		ack = ps->SND_ACK;

		// If there is no ACK, we must go to TCP_SYN_RECEIVED.  With an ACK, 
		// we can establish the connection now.
		if(!h->Flags.bits.flagACK)
		{
			ps->smState = TCP_SYN_RECEIVED;
			MACDiscardRx();
			// Send out a SYN+ACK for simultaneous connection open
			flags = SYN | ACK;
			goto SendTCPControlPacket;
		}

		// We received SYN+ACK, establish the connection now
		ps->smState = TCP_ESTABLISHED;
		// Send out an ACK
		flags = ACK;

		ps->RemoteWindow = h->Window;

		// Check for application data and make it 
		// available, if present
		if(len)
		{
			ps->Flags.bIsGetReady   = TRUE;
			ps->RxCount             = len;
			ps->Flags.bFirstRead    = TRUE;
		}
		else	// No application data in this packet
		{
			MACDiscardRx();
		}
		goto SendTCPControlPacket;
	}
#endif

	// Handle TCP_LISTEN state
	if(ps->smState == TCP_LISTEN )
	{
		MACDiscardRx();

		// Send a RST if this isn't a SYN packet
		if(!h->Flags.bits.flagSYN)
		{
			flags = RST;
			goto SendTCPControlPacket;
		}

		ps->SND_ACK = h->SeqNumber + len + 1;
		ps->RemoteWindow = h->Window;

		// This socket has received connection request (SYN).
		// Remember calling node, assign next segment seq. number
		// for this potential connection.
		memcpy((void*)&ps->remote, (const void*)remote, sizeof(*remote));
		ps->remotePort = h->SourcePort;

		// Grant connection request.
		ps->smState = TCP_SYN_RECEIVED;
		seq = ps->SND_SEQ++;
		ack =  ps->SND_ACK;
		flags = SYN | ACK;
		goto SendTCPControlPacket;
	}


	// Remember current seq and ack for our connection so that if
	// we have to silently discard this packet, we can go back to
	// previous ack and seq numbers.
	prevAck = ps->SND_ACK;
	prevSeq = ps->SND_SEQ;

	ack = h->SeqNumber;
	ack += (DWORD)len;
	seq = ps->SND_SEQ;

	// State is something other than TCP_LISTEN, handle it.
	{
		// Check to see if the incomming sequence number is what 
		// we expect (last transmitted ACK value).  Throw this packet 
		// away if it is wrong.
		if(h->SeqNumber == prevAck)
		{
			// After receiving a SYNchronization request, we expect an 
			// ACK to our transmitted SYN
			if(ps->smState == TCP_SYN_RECEIVED)
			{
				if(h->Flags.bits.flagACK)
				{
					// ACK received as expected, this connection is 
					// now established
					ps->SND_ACK = ack;
					ps->smState = TCP_ESTABLISHED;

					// Check if this first packet has application data 
					// in it.  Make it available if so.
					if(len)
					{
						ps->Flags.bIsGetReady   = TRUE;
						ps->RxCount             = len;
						ps->Flags.bFirstRead    = TRUE;
					}
					else
						MACDiscardRx();
				}
				else	// No ACK to our SYN
				{
					MACDiscardRx();
				}
			}
			// Connection is established, closing, or otherwise
			else
			{

				// Save the seq+len value of the packet for our future 
				// ACK transmission, and so out of sequence packets 
				// can be detected in the future.
				ps->SND_ACK = ack;

				// Handle packets received while connection established.
				if(ps->smState == TCP_ESTABLISHED)
				{
					// If this packet has the ACK set, mark all 
					// previous TX packets as no longer needed for 
					// possible retransmission.
					// TODO: Make this more sophisticated so that partial ACKs due to fragmentation are handled correctly.  i.e. Keep a real output stream buffer with slidable window capability.
					if(h->Flags.bits.flagACK && !ps->Flags.bIsPutReady)
					{
						if(ps->TxBuffer != INVALID_BUFFER)
						{
							MACDiscardTx(ps->TxBuffer);
							ps->TxBuffer            = INVALID_BUFFER;
							ps->Flags.bIsPutReady   = TRUE;
						}
					}

					// Check if the remote node is closing the connection
					if(h->Flags.bits.flagFIN)
					{
//						DebugPrint("|");
						flags = FIN | ACK;
						seq = ps->SND_SEQ++;
						ack = ++ps->SND_ACK;
						ps->smState = TCP_LAST_ACK;
					}

					// Check if there is any application data in 
					// this packet.
					if(len)
					{
						// There is data.  Make it available if we 
						// don't already have data available.
						if(!ps->Flags.bIsGetReady)
						{
							ps->Flags.bIsGetReady   = TRUE;
							ps->RxCount             = len;
							ps->Flags.bFirstRead    = TRUE;

							// 4/1/02
							flags |= ACK;
						}
						// There is data, but we cannot handle it at this time.
						else
						{
//							DebugPrint("D");
							// Since we cannot accept this packet,
							// restore to previous seq and ack.
							// and do not send anything back.
							// Host has to resend this packet when
							// we are ready.
							ps->SND_SEQ = prevSeq;
							ps->SND_ACK = prevAck;

							MACDiscardRx();
						}
					}
					// There is no data in this packet, and thus it 
					// can be thrown away.
					else
					{
						MACDiscardRx();
					}
				}
				// Connection is not established; check if we've sent 
				// a FIN and expect our last ACK
				else if(ps->smState == TCP_LAST_ACK)
				{
					MACDiscardRx();

					if(h->Flags.bits.flagACK)
					{
						CloseSocket(ps);
					}
				}
				else if(ps->smState == TCP_FIN_WAIT_1)
				{
					MACDiscardRx();

					if(h->Flags.bits.flagFIN)
					{
						flags = ACK;
						ack = ++ps->SND_ACK;
						if(h->Flags.bits.flagACK)
						{
							CloseSocket(ps);
						}
						else
						{
							ps->smState = TCP_CLOSING;
						}
					}
					else if(h->Flags.bits.flagACK)
					{
						ps->smState = TCP_FIN_WAIT_2;
					}
				}
				else if(ps->smState == TCP_FIN_WAIT_2)
				{
					MACDiscardRx();

					if(h->Flags.bits.flagFIN)
					{
						flags = ACK;
						ack = ++ps->SND_ACK;
						CloseSocket(ps);
					}
				}
				else if ( ps->smState == TCP_CLOSING )
				{
					MACDiscardRx();

					if ( h->Flags.bits.flagACK )
					{
						CloseSocket(ps);
					}
				}
			}
		}
		// This packet's sequence number does not match what we were 
		// expecting (the last value we ACKed).  Throw this packet 
		// away.  This may happen if packets are delivered out of order.
		// Not enough memory is available on our PIC or Ethernet 
		// controller to implement a robust stream reconstruction 
		// buffer.  As a result, the remote node will just have to 
		// retransmit its packets starting with the proper sequence number.
		else
		{
			MACDiscardRx();

			// Send a new ACK out in case if the previous one was lost 
			// (ACKs aren't ACKed).  This is required to prevent an 
			// unlikely but possible situation which would cause the 
			// connection to time out if the ACK was lost and the 
			// remote node keeps sending us older data than we are 
			// expecting.
			flags = ACK;	
			ack = prevAck;
		}
	}

SendTCPControlPacket:
	if(flags)
	{
		SendTCP(remote,
			h->DestPort,
			h->SourcePort,
			seq,
			ack,
			flags);
	}
}


#endif //#if defined(STACK_USE_TCP)
