#ifndef __HEXFILE_H
#define __HEXFILE_H

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;

#define USER_RESET_ADDR 0x1000
#define BUFFER_SIZE 5200000
#define LINE_MAX_SIZE 1024
#define ERROR_BUFFER_SIZE 256
#define FILE_PATH_SIZE 256

class HexFile
{
public:
	bool isValid(void);
	HexFile();
	~HexFile();
	bool loadHex(char filepath[],bool isMC18);
	unsigned long getAddrUpper(void);
	unsigned long getAddrLower(void);
	unsigned long getLength(void);
	unsigned char getByte(unsigned long addr);
	char lastError[ERROR_BUFFER_SIZE];
	char filepath[FILE_PATH_SIZE];
protected:
	unsigned long hexToLong(char *str,unsigned int start,unsigned int length);
	unsigned char buff[BUFFER_SIZE];
	unsigned long addrUpper;
	unsigned long addrLower;
	unsigned long length;
};

#endif