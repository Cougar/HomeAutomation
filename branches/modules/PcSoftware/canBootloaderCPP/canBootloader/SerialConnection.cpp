#define STRICT
#include "SerialConnection.h"



void arrayCopy(unsigned char *src,unsigned int srcIndex,unsigned char *dst,unsigned int dstIndex,unsigned int length);

bool queueEnqueue(CanProtoMessage cpm);
bool queueDequeue(CanProtoMessage *cpm);

CanProtoMessage *cpmQueue;
int queueReadPointer=0;
int queueWritePointer=0;
int queueLength=0;


static HANDLE hCom;
static HANDLE  hRunMutex;  // "Keep Running" mutex 

static void SerRead(void *pMyID);

SerialConnection::SerialConnection(LPCTSTR portName,EBaudrate baudRate,EDataBits byteSize,EParity parity, EStopBits stopBits)
{
	SerialConnection::uportName=portName;
	SerialConnection::ubaudRate=baudRate;
	SerialConnection::ubyteSize=byteSize;
	SerialConnection::uparity = parity;
	SerialConnection::ustopBits=stopBits;

	hCom = NULL;
}

SerialConnection::~SerialConnection() {  }

bool SerialConnection::open()
{
	DCB dcb;

	// Check if the port isn't already opened
	if (hCom)
	{
		printf ("Open - Port already opened\n");
		return false;
	}

	hCom = CreateFile( uportName,
					GENERIC_READ | GENERIC_WRITE,
					0,    // must be opened with exclusive-access
					NULL, // no security attributes
					OPEN_EXISTING, // must use OPEN_EXISTING
					FILE_FLAG_OVERLAPPED,    // not overlapped I/O
					NULL  // hTemplate must be NULL for comm devices
					);

	if (hCom == INVALID_HANDLE_VALUE) 
	{
	   // Handle the error.
	   printf ("CreateFile failed with error %d.\n", GetLastError());
	   return false;
	}

	// Build on the current configuration, and skip setting the size
	// of the input and output buffers with SetupComm.


	if (!GetCommState(hCom, &dcb)) 
	{
	  // Handle the error.
	  printf ("GetCommState failed with error %d.\n", GetLastError());
	  return false;
	}

	// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

	dcb.BaudRate = SerialConnection::ubaudRate;     // set the baud rate
	dcb.ByteSize = SerialConnection::ubyteSize;             // data size, xmit, and rcv
	dcb.Parity = SerialConnection::uparity;        // no parity bit
	dcb.StopBits = SerialConnection::ustopBits;    // one stop bit

	if (!SetCommState(hCom, &dcb)) 
	{
	  // Handle the error.
	  printf ("SetCommState failed with error %d.\n", GetLastError());
	  return false;
	}

	if (!SetCommMask(hCom, EV_RXCHAR))
	{
		// Handle the error. 
		printf("SetCommMask failed with error %d.\n", GetLastError());
		return false;
	}

	hRunMutex = CreateMutex( NULL, TRUE, NULL );      // Set 
	_beginthread( SerRead, 0, 0 );

	printf ("Serial port successfully reconfigured.\n");
	return true;
}

HANDLE SerialConnection::getHandle()
{

	return hCom;
}


bool SerialConnection::close()
{
	ReleaseMutex( hRunMutex );
	CloseHandle( hRunMutex );

	if (hCom==0)
	{
		printf("Close - Method called when device is not open\n");
		return false;
	}

	CloseHandle(hCom);

	return true;
}

bool SerialConnection::isOpen()
{
	return (hCom != 0);
}

bool SerialConnection::write(LPCVOID lpBuffer, unsigned int nNumberOfBytesToWrite)
{
	if (hCom==0)
	{
		printf("Close - Method called when device is not open\n");
		return false;
	}

	/*
	OVERLAPPED ol;
	memset(&ol,0,sizeof(ol));

	if (!WriteFileEx(hCom,lpBuffer,nNumberOfBytesToWrite,&ol,NULL))
	{
		printf("Write - Unable to write the data\n");
	}
*/

	OVERLAPPED osWrite = {0};
   DWORD dwWritten;
   DWORD dwRes;
   BOOL fRes;


	// Create this write operation's OVERLAPPED structure's hEvent.
   osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
   if (osWrite.hEvent == NULL)
   {
      printf("error creating overlapped event handle\n");
      return FALSE;
   }


	// Issue write.
   if (!WriteFile(hCom, lpBuffer, nNumberOfBytesToWrite, &dwWritten, &osWrite)) 
   {
      if (GetLastError() != ERROR_IO_PENDING) 
	  { 
         printf("WriteFile failed, but isn't delayed. Report error and abort.\n");
         fRes = FALSE;
      }
      else
	  {
         // Write is pending.
         dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
         switch(dwRes)
         {
            // OVERLAPPED structure's event has been signaled. 
            case WAIT_OBJECT_0:
                 if (!GetOverlappedResult(hCom, &osWrite, &dwWritten, FALSE))
                       fRes = FALSE;
                 else
                  // Write operation completed successfully.
                  fRes = TRUE;
                 break;
            
            default:
                 // An error has occurred in WaitForSingleObject.
                 // This usually indicates a problem with the
                // OVERLAPPED structure's event handle.
                 fRes = FALSE;
                 break;
         }
      }
   }
   else fRes = TRUE;
      // WriteFile completed immediately.
      

   CloseHandle(osWrite.hEvent);
   return fRes;




	return false;
}


void SerRead(void *pMyID)
{
	unsigned char iBuff[SERIAL_READ_BUFFER_SIZE];
	unsigned int iBuffPointer = 0;

	cpmQueue = (CanProtoMessage *)malloc(QUEUE_PLACES*sizeof(CanProtoMessage));

	DWORD dwCommEvent;
	DWORD dwRead;
	unsigned char chRead[128];

	if (hCom ==0)
	{
		printf("Serial port closed.");
		return;
	}

	OVERLAPPED osReader = {0};

	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);



	while ( WaitForSingleObject( hRunMutex, 75L ) == WAIT_TIMEOUT )
	{

		if (WaitCommEvent(hCom, &dwCommEvent, NULL)) 
		{
			if (ReadFile(hCom, &chRead, 128, &dwRead, &osReader))
			{

				// Read data, until there is nothing left
				DWORD dwBytesRead = 0;
				unsigned char szBuffer[SERIAL_INPUT_BUFFER_SIZE];
				do
				{

					if (dwRead > 0)
					{
					
						// Spara i array, sist.
						arrayCopy(chRead, 0, iBuff, iBuffPointer, dwRead);
						iBuffPointer += dwRead;

						// Följ parserutin.
						// Köa.
						// flytta.

						 // När array större eller lika med PACKET_LENGTH...
						while (iBuffPointer >= PACKET_LENGTH)
						{
							int startIndex = 0;

							// Sök igenom efter start byte från början.
							for (unsigned int i = 0; i < iBuffPointer; i++)
							{
								// Poppa alla bytes som inte är startbyte.
								if (iBuff[i] != UART_START_BYTE) startIndex++;
								else
								{
									// När startbyte hittas, kolla om återstående längd är större eller lika med PACKET_LENGTH (inkl startbyte)
									if ((iBuffPointer - startIndex) >= PACKET_LENGTH)
									{
										//om så, kolla PACKET_LENGTH-1 byte fram.
										if (iBuff[startIndex + PACKET_LENGTH - 1] == UART_END_BYTE)
										{
											// Om byte PACKET_LENGTH-1 är slutbyte så extraktas startIndex till slutbyteindex.
											CanProtoMessage cmp(iBuff,startIndex + 1);
											queueEnqueue(cmp);
											

											// Sätt ny startindex och avsluta loop.
											startIndex += PACKET_LENGTH;
											break;
										}
									}
								}
							}
							// och i slutet göra en array copy.
							// Flytta ner allt efter slutbyte till index 0 i array.
							arrayCopy(iBuff, startIndex, iBuff, 0, iBuffPointer - PACKET_LENGTH);
							iBuffPointer -= startIndex;
						}
					}


				}while (dwBytesRead == sizeof(szBuffer)-1);
		
				//printf("byte receiced\n");


			} else { printf("Error (1): %d\n",GetLastError()); break;}
		} else { printf("Error (2): %d\n",GetLastError()); break;}
	}


}

bool SerialConnection::getMessage(CanProtoMessage *cpm)
{
	return queueDequeue(cpm);
}

void arrayCopy(unsigned char *src,unsigned int srcIndex,unsigned char *dst,unsigned int dstIndex,unsigned int length)
{
	// Copy an aray to another array (of unsigned chars == bytes)
	for(unsigned int i=0;i<length;i++) dst[dstIndex+i]=src[srcIndex+i];
}


bool queueEnqueue(CanProtoMessage cpm)
{
	// Enqueue reveiced message to queue
	if (queueLength>=QUEUE_PLACES) return false;

	cpmQueue[queueWritePointer]=cpm;

	queueLength++;
	queueWritePointer++;
	if (queueWritePointer>=QUEUE_PLACES) queueWritePointer=0;

	printf("[%ld] Queue message, length: %d\n",GetTickCount(),queueLength);

	return true;
}
bool queueDequeue(CanProtoMessage *cpm)
{
	// Dequeue can message.
	if (queueLength==0) return false;

	cpm = &cpmQueue[queueReadPointer];

	queueLength--;
	queueReadPointer++;
	if (queueReadPointer>=QUEUE_PLACES) queueReadPointer=0;

	return true;
}