#include <ks0108.h>
#include <avr/io.h>
#include <font.h>
#include <avr\pgmspace.h>

GrLcdStateType GrLcdState;


void SetControls(uint8_t RS, uint8_t RW){
	if(RS) {LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_RS);} else { LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_RS);};
	if(RW) {LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_RW);} else { LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_RW);};
}

void SetData(uint8_t Data){
	if(Data&0b00000001) {LCD_DATA_PORT_DB0 |= (1<<LCD_DATA_PIN_DB0);} else { LCD_DATA_PORT_DB0 &= ~(1<<LCD_DATA_PIN_DB0);};	
	if(Data&0b00000010) {LCD_DATA_PORT_DB1 |= (1<<LCD_DATA_PIN_DB1);} else { LCD_DATA_PORT_DB1 &= ~(1<<LCD_DATA_PIN_DB1);};	
	if(Data&0b00000100) {LCD_DATA_PORT_DB2 |= (1<<LCD_DATA_PIN_DB2);} else { LCD_DATA_PORT_DB2 &= ~(1<<LCD_DATA_PIN_DB2);};	
	if(Data&0b00001000) {LCD_DATA_PORT_DB3 |= (1<<LCD_DATA_PIN_DB3);} else { LCD_DATA_PORT_DB3 &= ~(1<<LCD_DATA_PIN_DB3);};	
	if(Data&0b00010000) {LCD_DATA_PORT_DB4 |= (1<<LCD_DATA_PIN_DB4);} else { LCD_DATA_PORT_DB4 &= ~(1<<LCD_DATA_PIN_DB4);};	
	if(Data&0b00100000) {LCD_DATA_PORT_DB5 |= (1<<LCD_DATA_PIN_DB5);} else { LCD_DATA_PORT_DB5 &= ~(1<<LCD_DATA_PIN_DB5);};	
	if(Data&0b01000000) {LCD_DATA_PORT_DB6 |= (1<<LCD_DATA_PIN_DB6);} else { LCD_DATA_PORT_DB6 &= ~(1<<LCD_DATA_PIN_DB6);};	
	if(Data&0b10000000) {LCD_DATA_PORT_DB7 |= (1<<LCD_DATA_PIN_DB7);} else { LCD_DATA_PORT_DB7 &= ~(1<<LCD_DATA_PIN_DB7);};	
}


void Disable(){
	LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_E);
}

void Delay(){ //OBS!! BEHÖVER VEEERKLIGEN FIXAS!
           // 1-8us      ...2-13us     ...5-31us
                               // 10-60us    ...50-290us
unsigned int i;             // 100-580us  ...500-2,9ms
unsigned char j;            // 1000-5,8ms ...5000-29ms
uint8_t p = 10;                             // 10000-56ms ...30000-170ms
                             // 50000-295ms...60000-345ms
//  for (i = 0; i < p; i++) for (j = 0; j < 10; j++) asm volatile ("nop");
    for (i = 0; i < p; i++) for (j = 0; j < 10; j++);

}
void Enable(){
	Delay();
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_E);
	Delay();
	Disable();
	Delay();
}


void glcdWriteData(uint8_t data){
	SetControls(1,0);
	SetData(data);
	Enable();
	GrLcdState.lcdXAddr++;

	if (GrLcdState.lcdXAddr > 63){
		glcdSetXY(GrLcdState.lcdXAddr,GrLcdState.lcdYAddr);
	}
	if (GrLcdState.lcdXAddr > 128){
		glcdSetXY(0,GrLcdState.lcdYAddr+1);
	}
}

void glcdPowerOn(){
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_CS2)|(1<<LCD_CONTROL_PIN_CS1);
	Disable();
	Delay();
	SetControls(0,0);
	SetData(0b00111111);
	Enable();
	
	uint8_t i;
	uint8_t j;

	for(j = 0; j < 8; j++){
		SetControls(0,0);
		SetData(0b10111000 + j);
		Enable();

		SetControls(0,0);
		SetData(0b01000000);
		Enable();
		for (i = 0; i < 64; i++){
			SetControls(1,0);
			SetData(0x00);
			Enable();
		}
	}

}

void glcdSetXY(uint8_t x, uint8_t y){
	GrLcdState.lcdXAddr = x;
	GrLcdState.lcdYAddr = y;

	//Vi börjar med X. Steg 1: Välj rätt chip:
	if (x > 63){
		LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_CS2);
		LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_CS1);
	} else {
		LCD_CONTROL_PORT |= (1<<LCD_CONTROL_PIN_CS1);
		LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_PIN_CS2);	
	}
	
	//Steg 2: Sätt rätt x-adress på det aktiva chippet
	SetControls(0,0);
	SetData(0b01000000 + x%64);

	Enable();

	//Steg 3: Sätt rätt y-adress på det aktiva chippet
	SetData(0b10111000 + y);
	Enable();
}

void glcdWriteChar(unsigned char c)
{
uint8_t i = 0;
if (GrLcdState.lcdXAddr > (128-5)){
	glcdSetXY(0,GrLcdState.lcdYAddr + 1);
}

for(i=0; i<5; i++)
{
	glcdWriteData(pgm_read_byte(&Font5x7[((c - 0x20) * 5) + i])); //Borde snyggas till med pekare
}
	glcdWriteData(0x00);

}

void glcdPutStr(unsigned char *data){
	while (*data){
		glcdWriteChar(*data);
		data++;
	}
}

