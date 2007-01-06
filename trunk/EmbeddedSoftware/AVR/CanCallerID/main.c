/**
 * Description
 * 
 * @date	2007-01-06
 * @author	Anders Runeson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
/* lib files */
#include <can.h>
#include <serial.h>
#include <timebase.h>

/*
 * De få signaler som man behöver koppla in från MT8870 är 
 * StD, Q1, Q2, Q3 och Q4
 */

#define MT_StD_PIN	PIND
#define MT_StD_DDR	DDRD
#define MT_StD_BIT	PD4

#define MT_Q1_PIN	PIND
#define MT_Q1_DDR	DDRD
#define MT_Q1_BIT	PD5

#define MT_Q2_PIN	PIND
#define MT_Q2_DDR	DDRD
#define MT_Q2_BIT	PD6

#define MT_Q3_PIN	PINB
#define MT_Q3_DDR	DDRB
#define MT_Q3_BIT	PB0

#define MT_Q4_PIN	PINB
#define MT_Q4_DDR	DDRB
#define MT_Q4_BIT	PB1


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	
	Mcu_Init();
	Timebase_Init();
	Serial_Init();
	
	sei();

	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	uint32_t lastToneTimeStamp = 0;
	uint8_t timerOn = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = 4;
	txMsg.DataLength = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; //DataLength and the databytes are just what happens to be in the memory. They are never set.
	
	//set ports to input
	MT_StD_DDR &= ~(1<<MT_StD_BIT);
	MT_Q1_DDR &= ~(1<<MT_Q1_BIT);
	MT_Q2_DDR &= ~(1<<MT_Q2_BIT);
	MT_Q3_DDR &= ~(1<<MT_Q3_BIT);
	MT_Q4_DDR &= ~(1<<MT_Q4_BIT);
	
	uint8_t receviercounter = 0;
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		if ((MT_StD_PIN & (1<<MT_StD_BIT)) != 0) {
			//här ska det egentligen vara en en kort delay innan man kollar 
			//om std är hög igen

			if ((MT_StD_PIN & (1<<MT_StD_BIT)) != 0) {
				//testa om receviercounter har blivit för stor, error error
				if (receviercounter >= 16) {
					receviercounter = 0;	//kasta paket
				} else {
					//nu finns data tillgänglig på Qx
					uint8_t datain = 0;
					datain |= ((MT_Q1_PIN >> MT_Q1_BIT) & 0x1);
					datain |= (((MT_Q2_PIN >> MT_Q2_BIT) & 0x1)<<1);
					datain |= (((MT_Q3_PIN >> MT_Q3_BIT) & 0x1)<<2);
					datain |= (((MT_Q4_PIN >> MT_Q4_BIT) & 0x1)<<3);
					if (datain == 0) { datain = 0x1d; }		//DTMF0  = D	//this is temporary
					if (datain == 11) { datain = 0x1e; } 	//DTMF12 = *	//this is temporary
					if (datain == 12) { datain = 0x1f; }	//DTMF11 = #	//this is temporary
					if (datain == 15) { datain = 0xc; }		//DTMF11 = C
					if (datain == 10) { datain = 0; }		//DTMF10 = number 0
					if (datain == 13) { datain = 0xa; }		//DTMF13 = A
					if (datain == 14) { datain = 0xb; }		//DTMF14 = B
					if (datain == 0x1d) { datain = 0xd; }	//DTMF0  = D	//now permanent
					if (datain == 0x1e) { datain = 0xe; } 	//DTMF12 = *	//now permanent
					if (datain == 0x1f) { datain = 0xf; }	//DTMF11 = #	//now permanent
					
					if ((receviercounter & 0x1) == 0) {
						txMsg.Data.bytes[(receviercounter>>1)] = (datain<<4);
					} else {
						txMsg.Data.bytes[(receviercounter>>1)] |= datain; 
					}
	
					receviercounter++;
					 
					timerOn = 1;
				}
				while ((MT_StD_PIN & (1<<MT_StD_BIT)) != 0) {}
				lastToneTimeStamp = Timebase_CurrentTime();
			}
		}
		
		if (timerOn ==1 && Timebase_PassedTimeMillis(lastToneTimeStamp) >= 1800) {
			timerOn = 0;
			if ((receviercounter & 0x1) == 0) {
				txMsg.DataLength = (receviercounter>>1);
			} else {
				txMsg.DataLength = (receviercounter>>1)+1;
			}
			
			receviercounter = 0;
			Can_Send(&txMsg);
		}		
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 2000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			//txMsg.Id++;
			//Can_Send(&txMsg);
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			/*printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");
    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");*/
		}
	}
	
	return 0;
}
