#ifndef __SERIALREADER_H
#define __SERIALREADER_H

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include "Serial.h"
#include "compiler.h"
#include "CanProtoMessage.h"
#include "SerialConnection.h"

#define SERIAL_INPUT_BUFFER_SIZE 1024
#define SERIAL_READ_BUFFER_SIZE 8192
#define QUEUE_PLACES 32
/*
void serialReaderStart(CSerial * str);
void serialReaderStartSerCon(SerialConnection * ser);
void serialReaderStop( void );
*/
//bool getMessage(CanProtoMessage *cpm);

#endif