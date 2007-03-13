#include <string.h>

#include "..\Include\StackTsk.h"
#include "..\Include\SGP.h"
#include "..\Include\TCP.h"
#include "..\Include\Helpers.h"
#include "..\Include\UART.h"
#include "..\Include\compiler.h"

#if defined(STACK_USE_SGP_SERVER)

#define MAX_SGP_CONNECTIONS 2
#define MAX_SGP_CMD_LEN 100

#if (MAC_TX_BUFFER_SIZE <= 130 || MAC_TX_BUFFER_SIZE > 1500 )
#error SGP : Invalid MAC_TX_BUFFER_SIZE value specified.
#endif


// SGP FSM states for each connection.
typedef enum _SM_SGP
{
    SM_SGP_IDLE = 0,
	SM_SGP_GET,
    SM_SGP_DISCONNECT,
    SM_SGP_DISCONNECT_WAIT,
    SM_SGP_DISCARD
} SM_SGP;


// SGP Connection Info - one for each connection.
typedef struct _SGP_INFO
{
    TCP_SOCKET socket;
    SM_SGP smSGP;

    BYTE bProcess;

} SGP_INFO;
typedef BYTE SGP_HANDLE;


static SGP_INFO HCB[MAX_SGP_CONNECTIONS];

static void SGPProcess(SGP_HANDLE h);


void SGPInit(void)
{
    BYTE i;

    for ( i = 0; i <  MAX_SGP_CONNECTIONS; i++ )
    {
        HCB[i].socket = TCPListen(SGP_PORT);
        HCB[i].smSGP = SM_SGP_IDLE;
    }
}



void SGPServer(void)
{
    BYTE conn;

    for ( conn = 0;  conn < MAX_SGP_CONNECTIONS; conn++ )
        SGPProcess(conn);
}


/*********************************************************************
 * Function:        static BOOL HTTPProcess(HTTP_HANDLE h)
 *
 * PreCondition:    HTTPInit() called.
 *
 * Input:           h   -   Index to the handle which needs to be
 *                          processed.
 *
 * Output:          Connection referred by 'h' is served.
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            None.
 ********************************************************************/
static void SGPProcess(SGP_HANDLE h)
{
    BYTE sgpData[MAX_SGP_CMD_LEN+1];
	WORD sgpLength;
    BOOL lbContinue;
    SGP_INFO* ph;
    ROM char* romString;
	BYTE i;

    ph = &HCB[h];
	
    do
    {
        lbContinue = FALSE;

        // If during handling of HTTP socket, it gets disconnected,
        // forget about previous processing and return to idle state.
        if(!TCPIsConnected(ph->socket))
        {
            ph->smSGP = SM_SGP_IDLE;
            break;
        }


        switch(ph->smSGP)
        {
        case SM_SGP_IDLE:
            if(TCPIsGetReady(ph->socket))
            {
			
                lbContinue = TRUE;

                sgpLength = 0;
                while(sgpLength < MAX_SGP_CMD_LEN &&
                      TCPGet(ph->socket, &sgpData[sgpLength++]) );
                sgpData[sgpLength] = '\0';
                TCPDiscard(ph->socket);


					ph->smSGP = SM_SGP_GET;
            }
            break;

		case SM_SGP_GET:

            if ( TCPIsPutReady(ph->socket) )
            {
				
				for(i=0;i<sgpLength;i++)
                    TCPPut(ph->socket, sgpData[i]);

				if (sgpData[0]=='1') LED1_IO=1;
				if (sgpData[0]=='2') LED1_IO=0;

                TCPFlush(ph->socket);
                ph->smSGP = SM_SGP_DISCONNECT;
            }
		break;


        case SM_SGP_DISCONNECT:
            if(TCPIsConnected(ph->socket))
            {
                if(TCPIsPutReady(ph->socket))
                {
                    TCPDisconnect(ph->socket);

                    // Switch to not-handled state.  This FSM has
                    // one common action that checks for disconnect
                    // condition and returns to Idle state.
                    ph->smSGP = SM_SGP_DISCONNECT_WAIT;
                }
            }
            break;

        }
   	} while( lbContinue );
}


#endif //#if defined(STACK_USE_SGP_SERVER)
