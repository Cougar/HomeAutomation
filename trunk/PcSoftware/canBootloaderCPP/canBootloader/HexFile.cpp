#define STRICT
#include "HexFile.h"
 

HexFile::HexFile()
{
	// Constructior
}

HexFile::~HexFile()
{
	//Destructor
}

bool HexFile::isValid()
{
	// Returns if file is a valid hex file.
	// TBD!

	return true;
}

bool HexFile::loadHex(char filepath[], bool isMC18)
{
	// Populate buffer

	unsigned char lineDataCount = 0;
	unsigned long lineAddrHigh = 0;
	unsigned long lineAddr = 0;
	unsigned char lineCode = 0;
	unsigned long fullAddr = 0;

	

	strcpy_s(HexFile::filepath,FILE_PATH_SIZE,filepath);
	HexFile::length = 0;
	HexFile::addrLower = 0x7FFFFFFF;
	HexFile::addrUpper = 0;

	// Prime the buffer
	for (int i = 0; i < BUFFER_SIZE; i++) buff[i] = 0xFF;


	// Try opening file
	ifstream a_file (filepath);

	if ( !a_file.is_open() ) 
	{
		// The file could not be opened
		strcpy_s(lastError,ERROR_BUFFER_SIZE,"Err! Unable to open file.");
		return false;
	}
	else 
	{
		// Safely use the file stream

		char oneLine[LINE_MAX_SIZE];

		// For each line, read until eof.
		while (!a_file.eof())
		{
			// read one line
			a_file.getline(&oneLine[0],LINE_MAX_SIZE);
			
            // If valid line
			if (strcmp(oneLine,"")!=0 && oneLine[0]==':')
			{

				lineDataCount = (unsigned char)HexFile::hexToLong(oneLine,1,2); 
				lineAddr = (unsigned long)HexFile::hexToLong(oneLine,3,4) & 65535;      
				lineCode = (unsigned char)HexFile::hexToLong(oneLine,7,2);     
		
				fullAddr = (lineAddrHigh * 65536) + lineAddr;
			
				switch(lineCode)
				{
                        case 0:
							HexFile::length +=lineDataCount;
							if ((fullAddr + lineDataCount - 1) > HexFile::addrUpper) HexFile::addrUpper = fullAddr + lineDataCount - 1;
                            if (lineCode == 0 && fullAddr < HexFile::addrLower) HexFile::addrLower = fullAddr;
							for (int j = 0; j < lineDataCount; j++)
                            {
                                buff[fullAddr] = (unsigned char)HexFile::hexToLong(oneLine,j * 2 + 9, 2); //byte.Parse(oneLine.Substring(j * 2 + 9, 2), System.Globalization.NumberStyles.HexNumber);
                                buff[fullAddr] &= 255; //(byte)255;
                                fullAddr++;
                            }
                            break;
                        case 4:
                            lineAddrHigh = (unsigned long)HexFile::hexToLong(oneLine,9, 4) & 65535; //long.Parse(oneLine.Substring(9, 4), System.Globalization.NumberStyles.HexNumber) & 65535;
                            break;
                        case 1: break; break;
                    
				}

			}
		
		}
		
	}

	// Force lower addr to start
	HexFile::addrLower = USER_RESET_ADDR;

	return true;
}


unsigned long HexFile::getAddrUpper(void) { return addrUpper; }

unsigned long HexFile::getAddrLower(void) { return addrLower; }

unsigned long HexFile::getLength(void) { return length; }

unsigned char HexFile::getByte(unsigned long addr)
{
	if (addr>=BUFFER_SIZE) return 0xFF;
	return HexFile::buff[addr];
}

// hex to integer
unsigned long HexFile::hexToLong(char *str,unsigned int start,unsigned int length)
{
	unsigned long lng = 0;

	for (unsigned int i=0;i<length;i++)
	{
		unsigned int newi=i+start;
		if (newi>=strlen(str)) break;

		if(str[newi]>='0' && str[newi]<='9')     { lng +=(unsigned long)pow(16.0,(double)(length-i-1))*(str[newi]-'0'+00); }
		else if(str[newi]>='a' && str[newi]<='f'){ lng +=(unsigned long)pow(16.0,(double)(length-i-1))*(str[newi]-'a'+10); }
		else if(str[newi]>='A' && str[newi]<='F'){ lng +=(unsigned long)pow(16.0,(double)(length-i-1))*(str[newi]-'A'+10); }
		else { lng = 0; break; }
	} 

	return lng;
}