/*
File: bmp2lcd.c 
Function: converts 128*64 black and white (1 bit per pixel) bitmap images to .lcd binary files
the bytes in .lcd file are arranged to match the paging scheme used by KS0108 or HD61202 controlled
graphic LCDs.
Author: Dincer Aydin (dinceraydin@altavista.net) 
Modified by Martin Nordén to output as array
Please note that this software is presented as is,
 without any warranty of any kind. Use it at you own risk !!  */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
typedef unsigned char byte;
#define SIZEBMP 1086
#define SIZELCD 1024
static int i;
void convert(void);
//byte getvertByte(int ,int bit); 
 unsigned char getbits(unsigned x, int p, int n);
  byte count, arrayLCD[SIZELCD], arrayBmp[SIZEBMP];
 main(int argc, char *argv[])
{	 
	FILE *fp;
	if(argc > 3)
	{
		printf("too many argumets...\n");
		printf("usage: bmp2lcd fileName.bmp fileName.lcd\n");
		return (0);	
	}
	else if(argc < 3)
	{
		printf("missing argumets...\n");
		printf("usage: bmp2lcd fileName.bmp fileName.lcd\n");
		return (0);	
	}		  

  /* open the bitmap for reading in binary mode  */
 
     if ( (fp = fopen(*++argv, "rb")) == NULL)
     {
         fprintf(stderr, "Error opening file.");
         exit(1);
     }
 
     /* Read the bitmap into arrayBmp[]. */
 
     if (fread(arrayBmp, sizeof( byte), SIZEBMP, fp) != SIZEBMP)
     {
         fprintf(stderr, "Error reading file.");
         exit(1);
     }
 
     fclose(fp);
/*-------------------------------------------------------------*/
/*-----between goes the conversion---------------------------*/

	convert();	
/*-------------------------------------------------------------*/

 /* Open a binary mode file. */

    if ( (fp = fopen(*++argv, "wb")) == NULL)
    {
        fprintf(stderr, "Error opening file.");
        exit(1);
    }
    /* Save arrayLCD[] to the file. */

    //if (fwrite(arrayLCD, sizeof( byte), SIZELCD, fp) != SIZELCD)
    //{
    //    fprintf(stderr, "Error writing to file.");
    //    exit(1);
    //}
	fprintf(fp,"static unsigned char PictArray[] PROGMEM = {\n");
	for (i=0; i<SIZELCD-1;i++)
	{
		fprintf(fp,"0x%x,",arrayLCD[i]);
		if ((i+1)%128 == 0){
			fprintf(fp,"//Line %0x \n",(i+1)/128);
		}
	}
		fprintf(fp,"0x%x//Line 7 \n};",arrayLCD[SIZELCD]);
	
	
    fclose(fp);
    return(0);
}

void convert(void)
{
	byte i,j,k,bmpCol,page,bitPos,dByte = 0;
	byte oneBlock[8];	
	for(page=0;page<8;page++)
	{
		for(bmpCol=0;bmpCol<16;bmpCol++)
		{
			for(i=0;i<8;i++)
				oneBlock[i] = arrayBmp[SIZEBMP -1 - i*16 - page*16*8 - bmpCol];
			for(bitPos=0;bitPos<8;bitPos++)
			{	
				dByte = 0;
				for(i=0;i<8;i++)
				{	
					if(getbits(oneBlock[i], bitPos, 1))
					dByte += (byte)pow((double)2, (double)i);	
				}  
				arrayLCD[(page * 128) + 127 - (bmpCol * 8) - bitPos]= ~dByte;
			}
		}
	}
}

/* getbits:  get n bits from position p */
 unsigned char getbits(unsigned x, int p, int n)
   {
       return (x >> (p+1-n)) & ~(~0 << n);
   }