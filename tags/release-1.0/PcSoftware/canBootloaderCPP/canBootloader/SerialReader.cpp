#include "SerialReader.h"

#include <assert.h>

//void arrayCopy(unsigned char *src,unsigned int srcIndex,unsigned char *dst,unsigned int dstIndex,unsigned int length);

//bool queueEnqueue(CanProtoMessage cpm);
//bool queueDequeue(CanProtoMessage *cpm);
/*
CanProtoMessage cpmQueue[QUEUE_PLACES];
int queueReadPointer=0;
int queueWritePointer=0;
int queueLength=0;

static CSerial * serial;

static SerialConnection * sc;

static HANDLE  hRunMutex;                   // "Keep Running" mutex 
*/
void SerRead(void *pMyID)
{

	unsigned char iBuff[SERIAL_READ_BUFFER_SIZE];
	unsigned int iBuffPointer = 0;

	
	long lLastError = 0;

	/*
	// Register only for the receive event
	lLastError = serial->SetMask(CSerial::EEventRecv);
	if (lLastError != ERROR_SUCCESS)
	{
		printf("Err! Unable to set COM-port event mask.\n");
		return;
	}

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
	lLastError = serial->SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (lLastError != ERROR_SUCCESS)
	{
		printf("Err! Unable to set COM-port read timeout.\n");
		return;
	}
*/
	/*
	if (SetCommMask(sc->hCom,EV_RXCHAR)) 
    {
        // Handle the error. 
        printf("SetCommMask failed with error %d.\n", GetLastError());
        return;
    }

	    // Create an event object for use by WaitCommEvent. 

	OVERLAPPED o;
	DWORD dwEvtMask;



	o.hEvent = CreateEvent(
        NULL,   // default security attributes 
        TRUE,   // manual-reset event 
        FALSE,  // not signaled 
        NULL    // no name
		);


	// Intialize the rest of the OVERLAPPED structure to zero.
    o.Internal = 0;
    o.InternalHigh = 0;
    o.Offset = 0;
    o.OffsetHigh = 0;

	*/

		DWORD dwCommEvent;
		DWORD dwRead;
		char  chRead;
/*
		if (sc->getHandle() ==0)
		{
			printf("Serial port closed.");
			return;
		}
*/
		/*
		if (!SetCommMask(sc->getHandle(), EV_RXCHAR))
		{
			// Handle the error. 
			printf("SetCommMask failed with error %d.\n", GetLastError());
			return;
		}


	while ( WaitForSingleObject( hRunMutex, 75L ) == WAIT_TIMEOUT )
	{

		if (WaitCommEvent(sc->getHandle(), &dwCommEvent, NULL)) 
		{
			if (ReadFile(sc->getHandle(), &chRead, 1, &dwRead, NULL))
			{
				printf("byte receiced\n");
			} else {printf("Error1\n"); break;}
		} else {printf("Error2\n"); break;}



		/*
		if (WaitCommEvent(sc->hCom, &dwEvtMask, &o)) 
		{
			if (dwEvtMask & EV_RXCHAR) 
			{
				printf("RX\n");
			}
		}
		else
		{
			DWORD dwRet = GetLastError();
			if( ERROR_IO_PENDING == dwRet)
			{
				printf("I/O is pending...\n");

				// To do.
			}
			//else printf("Wait failed with error %d.\n", GetLastError());
		}*/
	//}


	return;
/*
	while(1)
	{

		// Wait for an event
		lLastError = serial->WaitEvent();
		if (lLastError != ERROR_SUCCESS)
		{
			printf("Err! Unable to wait for a COM-port event.\n");
			break;
		}

		// Save event
		const CSerial::EEvent eEvent = serial->GetEventType();


		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{

			// Read data, until there is nothing left
			DWORD dwBytesRead = 0;
			unsigned char szBuffer[SERIAL_INPUT_BUFFER_SIZE];
			do
			{
				// Read data from the COM-port
				lLastError = serial->Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
				{
					printf("Unable to read from COM-port.");
					return;
				}

				if (dwBytesRead > 0)
				{
					


					// Spara i array, sist.
					arrayCopy(szBuffer, 0, iBuff, iBuffPointer, dwBytesRead);
					iBuffPointer += dwBytesRead;

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
		}

	}
*/
	return;
}

/*
void serialReaderStart(CSerial * ser)
{
	serial = ser;
	hRunMutex = CreateMutex( NULL, TRUE, NULL );      // Set 
	_beginthread( SerRead, 0, 0 );
}

void serialReaderStartSerCon(SerialConnection * ser)
{
	sc = ser;
	hRunMutex = CreateMutex( NULL, TRUE, NULL );      // Set 
	_beginthread( SerRead, 0, 0 );
}

void serialReaderStop( void )
{
	ReleaseMutex( hRunMutex );
	CloseHandle( hRunMutex );
}
*/



//bool getMessage(CanProtoMessage *cpm)
//{
//	return queueDequeue(cpm);
//}
/*
void arrayCopy(unsigned char *src,unsigned int srcIndex,unsigned char *dst,unsigned int dstIndex,unsigned int length)
{
	// Copy an aray to another array (of unsigned chars == bytes)
	for(unsigned int i=0;i<length;i++) dst[dstIndex+i]=src[srcIndex+i];
}
*/
/*
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
}*/