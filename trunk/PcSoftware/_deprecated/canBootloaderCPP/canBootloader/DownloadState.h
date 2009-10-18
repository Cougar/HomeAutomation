#ifndef __DOWNLOADSTATE_H
#define __DOWNLOADSTATE_H

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
//#include "Serial.h"
#include "compiler.h"
#include "CanProtoMessage.h"
#include "SerialConnection.h"
//#include "SerialReader.h"
#include "HexFile.h"

void downloadStart(SerialConnection * serial,HexFile *hex);
void downloadStop( void );

#endif