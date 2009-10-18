// canBootloader.cpp : Defines the entry point for the console application.
//

//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_serial.asp


#define STRICT
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
//#include "Serial.h"
#include "HexFile.h"
#include "CanProtoMessage.h"
//#include "SerialReader.h"
#include "DownloadState.h"
#include "SerialConnection.h"

bool parseInput(char instr[]);
bool serialSetConnected(bool connected);
bool isConnected(void);

#define INPUT_BUFFER_SIZE 128
#define FILEPATH1 "F:/MCP/projects/CAN/canNode/_output/canNode.hex"
#define FILEPATH2 "F:/MCP/projects/CAN/canDebug/canDebug.hex"

//static CSerial serial;
static SerialConnection scp(TEXT("COM2"),SerialConnection::EBaud115200,SerialConnection::EData8,SerialConnection::EParNone,SerialConnection::EStop1);
static HexFile hf;

int _tmain(int argc, _TCHAR* argv[])
{

	char inbuff[INPUT_BUFFER_SIZE];

	printf("Main: ");

	// Forever input read loop.
	do
	{
		// Read user input.
		gets_s(inbuff,INPUT_BUFFER_SIZE);
	} while(parseInput(inbuff));

	// Close the port again
    if(isConnected()) serialSetConnected(false);

	return 0;
}


bool parseInput(char instr[])
{
	LONG    lLastError = ERROR_SUCCESS;

	// Make instr lower.
	_strlwr_s(instr,INPUT_BUFFER_SIZE);

	if (strcmp(instr,"exit")==0) return false; // If exit, exit.
	else if (strcmp(instr,"go")==0)
	{
		// Download program.
		if (hf.isValid())
			printf("GO!\n");
		else 
			printf("Un-GO!\n");

		// First connect
		scp.open();
		//serialSetConnected(true);
		// download program
		downloadStart(&scp,&hf);

	}
	else if (strcmp(instr,"load debug")==0)
	{
		// Load debug app.
	}
	else if (strcmp(instr,"load")==0)
	{
		// load regular app.
		if (hf.loadHex(FILEPATH1,true)) { printf("File %s loaded sucessfully!\n",hf.filepath); printf("File size: %lu, upper addr: %lu, lower addr: %lu\n",hf.getLength(),hf.getAddrUpper(),hf.getAddrLower()); }
		else { printf(hf.lastError); }

	}
	/*
	else if (strcmp(instr,"put")==0)
	{
		// Put dummy packet

		unsigned char canmsg[17];
		canmsg[0]=253;
		canmsg[16]=250;
		if (isConnected())
			{
			lLastError = serial.Write(canmsg,17);
			if (lLastError != ERROR_SUCCESS)
			{
				printf("Unable to send data\n");
			}
		} else { printf("Err! No connected to serial port.\n"); }
	}
*/

	else if (strcmp(instr,"conn")==0)
	{
		// Connect to serial port.

		if (isConnected()) { printf("Err! Alredy connected to serial port.\n"); }
		else { serialSetConnected(true); }
	}
	else if (strcmp(instr,"diss")==0)
	{		
		// Dissconnect from serial port.

		if (!isConnected()) { printf("Err! Not connected to serial port.\n"); }
		else { serialSetConnected(false); }
	}
	else if (strcmp(instr,"test2")==0)
	{
		//serialReaderStart(&serial);
	}
	else if (strcmp(instr,"test2 stop")==0)
	{
		//serialReaderStop();
	}
	else if (strcmp(instr,"test3")==0)
	{
		DCB dcb;
		HANDLE hCom;
		bool fSuccess;


	   hCom = ::CreateFile(_T("COM2"),
						GENERIC_READ | GENERIC_WRITE,
						0,    // must be opened with exclusive-access
						0, // no security attributes
						OPEN_EXISTING, // must use OPEN_EXISTING
						FILE_FLAG_OVERLAPPED,    // not overlapped I/O
						0  // hTemplate must be NULL for comm devices
						);

	   if (hCom == INVALID_HANDLE_VALUE) 
	   {
		   // Handle the error.
		   printf ("Err: %d.\n",GetLastError());
	   }


		// Build on the current configuration, and skip setting the size
	   // of the input and output buffers with SetupComm.

	   fSuccess = GetCommState(hCom, &dcb);

	   if (!fSuccess) 
	   {
		  // Handle the error.
		  printf ("GetCommState failed with error %d.\n", GetLastError());
	   }

		// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

	   dcb.BaudRate = CBR_57600;     // set the baud rate
	   dcb.ByteSize = 8;             // data size, xmit, and rcv
	   dcb.Parity = NOPARITY;        // no parity bit
	   dcb.StopBits = ONESTOPBIT;    // one stop bit

	   fSuccess = SetCommState(hCom, &dcb);

	   if (!fSuccess) 
	   {
		  // Handle the error.
		  printf ("SetCommState failed with error %d.\n", GetLastError());
	   }

	   printf ("Serial port successfully reconfigured.\n");
		


	}
	else if (strcmp(instr,"test4")==0)
	{

		scp.open();
	
		char data[] = "hej";
		unsigned char data2[17];

		CanProtoMessage cpm(0x01,0x01,0x01,0x01,1,(unsigned char *)data);
		
		cpm.getBytes(data2);

		scp.write(data2,17);

		//serialReaderStartSerCon(&scp);

		//scp.close();
	}
	else if(strcmp(instr,"tick")==0)
	{ 
      printf ("actual:%ld\n", GetTickCount());

	}
	else if (strcmp(instr,"test")==0)
	{
		// Debug for testing canProtoMessage

		unsigned char dat[8];
		
		for (int i=0;i<8;i++) dat[i]=0x10+i*2;
		
		unsigned char dat2[17];

		CanProtoMessage cpm1(0x07,0x91,0x10,0x87,8,&dat[0]);
		
		cpm1.getBytes(&dat2[0]);
	

		CanProtoMessage cpm2(&dat2[0],1);
	
		if (cpm1.equals(&cpm2)) printf("OK!\n"); else printf("Not OK!\n");

	}
	else
	{
		printf("Command not found.\n");
	}

	printf("Main: ");

	return true;
}


bool serialSetConnected(bool connected)
{
	/*
	LONG    lLastError = ERROR_SUCCESS;

	if (connected)
	{
	    // Attempt to open the serial port (COM1)
		lLastError = serial.Open(_T("COM2"),0,0,true);
		if (lLastError != ERROR_SUCCESS)
		{
			printf("Unable to open COM-port\n");
			return false;
		}
		// Setup the serial port (9600,N81) using hardware handshaking
		lLastError = serial.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
		if (lLastError != ERROR_SUCCESS)
		{
				printf("Unable to set COM-port setting\n");
			return false;
		}
		// Setup handshaking
		lLastError = serial.SetupHandshaking(CSerial::EHandshakeOff);
		if (lLastError != ERROR_SUCCESS)
		{
			printf("Unable to set COM-port handshaking\n");
			return false;
		}

		return true;
	}
	else
	{
		serial.Close();
		return true;
	}
*/
	return false;

}

bool isConnected(void)
{
	//return serial.IsOpen();
	return true;
}