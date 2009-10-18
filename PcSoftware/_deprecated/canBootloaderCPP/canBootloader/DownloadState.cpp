#include "DownloadState.h"



static HexFile * hf;
//static CSerial * serial;
static SerialConnection * serial;

static HANDLE  hRunMutex;                   // "Keep Running" mutex 

void downloadState(void *pMyID);

typedef enum { SEND_START, WAIT_ACK, SEND_PGM_DATA, WAIT_OWN_PACKET, WAIT_PGM_ACK, RESEND_ADDR, WAIT_DONE, SEND_DONE, DONE, WAIT_ADDR_ACK,DEBUG_STATE1,DEBUG_STATE2 } dState;


void downloadState(void *pMyID)
{
	long lLastError = 0;

	dState pgs = SEND_START;
	DWORD t = 0;
	CanProtoMessage * outCm = (CanProtoMessage *)malloc(sizeof(CanProtoMessage));
	CanProtoMessage * cm = (CanProtoMessage *)malloc(sizeof(CanProtoMessage));
	unsigned char data[8];
	unsigned char byteSent = 0;
	unsigned long currentAddress = 0;
	unsigned char bytesToWrite[PACKET_LENGTH];

	while ( WaitForSingleObject( hRunMutex, 75L ) == WAIT_TIMEOUT )
	{

		bool hasMessage = serial->getMessage(cm);
		if (hasMessage) 
		{
			unsigned char *dtmp = cm->getData();
			for(int i=0;i<8;i++) data[i]=dtmp[i];
			printf("[%ld] Has message\n",GetTickCount());
		}

		switch(pgs)
		{
			 case SEND_START:
				// Send boot start packet to target.
				// and wait for ack.
				currentAddress = hf->getAddrLower();
				data[RID_INDEX] = TARGET_ID;
				data[ADDRU_INDEX] = (unsigned char)((currentAddress & 0xFF0000) >> 16);
				data[ADDRH_INDEX] = (unsigned char)((currentAddress & 0xFF00) >> 8);
				data[ADDRL_INDEX] = (unsigned char)(currentAddress & 0xFF);
				outCm = &CanProtoMessage::CanProtoMessage(FUNCT_BOOTLOADER, FUNCC_BOOT_INIT, MY_NID, MY_ID, 8, data);
				outCm->getBytes(bytesToWrite);
				serial->write(bytesToWrite,PACKET_LENGTH);
				t = GetTickCount();
				pgs = WAIT_ACK;
			break;


			case WAIT_ACK:
                
				// Check for timeout, resend start packet in that case..
				if ((GetTickCount() - t) > TIMEOUT_MS)
				{
					pgs = SEND_START;
				}

				// If received ack.
				if (hasMessage && cm->getFunct() == FUNCT_BOOTLOADER && cm->getFuncc() == FUNCC_BOOT_ACK && cm->getNid() == MY_NID && cm->getSid() == TARGET_ID)
				{
					// If no error
					if (data[ERR_INDEX] == ERR_NO_ERROR)
					{
						// Start sending program data..
						byteSent = 0;
						pgs = SEND_PGM_DATA;
					}
					else 
					{ 
						// else resend start..
						pgs = SEND_START;
					}
				}
			break;

			case SEND_PGM_DATA:
				// Send program data.
				for (int i = 0; i < 8; i++)
				{ 
					// Populate data.
					data[i] = hf->getByte(currentAddress + i + byteSent);
				}
				outCm = &CanProtoMessage::CanProtoMessage(FUNCT_BOOTLOADER, (unsigned int)((unsigned int)((unsigned int)(floor(((double)byteSent/8.0)))<<2) + (FUNCC_BOOT_PGM)), MY_NID, MY_ID, 8, data);
				outCm->getBytes(bytesToWrite);
				serial->write(bytesToWrite,PACKET_LENGTH);
				t = GetTickCount();
				pgs = WAIT_OWN_PACKET;
			break;

			case WAIT_OWN_PACKET:
				// Wait reciving own packet, if timeout, resend last.
				if ((GetTickCount() - t) > TIMEOUT_MS)
				{
					//pgs = dState.SEND_PGM_DATA;
					pgs = RESEND_ADDR;
				}

				// If received own packet..
				if (hasMessage && outCm->equals(cm))
				{

					byteSent += 8;

					// Check if 64 bytes sent or not..
					if (byteSent == 64)
					{
						// Yes, wait for ack.
						t = GetTickCount();
						pgs = WAIT_PGM_ACK;
					}
					else
					{ 
						// No, send more.
						pgs = SEND_PGM_DATA;
					}

				}
			break;


			case WAIT_PGM_ACK:
				// Wait for pgm ack.
				if ((GetTickCount() - t) > 2*TIMEOUT_MS)
				{
					// Woops, error. Resend address and start over.
					pgs = RESEND_ADDR;
				}

				// If received ack.
				if (hasMessage && cm->getFunct() == FUNCT_BOOTLOADER && cm->getFuncc() == FUNCC_BOOT_ACK && cm->getNid() == MY_NID && cm->getSid() == TARGET_ID)
				{
					// If no error
					if (data[ERR_INDEX] == ERR_NO_ERROR)
					{
						currentAddress += 64;
						// Check if end.
						if (currentAddress > hf->getAddrUpper())
						{
							// Yes, we are done, send done.
							pgs = SEND_DONE;
						}
						else
						{ 
							// More to write.
							byteSent = 0;
							pgs = SEND_PGM_DATA;
						}
					}
					else
					{
						// else resend start..
						pgs = RESEND_ADDR;
					}
				}
			break;


			case SEND_DONE:
				// Send done
				outCm = &CanProtoMessage::CanProtoMessage(FUNCT_BOOTLOADER, FUNCC_BOOT_DONE, MY_NID, MY_ID, 8, data);
				outCm->getBytes(bytesToWrite);
				serial->write(bytesToWrite,PACKET_LENGTH);
				t = GetTickCount();
				pgs = WAIT_DONE;
			break;

			case RESEND_ADDR:
				// Resend addr.
				data[RID_INDEX] = TARGET_ID;
				data[ADDRU_INDEX] = (byte)((currentAddress & 0xFF0000) >> 16);
				data[ADDRH_INDEX] = (byte)((currentAddress & 0xFF00) >> 8);
				data[ADDRL_INDEX] = (byte)(currentAddress & 0xFF);
				outCm = &CanProtoMessage::CanProtoMessage(FUNCT_BOOTLOADER, FUNCC_BOOT_ADDR, MY_NID, MY_ID, 8, data);
				outCm->getBytes(bytesToWrite);
				serial->write(bytesToWrite,PACKET_LENGTH);
				t = GetTickCount();
				pgs = WAIT_ADDR_ACK;
			break;


			case WAIT_ADDR_ACK:
				// Check for timeout, resend addr packet in that case..
				if ((GetTickCount() - t) > TIMEOUT_MS)
				{
					pgs = RESEND_ADDR;
				}

				// If received ack.
				if (hasMessage && cm->getFunct() == FUNCT_BOOTLOADER && cm->getFuncc() == FUNCC_BOOT_ACK && cm->getNid() == MY_NID && cm->getSid() == TARGET_ID)
				{
					// If no error
					if (data[ERR_INDEX] == ERR_NO_ERROR)
					{
						// Start sending program data..
						byteSent = 0;
						pgs = SEND_PGM_DATA;
					}
					else
					{
						// else resend addr..
						pgs = RESEND_ADDR;
					}
				}
			break;

			case WAIT_DONE:
				// Check for timeout, resend done packet in that case..
				if ((GetTickCount() - t) > TIMEOUT_MS)
				{
					pgs = SEND_DONE;
				}

				// If received ack.
				if (hasMessage && cm->getFunct() == FUNCT_BOOTLOADER && cm->getFuncc() == FUNCC_BOOT_ACK && cm->getNid() == MY_NID && cm->getSid() == TARGET_ID)
				{
					// If no error
					if (data[ERR_INDEX] == ERR_NO_ERROR)
					{
						// Start sending program data..
						byteSent = 0;
						pgs = DONE;
					}
					else
					{
						// else resend addr..
						pgs = RESEND_ADDR;
					}
				}
			break;

			case DONE:
				downloadStop();
				printf("DONE!\n");
			break;
		}
	}

	return;
}


void downloadStart(SerialConnection * ser,HexFile *hex)
{
	hf = hex;
	serial = ser;
	hRunMutex = CreateMutex( NULL, TRUE, NULL );      // Set 
	_beginthread( downloadState, 0, 0 );

}

void downloadStop( void )
{
	ReleaseMutex( hRunMutex );
	CloseHandle( hRunMutex );
}

