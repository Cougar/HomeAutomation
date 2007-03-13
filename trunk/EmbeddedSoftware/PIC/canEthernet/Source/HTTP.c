/*********************************************************************
 *
 *               HTTP Implementation for Microchip TCP/IP Stack
 *
 **********************************************************************
 * FileName:        Http.c
 * Dependencies:    string.h
 *                  Stacktsk.h
 *                  Http.h
 *                  MPFS.h
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original
 * Nilesh Rajbharti     9/12/01     Released (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     7/9/02      Rev 2.1 (Fixed HTTPParse bug)
 * Howard Schlunder		2/9/05		Fixed variable substitution 
 *									parsing (uses hex now)
 ********************************************************************/
#define THIS_IS_HTTP_SERVER

#include <string.h>

#include "..\Include\StackTsk.h"
#include "..\Include\HTTP.h"
#include "..\Include\MPFS.h"
#include "..\Include\TCP.h"
#include "..\Include\Helpers.h"
#include "..\Include\UART.h"
#include "..\Include\compiler.h"
#include "..\Include\TimeSync.h"
#include "..\Include\Scheduler.h"

#if defined(STACK_USE_HTTP_SERVER)



#if (MAC_TX_BUFFER_SIZE <= 130 || MAC_TX_BUFFER_SIZE > 1500 )
#error HTTP : Invalid MAC_TX_BUFFER_SIZE value specified.
#endif


// HTTP FSM states for each connection.
typedef enum _SM_HTTP
{
    SM_HTTP_IDLE = 0,
    SM_HTTP_GET,
    SM_HTTP_NOT_FOUND,
    SM_HTTP_GET_READ,
    SM_HTTP_GET_PASS,
    SM_HTTP_GET_DLE,
    SM_HTTP_GET_HANDLE,
    SM_HTTP_GET_HANDLE_NEXT,
    SM_HTTP_GET_VAR,
    SM_HTTP_DISCONNECT,
    SM_HTTP_DISCONNECT_WAIT,
    SM_HTTP_HEADER,
	SM_HTTP_HEADER2,
	SM_HTTP_HEADER3,
    SM_HTTP_DISCARD
} SM_HTTP;

// Supported HTTP Commands
typedef enum _HTTP_COMMAND
{
    HTTP_GET,
    HTTP_POST,
    HTTP_NOT_SUPPORTED,
    HTTP_INVALID_COMMAND
} HTTP_COMMAND;

// HTTP Connection Info - one for each connection.
typedef struct _HTTP_INFO
{
    TCP_SOCKET socket;
    MPFS file;
    SM_HTTP smHTTP;
    BYTE smHTTPGet;
    WORD VarRef;
    BYTE bProcess;
    BYTE Variable;
} HTTP_INFO;
typedef BYTE HTTP_HANDLE;


typedef enum
{
    HTTP_NOT_FOUND,
    HTTP_NOT_AVAILABLE
} HTTP_MESSAGES;



// HTTP Command Strings
ROM BYTE HTTP_GET_STRING[]                      =           \
                        "GET";
#define HTTP_GET_STRING_LEN                                 \
                        (sizeof(HTTP_GET_STRING)-1)


// Maximum nuber of arguments supported by this HTTP Server.
#define MAX_HTTP_ARGS       (11)

// Maximum HTML Command String length.
#define MAX_HTML_CMD_LEN    (100)

static HTTP_INFO HCB[MAX_HTTP_CONNECTIONS];

static void HTTPProcess(HTTP_HANDLE h);
static HTTP_COMMAND HTTPParse(BYTE *string,
                              BYTE** arg,
                              BYTE* argc);



ROM BYTE GOOD_BEG[]="HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n<html><head><title>Webbserver på 18F2620</title></head><body>";
ROM BYTE GOOD_END[]="</body></html>\r\n";
//ROM BYTE NOT_FOUND[]="HTTP/1.0 404 Not found\r\n\r\nNot found.\r\n";

ROM BYTE WELCOME[] ="<center><h3>Väkommen!</h3></center>Denna sida ligger på en microkontroller av typen 18F2620.<br><br>";
ROM BYTE LED_TEXT[]="<b>LED1</b> är <i>";
ROM BYTE LED_ON[]  ="på";
ROM BYTE LED_OFF[] ="av";
ROM BYTE TOGG[]    ="</i>. Växla <a href=\"?" VAR_LED1_STR "=1\">på</a> <a href=\"?" VAR_LED1_STR "=0\">av</a><br><br>";
ROM BYTE CURRENTIME[]="Nuvarande tid (timestamp): <b>";
ROM BYTE LASTTIME[]=", synkad (timestamp): <b>";
ROM BYTE NO_SYNC[]="<i>Ingen synk ännu.</i>";
ROM BYTE TIMEND[]="</b>";
ROM BYTE SCHED[]="<br><br><u>Schemalagda händelser</u><br>";
ROM BYTE JA[]="Ja";
ROM BYTE NEJ[]="Nej";
ROM BYTE ACTIVE[]="Aktiv: <i>";
ROM BYTE WHEN[]="</i>, När: <i>";
ROM BYTE REPEAT[]="</i>, Repetera: <i>";
ROM BYTE WHAT[]="</i>, Vad: <i>";
ROM BYTE VALUE[]="</i>, Sätts till värde: <i>";
ROM BYTE SHEDEND[]="</i><br>";

char tmp[32];

/*********************************************************************
 * Function:        void HTTPInit(void)
 *
 * PreCondition:    TCP must already be initialized.
 *
 * Input:           None
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        Set all HTTP connections to Listening state.
 *                  Initialize FSM for each connection.
 *
 * Note:            This function is called only one during lifetime
 *                  of the application.
 ********************************************************************/
void HTTPInit(void)
{
    BYTE i;

    for ( i = 0; i <  MAX_HTTP_CONNECTIONS; i++ )
    {
        HCB[i].socket = TCPListen(HTTP_PORT);
        HCB[i].smHTTP = SM_HTTP_IDLE;
    }
}



/*********************************************************************
 * Function:        void HTTPServer(void)
 *
 * PreCondition:    HTTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened HTTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:        Browse through each connections and let it
 *                  handle its connection.
 *                  If a connection is not finished, do not process
 *                  next connections.  This must be done, all
 *                  connections use some static variables that are
 *                  common.
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
void HTTPServer(void)
{
    BYTE conn;

    for ( conn = 0;  conn < MAX_HTTP_CONNECTIONS; conn++ )
        HTTPProcess(conn);
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
static void HTTPProcess(HTTP_HANDLE h)
{
    BYTE httpData[MAX_HTML_CMD_LEN+1];
    HTTP_COMMAND httpCommand;
    WORD httpLength;
    BOOL lbContinue;
    BYTE *arg[MAX_HTTP_ARGS];
    BYTE argc;
    BYTE i;
    HTTP_INFO* ph;
    ROM char* romString;

	BYTE j;
	EVENT event;
	DWORD t;

    ph = &HCB[h];
	
    do
    {
        lbContinue = FALSE;

        // If during handling of HTTP socket, it gets disconnected,
        // forget about previous processing and return to idle state.
        if(!TCPIsConnected(ph->socket))
        {
            ph->smHTTP = SM_HTTP_IDLE;
            break;
        }


        switch(ph->smHTTP)
        {
        case SM_HTTP_IDLE:
            if(TCPIsGetReady(ph->socket))
            {
			
                lbContinue = TRUE;

                httpLength = 0;
                while(httpLength < MAX_HTML_CMD_LEN &&
                      TCPGet(ph->socket, &httpData[httpLength++]) );
                httpData[httpLength] = '\0';
                TCPDiscard(ph->socket);

                ph->smHTTP = SM_HTTP_NOT_FOUND;
                argc = MAX_HTTP_ARGS;
                httpCommand = HTTPParse(httpData, arg, &argc);
                if ( httpCommand == HTTP_GET )
                {
	            	HTTPExecCmd(&arg[0], argc);
					ph->smHTTP = SM_HTTP_HEADER;
                }
            }
            break;

		case SM_HTTP_HEADER:

            if ( TCPIsPutReady(ph->socket) )
            {
				for(i=0;i<sizeof(GOOD_BEG)-1;i++) TCPPut(ph->socket, GOOD_BEG[i]);

				for(i=0;i<sizeof(WELCOME)-1;i++) TCPPut(ph->socket, WELCOME[i]);

				for(i=0;i<sizeof(LED_TEXT)-1;i++) TCPPut(ph->socket, LED_TEXT[i]);
	
				if (getVariable(VAR_LED1)) romString=LED_ON; else romString=LED_OFF;

				while( (i = *romString++) ) TCPPut(ph->socket, i);

				for(i=0;i<sizeof(TOGG)-1;i++) TCPPut(ph->socket, TOGG[i]);

				for(i=0;i<sizeof(CURRENTIME)-1;i++) TCPPut(ph->socket, CURRENTIME[i]);

				//getTimeStr(CURRENT,tmp);
				
				if (t=getTime(CURRENT))
				{
					timestampToString(t,tmp);
					for(i=0;tmp[i]!='\0';i++) TCPPut(ph->socket, tmp[i]);
				} else TCPPut(ph->socket, '-');

				for(i=0;i<sizeof(TIMEND)-1;i++) TCPPut(ph->socket, TIMEND[i]);

				for(i=0;i<sizeof(LASTTIME)-1;i++) TCPPut(ph->socket, LASTTIME[i]);

				if (t=getTime(LAST))
				{
					timestampToString(t,tmp);
					for(i=0;tmp[i]!='\0';i++) TCPPut(ph->socket, tmp[i]);
				}
				else
				{
					for(i=0;i<sizeof(NO_SYNC)-1;i++) TCPPut(ph->socket, NO_SYNC[i]);
				}

				for(i=0;i<sizeof(TIMEND)-1;i++) TCPPut(ph->socket, TIMEND[i]);
				TCPFlush(ph->socket);
				ph->smHTTP = SM_HTTP_HEADER2;
			}
		break;

		case SM_HTTP_HEADER2:

            if ( TCPIsPutReady(ph->socket) )
            {	

				for(i=0;i<sizeof(GOOD_END)-1;i++) TCPPut(ph->socket, GOOD_END[i]);

                TCPFlush(ph->socket);
                ph->smHTTP = SM_HTTP_DISCONNECT;
            }
		break;


        case SM_HTTP_DISCONNECT:
            if(TCPIsConnected(ph->socket))
            {
                if(TCPIsPutReady(ph->socket))
                {
                    TCPDisconnect(ph->socket);

                    // Switch to not-handled state.  This FSM has
                    // one common action that checks for disconnect
                    // condition and returns to Idle state.
                    ph->smHTTP = SM_HTTP_DISCONNECT_WAIT;
                }
            }
            break;

        }
   	} while( lbContinue );
}


/*********************************************************************
 * Function:        static HTTP_COMMAND HTTPParse(BYTE *string,
 *                                              BYTE** arg,
 *                                              BYTE* argc,
 *                                              BYTE* type)
 *
 * PreCondition:    None
 *
 * Input:           string      - HTTP Command String
 *                  arg         - List of string pointer to hold
 *                                HTTP arguments.
 *                  argc        - Pointer to hold total number of
 *                                arguments in this command string/
 *                  type        - Pointer to hold type of file
 *                                received.
 *                              Valid values are:
 *                                  HTTP_TXT
 *                                  HTTP_HTML
 *                                  HTTP_GIF
 *                                  HTTP_CGI
 *                                  HTTP_UNKNOWN
 *
 * Output:          HTTP FSM and connections are initialized
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function parses URL that may or may not
 *                  contain arguments.
 *                  e.g. "GET HTTP/1.0 thank.htm?name=MCHP&age=12"
 *                      would be returned as below:
 *                          arg[0] => GET
 *                          arg[1] => thank.htm
 *                          arg[2] => name
 *                          arg[3] => MCHP
 *                          arg[4] => 12
 *                          argc = 5
 *
 *                  This parses does not "de-escape" URL string.
 ********************************************************************/
static HTTP_COMMAND HTTPParse(BYTE *string,
                            BYTE** arg,
                            BYTE* argc)
{
    BYTE i;
    BYTE smParse;
    HTTP_COMMAND cmd;
    BYTE *ext;
    BYTE c;

    enum
    {
        SM_PARSE_IDLE,
        SM_PARSE_ARG,
        SM_PARSE_ARG_FORMAT
    };



    smParse = SM_PARSE_IDLE;
    ext = NULL;
    i = 0;

    // Only "GET" is supported for time being.
    if ( !memcmppgm2ram(string, (ROM void*) HTTP_GET_STRING, HTTP_GET_STRING_LEN) )
    {
        string += (HTTP_GET_STRING_LEN + 1);
        cmd = HTTP_GET;
    }
    else
    {
        return HTTP_NOT_SUPPORTED;
    }

    // Skip white spaces.
    while( *string == ' ' )
        string++;

    c = *string;

    while ( c != ' ' &&  c != '\0' && c != '\r' && c != '\n' )

    {
        // Do not accept any more arguments than we haved designed to.
        if ( i >= *argc )
            break;

        switch(smParse)
        {
        case SM_PARSE_IDLE:
            arg[i] = string;
            c = *string;
            if ( c == '/' || c == '\\' )
                smParse = SM_PARSE_ARG;
            break;

        case SM_PARSE_ARG:
            arg[i++] = string;
            smParse = SM_PARSE_ARG_FORMAT;
            /*
             * Do not break.
             * Parameter may be empty.
             */

        case SM_PARSE_ARG_FORMAT:
            c = *string;
            if ( c == '?' || c == '&' )
            {
                *string = '\0';
                smParse = SM_PARSE_ARG;
            }
            else
            {
                // Recover space characters.
                if ( c == '+' )
                    *string = ' ';

                // Remember where file extension starts.
                else if ( c == '.' && i == 1u )
                {
                    ext = ++string;
                }

                else if ( c == '=' )
                {
                    *string = '\0';
                    smParse = SM_PARSE_ARG;
                }

                // Only interested in file name - not a path.
                else if ( c == '/' || c == '\\' )
                    arg[i-1] = string+1;

            }
            break;
        }
        string++;
        c = *string;
    }
    *string = '\0';

    if ( i == 0u )
    {
        arg[0][0] = '\0';
        i++;
    }
    *argc = i;

    return cmd;
}


#endif //#if defined(STACK_USE_HTTP_SERVER)
