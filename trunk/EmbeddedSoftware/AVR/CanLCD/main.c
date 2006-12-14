/**
 * CAN Test. This program sends a CAN message once every second. The ID of the
 * message is increased each time for testing purposes.
 * Added LCD output
 * 
 * @date	2006-11-21, LCD addition 2006-12-11
 * @author	Jimmy Myhrman, Erik Larsson
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
#include <lcd_HD44780.h>

#include <eqlazer_funcdefs.h>

#define BUFFER_SIZE 20

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    char buffer[ BUFFER_SIZE ];
    uint8_t subzero;

	Timebase_Init();
	Serial_Init();
	sei();
	lcd_init( LCD_DISP_ON );

	printf("\n------------------------------------------------------------\n");
	printf(  "   CAN Test: LCD\n");
	printf(  "------------------------------------------------------------\n");

    lcd_clrscr();
    lcd_puts("CAN LCD test\n");

	printf("CanInit...");
    lcd_puts("CanInit... ");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
        lcd_puts("FAILED!\n");
	}
	else {
		printf("OK!\n");
        lcd_puts("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; //DataLength and the databytes are just what happens to be in the memory. They are never set.

    lcd_clrscr();

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
//		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
//			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
//			txMsg.Id++;
//			Can_Send(&txMsg);
//		}

		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");

    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");


            if( (rxMsg.Id & 0x1E000000)>>25 == FUNCT_SENSORS){ // if sensor data received

                /* which temperature sensor? */
                if( (rxMsg.Id & 0x01FF8000)>>15 == FUNCC_SENSORS_TEMPERATURE_INSIDE ){
                    /* Below 0 degrees celsius? */
                    if(rxMsg.Data.bytes[0]&0x80){
                        rxMsg.Data.bytes[0]= -rxMsg.Data.bytes[0];
                        subzero = 1;
                    }else{
                        subzero = 0;
                    }
                        snprintf( buffer, BUFFER_SIZE, (subzero)?"-%d,%d%cC":"%d,%d%cC", rxMsg.Data.bytes[0], rxMsg.Data.bytes[1], 0xdf );
                        lcd_gotoxy( LCD_LINE0_0 );
                        lcd_puts( LCD_SENSOR_TEMPERATURE_INSIDE );
                        lcd_gotoxy( LCD_LINE1_0 );
                        lcd_puts( buffer );
                }else if( (rxMsg.Id & 0x01FF8000)>>15 == FUNCC_SENSORS_TEMPERATURE_OUTSIDE ){
                    if(rxMsg.Data.bytes[0]&0x80){
                        rxMsg.Data.bytes[0]= -rxMsg.Data.bytes[0];
                        subzero = 1;
                    }else{
                        subzero = 0;
                    }
                        snprintf( buffer, BUFFER_SIZE, (subzero)?"-%d,%d%cC":"%d,%d%cC", rxMsg.Data.bytes[0], rxMsg.Data.bytes[1], 0xdf );
                        lcd_gotoxy( LCD_LINE2_0 );
                        lcd_puts( LCD_SENSOR_TEMPERATURE_OUTSIDE );
                        lcd_gotoxy( LCD_LINE3_0 );
                        lcd_puts( buffer );
                }else if( (rxMsg.Id & 0x01FF8000)>>15 == FUNCC_SENSORS_TEMPERATURE_FREEZER ){
                    if(rxMsg.Data.bytes[0]&0x80){
                        rxMsg.Data.bytes[0]= -rxMsg.Data.bytes[0];
                        subzero = 1;
                    }else{
                        subzero = 0;
                    }
                        snprintf( buffer, BUFFER_SIZE, (subzero)?"-%d,%d%cC":"%d,%d%cC", rxMsg.Data.bytes[0], rxMsg.Data.bytes[1], 0xdf );
                        lcd_gotoxy( LCD_LINE2_2 );
                        lcd_puts( LCD_SENSOR_TEMPERATURE_FREEZER );
                        lcd_gotoxy( LCD_LINE3_2 );
                        lcd_puts( buffer );
                }else{
                    lcd_clrscr();
                    lcd_puts("Sensors: No config.");
                }
/*
                for( uint8_t i=0; i<(rxMsg.DataLength/2); i++ ){
                        snprintf( buffer, BUFFER_SIZE, "Sensor %i: %d,%d%cC", i, rxMsg.Data.bytes[i*2], rxMsg.Data.bytes[i*2+1], 0xdf );
                        lcd_gotoxy(0,1+i);
                        lcd_puts( buffer );
                }*/
            }
		}
	}
	
	return 0;
}
