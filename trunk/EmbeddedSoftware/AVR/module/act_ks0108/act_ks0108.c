
#include "act_ks0108.h"

static unsigned char Splash_left[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,//Line 1 
0xff,0xff,0xff,0x3,0x7f,0x7f,0xbf,0xbf,0xdf,0x7,0xff,0xff,0xff,0x3f,0xbf,0xdf,0xdf,0xdf,0x3f,0xff,0xff,0xff,0x3f,0x3f,0xdf,0x9f,0x7f,0x7f,0xbf,0x9f,0x3f,0xff,0xff,0x7f,0xbf,0xdf,0xdf,0x9f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0xc7,0xf3,0xf,0xff,0xff,0xff,0x1f,0xff,0xff,0xff,0xff,0x3f,0xdf,0xff,0xef,0xef,0xef,0x40,0x5,0xf7,0xff,0xff,0x3f,0xbf,0xdf,0xdf,0xdf,0x3f,0xff,0xff,0xff,0x1f,0x7f,0xdf,0x9f,0x7f,0x7f,0x9f,0xdf,0x3f,0xff,0xff,0xbf,0xbf,0xdf,0xdf,0xdf,0x3f,0xff,0xff,0xef,0xef,0xef,0x40,0x5,0xf7,0xff,0x1d,0xff,0xff,0xff,0xff,0x3f,0xbf,0xdf,0xdf,0xdf,0x3f,0xff,0xff,0xff,0x1f,0xff,0x7f,0x9f,0xdf,0x3f,0xff,0xff,0xff,0xff,0xff,//Line 2 
0xff,0xff,0xff,0xd0,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,0xff,0xe0,0xef,0xdf,0xdf,0xcf,0xe3,0xf8,0xff,0xff,0xff,0xc0,0xfe,0xff,0xff,0xe0,0xfa,0xff,0xff,0xe0,0xff,0xff,0xf8,0xe6,0xee,0xee,0xf6,0xf7,0xfb,0xff,0xff,0xef,0xf3,0xfc,0xfe,0xfe,0xfe,0xff,0xf0,0xff,0xff,0xf4,0xcf,0xdf,0xe7,0xfb,0xf0,0xe7,0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,0xe0,0xef,0xdf,0xdf,0xcf,0xe3,0xf8,0xff,0xff,0xff,0xc0,0xfe,0xff,0xff,0xe0,0xfa,0xff,0xff,0xe0,0xff,0xff,0xf3,0xeb,0xed,0xed,0xf5,0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0xff,0xff,0xe0,0xff,0xff,0xff,0xf0,0xcf,0xdf,0xdf,0xcf,0xe3,0xf8,0xff,0xff,0xff,0xf4,0xf9,0xfe,0xff,0xff,0xe0,0xff,0xff,0xff,0xff,0xff,//Line 3 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,//Line 4 
0xff,0xff,0xff,0xff,0xff,0xdf,0xef,0xef,0xef,0x1f,0xff,0xff,0xf,0xff,0xff,0xff,0xff,0x3f,0xcf,0xff,0xff,0x1f,0x9f,0xef,0xcf,0x3f,0x3f,0xdf,0xcf,0x1f,0xff,0xff,0x40,0x2f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0x5f,0xef,0xef,0xef,0xff,0xff,0x3f,0x1f,0x6f,0x6f,0xcf,0x9f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3f,0xcf,0xf3,0xfc,0xff,0xff,0xff,0x47,0xb7,0xf7,0xf7,0xf7,0x77,0x8f,0xff,0xff,0xf,0x7f,0x3f,0xdf,0xdf,0xff,0xff,0x7f,0x9f,0xdf,0xef,0xef,0xef,0x1f,0xff,0xff,0xff,0xff,0xdd,0x3f,0xff,0xff,0xff,0x3f,0x1f,0x6f,0x6f,0xcf,0x9f,0xff,0xff,0xfd,0x3,0x7f,0xbf,0xdf,0xef,0xff,0xff,0xf7,0xf7,0xf7,0xa0,0x2,0xfb,0xff,0xff,0xff,//Line 5 
0xff,0xff,0xff,0xff,0xf9,0xf6,0xf6,0xfa,0xfa,0xf8,0xff,0xff,0xf8,0xe7,0xef,0xf3,0xfd,0xfc,0xf1,0xff,0xff,0xe0,0xff,0xff,0xff,0xf0,0xfd,0xff,0xff,0xf0,0xff,0xff,0xff,0xf8,0xff,0xff,0xff,0xcf,0xc3,0xd3,0xe3,0xff,0xff,0xff,0xf7,0xe7,0xee,0xf6,0xf1,0xff,0xff,0xfc,0xf3,0xf7,0xf7,0xfb,0xfb,0xfd,0xff,0xff,0xff,0xcf,0xf3,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x80,0x5b,0xf9,0xfd,0xfe,0xff,0xff,0xff,0xf4,0xf8,0xff,0xff,0xff,0xff,0xff,0xf8,0xe7,0xef,0xef,0xe7,0xf1,0xfc,0xff,0xff,0xcf,0xbf,0xbf,0xc0,0xff,0xff,0xff,0xfc,0xfb,0xf7,0xf7,0xfb,0xfb,0xfd,0xff,0xff,0xf8,0xfd,0xfd,0xfb,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,0xff,0xff,0xff,//Line 6 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,//Line 7 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0//Line 7 
};




#define SOLID	0
#define TRANSPARENT 1
uint8_t color=GLCD_COLOR_BLACK;
uint8_t Transparent=SOLID;

#if act_ks0108_USE_EXTERNAL_EEPROM==1

	void storeImage(uint8_t id, uint8_t size_x , uint8_t size_y);

	void saveImageFromScreen(uint8_t id, uint8_t size_x , uint8_t size_y);

	#define MEMORY_I2C_ADDR  0x57	// 64K EEPROM - allowed addresses are 0x50 to 0x57
	#define MEMORY_ADDR_MSB  0		// start at base of memory. Next one is 0x01.
	#define MEMORY_ADDR_LSB  0		// start on a page boundary. Next one is 0x80.
	unsigned char messageBuf[MESSAGEBUF_SIZE];
#endif


void numtoascii( int16_t num, char **str );
void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);
void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);

#if act_ks0108_USE_EXTERNAL_EEPROM==1
//valid id is 1 to 255;
void printImage(uint8_t id, uint8_t x_start, uint8_t y_start) {
	if (id == 0 )
		return; 	//id=0 not allowed
	uint16_t address = 0;
	uint16_t buf_index = 0;
	uint8_t index;
	uint8_t size_x = 0;
	uint8_t size_y = 0;
	uint16_t max_picture_size;
	index = (id>>5)&0x07;
	//find starting adress of the requested data
	switch (index) {
		case 0:
			address = (index)*1024;
			max_picture_size = 1024;
			break;
		case 1:
			address = 32768+(index)*512;
			max_picture_size = 512;
			break;
		case 2:
			address = 32768+32*512+(index)*256;
			max_picture_size = 256;
			break;
		case 3:
			address = 32768+32*512+32*256+(index)*128;
			max_picture_size = 128;
			break;
		case 4:
			address = 32768+32*512+32*256+32*128+(index)*64;
			max_picture_size = 64;
			break;
		case 5:
			address = 32768+32*512+32*256+32*128+32*64+(index)*32;
			max_picture_size = 32;
			break;
		case 6:
			address = 32768+32*512+32*256+32*128+32*64+32*32+(index)*16;
			max_picture_size = 16;
			break;
		case 7:
			address = 32768+32*512+32*256+32*128+32*64+32*32+32*16+(index)*16;
			max_picture_size = 16;
			break;
	}
	//Get size of picture (every picture has an 4 byte info-tag)
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
	messageBuf[1] = (uint8_t)(0);       // Starting address in memory
	messageBuf[2] = (uint8_t)(((id&0xe0)>>5) & 0xff);       	// Starting address in memory
	TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
	// Could do other actions in here, then get data.
	TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
	size_x = messageBuf[((id&0x1f)<<2)];
	size_y = messageBuf[((id&0x1f)<<2)+1];
	//more_data = messageBuf[((id&0x1f)<<2)+2];
	//last_data = messageBuf[((id&0x1f)<<2)+3];

	// get data from eeprom
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
	messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
	messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
	TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
	// Could do other actions in here, then get data.
	TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
	uint8_t jy = 0;
	uint8_t jx = 0;
	buf_index = 0;
	for (jy = 0; jy< size_y; jy+=8){
		glcdSetXY(x_start,y_start+jy);
		for (jx = 0; jx< size_x ; jx++){
			if (buf_index>=128) {	//get new data if buffer empty
				address +=128;
				messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
				messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
				messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
				TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
				// Could do other actions in here, then get data.
				TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
				buf_index=0;
			}
			glcdWriteData(messageBuf[buf_index+1], GLCD_COLOR_CLEAR);
			buf_index++;
		}
	}
}
volatile uint8_t state;
volatile uint8_t lastId = 255;
#define IDLE	0
#define RECEIVING_DATA 1
void newImageData(StdCan_Msg_t *rxMsg) {
	//State machine for storing new data
	switch (state) {
		case IDLE:
			if (rxMsg->Header.Command == CAN_MODULE_CMD_KS0108_LCD_NEW_IMAGE)
			{
				storeImage(rxMsg->Data[0],rxMsg->Data[1],rxMsg->Data[2]);
				state = RECEIVING_DATA;
				lastId = 255;
				//response message
				rxMsg->Length = 1;
				rxMsg->Header.Command = CAN_MODULE_CMD_KS0108_LCD_ACK;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);	  
			}else {
				//error message
				rxMsg->Length = 0;
				rxMsg->Header.Command = CAN_MODULE_CMD_KS0108_LCD_ERROR;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);	  
			}
			break;
		case RECEIVING_DATA:
			if (rxMsg->Header.Command == CAN_MODULE_CMD_KS0108_LCD_IMAGE_DATA)
			{
				if (rxMsg->Data[0] == (lastId+1)) {
					storeImage(rxMsg->Data[0],rxMsg->Data[1],rxMsg->Data[2]);
					state = RECEIVING_DATA;
					lastId++;

					//FIXME: call function that handles the data

					//response message
					rxMsg->Length = 1;
					rxMsg->Header.Command = CAN_MODULE_CMD_KS0108_LCD_ACK;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);	  
				} else {
					//wrong id
					rxMsg->Length = 1;
					rxMsg->Header.Command = CAN_MODULE_CMD_KS0108_LCD_DATA_ID_ERROR;
					rxMsg->Data[0] = (lastId+1);
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);	  
				}
			} else if (rxMsg->Header.Command == CAN_MODULE_CMD_KS0108_LCD_DATA_DONE) {
				//Call function to flush eeprom cache
				//FIXME
				state=IDLE;
			} else {
				//error message
				rxMsg->Length = 0;
				rxMsg->Header.Command = CAN_MODULE_CMD_KS0108_LCD_ERROR;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);	  
			}
			break;
	}

}


//valid id is 1 to 255;
void storeImage(uint8_t id, uint8_t size_x , uint8_t size_y) {
	if (id == 0 )
		return; 	//id=0 not allowed
	//uint16_t address = 0;
	//uint16_t buf_index = 0;
	uint8_t index;
	//uint16_t max_picture_size;
	index = (id>>5)&0x07;
	//find starting adress of the requested data
	//Get size of picture (every picture has an 4 byte info-tag)
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
	messageBuf[1] = (uint8_t)(0);       // Starting address in memory
	messageBuf[2] = (uint8_t)(((id&0xe0)>>5) & 0xff);       	// Starting address in memory
	TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
	// Could do other actions in here, then get data.
	TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
	messageBuf[((id&0x1f)<<2)+1] = size_x;
	messageBuf[((id&0x1f)<<2)+2] = size_y;
	//more_data = messageBuf[((id&0x1f)<<2)+2];
	//last_data = messageBuf[((id&0x1f)<<2)+3];

	//move data in the buffer
	for (index = 128; index > 0; index--) {
		messageBuf[index+2] = messageBuf[index];
	}
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = (uint8_t)(0);       // Starting address in memory
	messageBuf[2] = (uint8_t)(((id&0xe0)>>5) & 0xff);       	// Starting address in memory
	TWI_Start_Read_Write( messageBuf, 131 );
	// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
	while ( TWI_Transceiver_Busy() );    // Check for an error.

	//Now the info-tag is written, only to add data left

	//FIXME........ FIX!!!

}


//valid id is 1 to 255;
void saveImageFromScreen(uint8_t id, uint8_t size_x , uint8_t size_y) {
	if (id == 0 )
		return; 	//id=0 not allowed
	uint16_t max_picture_size;
	uint8_t index;
	uint8_t bufferIndex;
	uint16_t address = 0;
	index = (id>>5)&0x07;
	//find starting adress of the requested data
	switch (index) {
		case 0:
			address = (index)*1024;
			max_picture_size = 1024;
			break;
		case 1:
			address = 32768+(index)*512;
			max_picture_size = 512;
			break;
		case 2:
			address = 32768+32*512+(index)*256;
			max_picture_size = 256;
			break;
		case 3:
			address = 32768+32*512+32*256+(index)*128;
			max_picture_size = 128;
			break;
		case 4:
			address = 32768+32*512+32*256+32*128+(index)*64;
			max_picture_size = 64;
			break;
		case 5:
			address = 32768+32*512+32*256+32*128+32*64+(index)*32;
			max_picture_size = 32;
			break;
		case 6:
			address = 32768+32*512+32*256+32*128+32*64+32*32+(index)*16;
			max_picture_size = 16;
			break;
		case 7:
			address = 32768+32*512+32*256+32*128+32*64+32*32+32*16+(index)*16;
			max_picture_size = 16;
			break;
	}
	//Get size of picture (every picture has an 4 byte info-tag)
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
	messageBuf[1] = (uint8_t)(0);       // Starting address in memory
	messageBuf[2] = (uint8_t)(((id&0xe0)>>5) & 0xff);       	// Starting address in memory
	TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
	// Could do other actions in here, then get data.
	TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
	messageBuf[((id&0x1f)<<2)+1] = size_x;
	messageBuf[((id&0x1f)<<2)+2] = size_y;

	//move data in the buffer
	for (index = 128; index > 0; index--) {
		messageBuf[index+2] = messageBuf[index];
	}
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = (uint8_t)(0);       // Starting address in memory
	messageBuf[2] = (uint8_t)(((id&0xe0)>>5) & 0xff);       	// Starting address in memory
	TWI_Start_Read_Write( messageBuf, 131 );
	// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
	while ( TWI_Transceiver_Busy() );    // Check for an error.

	//Now the info-tag is written, only to add data left
	uint8_t  h, i ;
		

	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
	messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
	bufferIndex = 3;

	h = 0;
	while(h+8 <= size_y) {
		h += 8;
		glcdSetXY(0, h);
		
		for(i=0; i<=size_x; i++) {
			messageBuf[bufferIndex] = glcdReadData();
			bufferIndex++;
			if (bufferIndex >= 131) {
				TWI_Start_Read_Write( messageBuf, 131 );
				// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
				while ( TWI_Transceiver_Busy() );    // Check for an error.
				bufferIndex = 3;
				address+=128;
				messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
				messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
				messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
			}
		}
	}
}
#endif

void act_ks0108_Init(void)
{
	/* Set up PWM controlling brightness */
#if GRAPHICS_DRIVER==KS0108
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0A = act_ks0108_INITIAL_BACKLIGHT;
	DDRD |= (1<<PD6);
#endif
#if GRAPHICS_DRIVER==DOTMATRIX
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0xff-act_ks0108_INITIAL_BACKLIGHT;
	DDRD |= (1<<PD5);
	
	/* Start timer for row management */
	Timer_SetTimeout(act_ks0108_DOTMATRIX_TIMER, 1, TimerTypeFreeRunning, &glcdRefresh);	
#endif


#if act_ks0108_USE_EXTERNAL_EEPROM==1
	TWI_Master_Initialise();
	uint8_t temp = 0;
	///	Do initial write to EEPROM. Write cycle takes 5 msecs, so flashing the led will delay enough.
	//
	
	messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] =	(((1024+128)>>8)&0xff);				// Starting address MSB
	messageBuf[2] =	((1024+128)&0xff);				// Starting address LSB
	
	for (temp=0; temp<128; temp++)
	{
		messageBuf[temp+3] = 128-temp;	
	}
	TWI_Start_Read_Write( messageBuf, 131 );
	// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
	while ( TWI_Transceiver_Busy() );    // Check for an error.
	
	//delay 10ms
	uint32_t temp2;
	temp2 =Timer_GetTicks();
	while (Timer_GetTicks() < temp2 + 10);

	#define TEMPID 0
	uint16_t address = (TEMPID+1)*1024;	//only addresses on page border is allowed
	uint16_t ixa = 0;
	uint8_t jxa = 0;
	for (jxa = 0; jxa< 8; jxa++){
		for (ixa = 0; ixa < 128; ixa++){
			messageBuf[ixa+3] = (((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128)) ^ 0xff);
		}
		messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
		messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
		TWI_Start_Read_Write( messageBuf, 131 );
		// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
		while ( TWI_Transceiver_Busy() );    // Check for an error.
		//delay 10ms
		temp2 =Timer_GetTicks();
		while (Timer_GetTicks() < temp2 + 10);
		address += 128;
	}

	address = (TEMPID+2)*1024;	//only addresses on page border is allowed
	ixa = 0;
	jxa = 0;
	for (jxa = 0; jxa< 8; jxa++){
		for (ixa = 0; ixa < 128; ixa++){
			messageBuf[ixa+3] = (uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128);
		}
		messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		messageBuf[1] = (uint8_t)((address >> 8) & 0xff);       // Starting address in memory
		messageBuf[2] = (uint8_t)(address & 0xff);       // Starting address in memory
		TWI_Start_Read_Write( messageBuf, 131 );
		// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
		while ( TWI_Transceiver_Busy() );    // Check for an error.
		//delay 10ms
		temp2 =Timer_GetTicks();
		while (Timer_GetTicks() < temp2 + 10);
		address += 128;
	}
	for (temp=0; temp<131; temp++)
	{
		messageBuf[temp] = 0;	
	}

#endif

	glcdInit();

#if act_ks0108_USE_EXTERNAL_EEPROM==0
	uint16_t ixa = 0;
	uint8_t jxa = 0;
	for (jxa = 0; jxa< 8; jxa++){
		glcdSetXY(0,jxa*8);
		for (ixa = 0; ixa < 128; ixa++){
			glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128), GLCD_COLOR_CLEAR);
		}
	}
#endif

#if act_ks0108_USE_EXTERNAL_EEPROM==1
	printImage(1,0,0);
	//delay 10ms
	temp2 =Timer_GetTicks();
	while (Timer_GetTicks() < temp2 + 5000);
	printImage(0,0,0);
#endif
	//glcdSetXY(0,0);
	//glcdPutStrTransparent("=_-[]_-=",GLCD_COLOR_SET);
	//glcdSetXY(0,8);
	//glcdPutStrTransparent("--------",GLCD_COLOR_SET);
	//glcdSetXY(0,16);
	//glcdPutStr("-_=[]=-_",GLCD_COLOR_SET);
	//glcdDrawRect(32, 32, 40, 16, GLCD_COLOR_SET);
	//glcdDrawLine(10, 10, 100, 60, GLCD_COLOR_SET);
	//glcdFillRect(90, 10, 20, 20, GLCD_COLOR_SET);
	//glcdInvertRect(28, 10, 20, 20);
	//glcdInvert();
	//glcdDrawRoundRect(85, 5, 30, 30, 5, GLCD_COLOR_SET);
	//glcdDrawCircle(64, 32, 10, 1);
	glcdSetXY(87,8);
	glcdPutStrTransparent("Home-",GLCD_COLOR_SET);
	glcdSetXY(80,16);
	glcdPutStrTransparent("Automa-",GLCD_COLOR_SET);
	glcdSetXY(91,24);
	glcdPutStrTransparent("ion",GLCD_COLOR_SET);
	//glcdDrawLine(10, 62, 50, 5, GLCD_COLOR_CLEAR);
	
}

void act_ks0108_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void act_ks0108_HandleMessage(StdCan_Msg_t *rxMsg)
{
	//StdCan_Msg_t txMsg;
	uint8_t n = 0;

	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_KS0108 &&
		rxMsg->Header.ModuleId == act_ks0108_ID)
	{
	switch (rxMsg->Header.Command)
	{
		case CAN_MODULE_CMD_KS0108_LCD_CLEAR:
		  if (rxMsg->Length == 1) {
			glcdSetColor((0x80&rxMsg->Data[0])>>7);
		  }
		  glcdClear();
		  rxMsg->Data[0] = glcdGetColor()<<7;
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 1;
		  #if act_ks0108_USE_EXTERNAL_EEPROM==1
			  uint8_t temp = 0;
			  //	Read the data from Memory. Value of read/write bit doesn't matter.
				messageBuf[0] = (MEMORY_I2C_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
				messageBuf[1] = MEMORY_ADDR_MSB;       // Starting address in memory
				messageBuf[2] = MEMORY_ADDR_LSB;       // Starting address in memory
				TWI_Start_Random_Read( messageBuf, 129, 3 );	// Desired data length plus one (command byte).
				// Could do other actions in here, then get data.
				temp = TWI_Read_Data_From_Buffer( messageBuf, 129 ); 
			  rxMsg->Length = 6;
			  rxMsg->Data[1] = temp;
			  rxMsg->Data[2] = messageBuf[0+3];
			  rxMsg->Data[3] = messageBuf[0+4];
			  rxMsg->Data[4] = messageBuf[0+5];
			  rxMsg->Data[5] = messageBuf[0+80];
		  #endif
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		  break;
		case CAN_MODULE_CMD_KS0108_LCD_INVERT:
		  if (rxMsg->Length == 1) {
			if ((0x80&rxMsg->Data[0])>>7 != glcdGetColor())
				glcdInvert();
		  } else
			glcdInvert();
		  rxMsg->Data[0] = glcdGetColor()<<7;
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 1;
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		  break;
		case CAN_MODULE_CMD_KS0108_LCD_CURSOR:
		  if (rxMsg->Length == 2) {
			glcdSetXY((0x3f&rxMsg->Data[0])*6, rxMsg->Data[1]*8);
			color = ((0x80&rxMsg->Data[2])>>7);
			Transparent = ((0x40&rxMsg->Data[2])>>6);
		  }
  		  rxMsg->Data[0] = glcdGetColor()<<7;
		  rxMsg->Data[0] = Transparent<<6;
		  rxMsg->Data[0] |= (0x3f & (glcdGetX()/6));
		  rxMsg->Data[1] |= ((glcdGetY()/8));
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 2;
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);

		  break;

		case CAN_MODULE_CMD_KS0108_LCD_TEXTAT:
		  glcdSetXY((0x3f&rxMsg->Data[0])*6, rxMsg->Data[1]*8);
		  for (n = 2; n < rxMsg->Length; n++)
		  {
			  if (((0x40&rxMsg->Data[0])>>6)== TRANSPARENT)
				glcdWriteCharTransparent((char)rxMsg->Data[n], (0x80&rxMsg->Data[0])>>7);
			  else
				glcdWriteChar((char)rxMsg->Data[n], (0x80&rxMsg->Data[0])>>7);
		  }
		break;

		case CAN_MODULE_CMD_KS0108_LCD_TEXT:
		for (n = 0; n < rxMsg->Length; n++)
		{
			if (Transparent == TRANSPARENT)
				glcdWriteCharTransparent((char)rxMsg->Data[n], color);
			else
				glcdWriteChar((char)rxMsg->Data[n], color);
		}
		break;

		case CAN_MODULE_CMD_KS0108_LCD_BACKLIGHT:
			if (rxMsg->Length > 0) {
				if ( rxMsg->Data[0] == 0) {
					TCCR0A &= ~(1<<COM0A0);
					TCCR0A &= ~(1<<COM0A1);
					PORTD &= ~(1<<PD6);
					OCR0A = rxMsg->Data[0];
				} else {
					TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
					OCR0A = rxMsg->Data[0];
				}

			}
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_KS0108;
			txMsg.Header.ModuleId = act_ks0108_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_KS0108_LCD_BACKLIGHT;
			txMsg.Length = 1;
			txMsg.Data[0] = OCR0A;
			while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		break;
		
		case CAN_MODULE_CMD_KS0108_LCD_DRAWRECT:
		  if ((0x40&rxMsg->Data[0])>>6)
			glcdFillRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
		  else
			if (rxMsg->Data[5] == 0)
				glcdDrawRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
			else
				glcdDrawRoundRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], rxMsg->Data[5],(0x80&rxMsg->Data[0])>>7);
		  break;
		
		case CAN_MODULE_CMD_KS0108_LCD_DRAWLINE:
		  glcdDrawLine(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
		  break;

		case CAN_MODULE_CMD_KS0108_LCD_DRAWCIRCLE:
		  glcdDrawCircle(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], (0x80&rxMsg->Data[0])>>7);
		  break;
	
		case CAN_MODULE_CMD_KS0108_LCD_INVERTRECT:
		  glcdInvertRect(rxMsg->Data[0], rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3]);
		  break;

		}
	}
/*
	static uint8_t saer=0;
	static uint8_t saer2=0;
	saer++;
	if (saer>10) {
		saer2++;
		uint8_t ixa = 0;
		uint8_t jxa = 0;
		for (jxa = 0; jxa< 8; jxa++){
			glcdSetXY(0,jxa);
			for (ixa = 0; ixa < 128; ixa++){
				glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128));
			}
		}
		saer=0;
		glcdSetXY(83,1);
		glcdPutStr("Tycker ");
		glcdSetXY(83,2);
		glcdPutStr(" detta ");
		glcdSetXY(83,3);
		glcdPutStr("funkar!");
		glcdSetXY(83,4);
		char buffer[20];	
		//numtoascii((int16_t)saer2 ,*buffer);
		unsignedtoascii((int16_t)saer2,0,buffer,1);
		glcdPutStr(buffer);
	}
*/
}

void act_ks0108_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_KS0108;
	txMsg.Header.ModuleId = act_ks0108_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

//� la pengi. Skall libbifieras.
	void numtoascii( int16_t num, char **str ) {
		if( num==0 ) return;
		numtoascii( num/10, str );
		**str = '0' + (num%10);
		(*str)++;
	}

	void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;

		if( num<0 ) {
			*(string++) = '-';
			num = -num;
		}
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) {
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}

	void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) {
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}
