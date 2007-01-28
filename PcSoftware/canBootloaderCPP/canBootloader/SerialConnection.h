#ifndef __SERIALCONNECTION_H
#define __SERIALCONNECTION_H

#include <crtdbg.h>
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include "CanProtoMessage.h"
#include "compiler.h"

#define SERIAL_INPUT_BUFFER_SIZE 1024
#define SERIAL_READ_BUFFER_SIZE 8192
#define QUEUE_PLACES 32


class SerialConnection
{
public:
	// Baudrate
	typedef enum
	{
		EBaudUnknown = -1,			// Unknown
		EBaud110     = CBR_110,		// 110 bits/sec
		EBaud300     = CBR_300,		// 300 bits/sec
		EBaud600     = CBR_600,		// 600 bits/sec
		EBaud1200    = CBR_1200,	// 1200 bits/sec
		EBaud2400    = CBR_2400,	// 2400 bits/sec
		EBaud4800    = CBR_4800,	// 4800 bits/sec
		EBaud9600    = CBR_9600,	// 9600 bits/sec
		EBaud14400   = CBR_14400,	// 14400 bits/sec
		EBaud19200   = CBR_19200,	// 19200 bits/sec (default)
		EBaud38400   = CBR_38400,	// 38400 bits/sec
		EBaud56000   = CBR_56000,	// 56000 bits/sec
		EBaud57600   = CBR_57600,	// 57600 bits/sec
		EBaud115200  = CBR_115200,	// 115200 bits/sec
		EBaud128000  = CBR_128000,	// 128000 bits/sec
		EBaud256000  = CBR_256000,	// 256000 bits/sec
	}
	EBaudrate;

	// Data bits (5-8)
	typedef enum
	{
		EDataUnknown = -1,			// Unknown
		EData5       =  5,			// 5 bits per byte
		EData6       =  6,			// 6 bits per byte
		EData7       =  7,			// 7 bits per byte
		EData8       =  8			// 8 bits per byte (default)
	}
	EDataBits;

	// Parity scheme
	typedef enum
	{
		EParUnknown = -1,			// Unknown
		EParNone    = NOPARITY,		// No parity (default)
		EParOdd     = ODDPARITY,	// Odd parity
		EParEven    = EVENPARITY,	// Even parity
		EParMark    = MARKPARITY,	// Mark parity
		EParSpace   = SPACEPARITY	// Space parity
	}
	EParity;

	// Stop bits
	typedef enum
	{
		EStopUnknown = -1,			// Unknown
		EStop1       = ONESTOPBIT,	// 1 stopbit (default)
		EStop1_5     = ONE5STOPBITS,// 1.5 stopbit
		EStop2       = TWOSTOPBITS	// 2 stopbits
	} 
	EStopBits;


public:
	SerialConnection(LPCTSTR portName,EBaudrate baudRate,EDataBits byteSize,EParity parity, EStopBits stopBits);
	~SerialConnection();
	bool open(void);
	bool close(void);
	bool isOpen(void);
	bool write(LPCVOID lpBuffer, unsigned int nNumberOfBytesToWrite);
	HANDLE getHandle(void);
	bool getMessage(CanProtoMessage *cpm);

protected:
	LPCTSTR uportName;
	EBaudrate ubaudRate;
	EDataBits ubyteSize;
	EParity uparity;
	EStopBits ustopBits;
	
};	


#endif