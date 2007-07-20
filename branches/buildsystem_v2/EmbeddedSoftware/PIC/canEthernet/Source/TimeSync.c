#define THIS_IS_TIMESYNC

#include "..\Include\DNS.h"
#include "..\Include\Helpers.h"
#include "..\Include\TCP.h"
#include "..\Include\ARPTsk.h"
#include "..\Include\ARP.h"
#include "..\Include\UART.h"
#include "..\Include\StackTsk.h"
#include "..\Include\TimeSync.h"

#if defined(STACK_USE_TIMESYNC)

	typedef enum _timeState
	{
		SM_START=0,
		SM_ARP_RESOLVE,
		SM_ARP_RESOLVED,
		SM_CONNECT,
		SM_CONNECT_WAIT,
		SM_CONNECTED,
		SM_RECEIVE,
		SM_DISCONNECT,
		SM_ABORT,
		SM_DONE

	} timeState;

timeState smTS = SM_START;

DWORD lastSync = 0;
DWORD timeNow = 0;

//#define TIMESYNC_DEBUG

void timeSync(void)
{
	BYTE i;
	signed char j;	
	static TICK			Timer;
	static TICK perodicTick = 0;
	static TICK t = 0;
	static TCP_SOCKET	MySocket = INVALID_SOCKET;
	static NODE_INFO	Server;
	static int arp_tries = 0;
	static int tcp_tries = 0;

	BYTE rcnt=0;
	BYTE ncnt=0;
	char foundData=0;

	if ((tickGet()-t) >= TICK_1S )
    {
		t = tickGet();
		timeNow++;
	}

	switch(smTS)
	{
		case SM_START:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Start!\r\n");
			#endif
			// Set IP adress to connect to.
			Server.IPAddr.v[0]=193;
			Server.IPAddr.v[1]=11;
			Server.IPAddr.v[2]=249;
			Server.IPAddr.v[3]=54;

			arp_tries = 0;
			tcp_tries = 0;

			smTS = SM_ARP_RESOLVE;
		break;
		
		case SM_ARP_RESOLVE:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Resolve..\r\n");
			#endif
			// If ARP is redy..
			if (ARPIsTxReady())
			{
				// Resolve the IP adress..
				ARPResolve(&Server.IPAddr);
				arp_tries++;
				Timer = tickGet();
				smTS = SM_ARP_RESOLVED;
			}
		break;

		case SM_ARP_RESOLVED:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Resolved..\r\n");
			#endif
			// If IP adress is resolved, go to next state
			if (ARPIsResolved(&Server.IPAddr, &Server.MACAddr))
			{
				smTS = SM_CONNECT;
			}
			// If not resolved and spent long time here,
			// Go back to previous state and re-resolve.
			else if (tickGet()-Timer > 1*TICK_1S)
			{
				smTS = SM_ARP_RESOLVE;
			}
			else if (arp_tries>=MAX_ARP_TRIES)
			{
				//Abort
				smTS = SM_ABORT;
			}

		break;

		case SM_CONNECT:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Connect..\r\n");
			#endif
			// We have an sucessfull ARP, connect..
			MySocket = TCPConnect(&Server, ServerPort);
			tcp_tries++;	

			if(MySocket == INVALID_SOCKET)
			{
				// Do something.
			}
		
			Timer = tickGet();
			smTS = SM_CONNECT_WAIT;

		break;

		case SM_CONNECT_WAIT:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Connect wait..\r\n");
			#endif
			// Wait for connection..

			if (TCPIsConnected(MySocket))
			{
				smTS = SM_CONNECTED;
			}
			// If not connected and spent long time here,
			// Go back to previous state and re-connect.
			else if (tickGet()-Timer > 5*TICK_1S)
			{
				TCPDisconnect(MySocket);
				MySocket = INVALID_SOCKET;
				smTS = SM_CONNECT;
			}
			else if (tcp_tries>=MAX_TCP_TRIES)
			{
				//Abort
				TCPDisconnect(MySocket);
				MySocket = INVALID_SOCKET;
				smTS = SM_ABORT;
			}


		break;

		case SM_CONNECTED:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Connected..\r\n");
			#endif
			// Send data.
			Timer = tickGet();

			if(TCPIsPutReady(MySocket))
			{

				TCPPut(MySocket, 'G');
				TCPPut(MySocket, 'E');
				TCPPut(MySocket, 'T');
				TCPPut(MySocket, ' ');
				TCPPut(MySocket, '/');
				TCPPut(MySocket, 't');
				TCPPut(MySocket, 'i');
				TCPPut(MySocket, 'm');
				TCPPut(MySocket, 'e');
				TCPPut(MySocket, '.');
				TCPPut(MySocket, 'p');
				TCPPut(MySocket, 'h');
				TCPPut(MySocket, 'p');
				TCPPut(MySocket, ' ');
				TCPPut(MySocket, 'H');
				TCPPut(MySocket, 'T');
				TCPPut(MySocket, 'T');
				TCPPut(MySocket, 'P');
				TCPPut(MySocket, '/');
				TCPPut(MySocket, '1');
				TCPPut(MySocket, '.');
				TCPPut(MySocket, '0');
				TCPPut(MySocket, '\r');
				TCPPut(MySocket, '\n');
				TCPPut(MySocket, '\r');
				TCPPut(MySocket, '\n');

				// Send the packet
				TCPFlush(MySocket);
				
				smTS = SM_RECEIVE;
			}

		break;

		case SM_RECEIVE:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Receive..\r\n");
			#endif
			// Client disconnected.
			if(!TCPIsConnected(MySocket))
			{
				smTS = SM_ABORT;
				break;
			}				

			if(TCPIsGetReady(MySocket))
			{
				while(TCPGet(MySocket, &i))
				{
					if (i==CR) rcnt++;
					else if(i==LF) ncnt++;
					else
					{ 
						rcnt=0; 
						ncnt=0; 
					}

					if (foundData==1)
					{
						if (j>=0)
						{	
							timeNow=timeNow+(((DWORD)i)<<(8*j--));
							#if defined(TIMESYNC_DEBUG)
							while(BusyUART()); WriteUART(i);
							#endif
						}
					}

					if(rcnt>1 && ncnt>1) {j=3; timeNow=0; foundData=1;}

					
				}
				smTS = SM_DISCONNECT;	
			}

		break;

		case SM_DISCONNECT:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("\r\nDisconnect\r\n");
			#endif
			foundData=0;
			t = tickGet();
			lastSync = timeNow;
			perodicTick=tickGet();
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;
			smTS = SM_DONE;
		break;

		case SM_ABORT:
			#if defined(TIMESYNC_DEBUG)
			putrsUART("Abort...\r\n");
			#endif
			smTS = SM_START;
		break;

		case SM_DONE:
			if (tickGet()-perodicTick > SYNC_INTERVAL*TICK_1S)
			{
				#if defined(TIMESYNC_DEBUG)
				putrsUART("GO!\r\n");
				#endif
				smTS = SM_START;
			}
		break;

		default: smTS = SM_START; break;

	}


}

DWORD getTime(timeClock tc)
{
	DWORD c;

	if (tc==CURRENT) c = timeNow;
	if (tc==LAST) c = lastSync;	

	if (lastSync>0)
		return c;
	return 0;

}

DWORD getTimeStr(timeClock tc,char *str)
{
	DWORD c;
	char i;

	c = getTime(tc);

	dwordToHex(c,str);

	return c;
}



#endif
