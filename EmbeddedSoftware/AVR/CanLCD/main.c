/**
 * CAN LCD.
 * For HD44780- and KS0073-based LCD displays
 * This is the early version that only prints temperature sensor data.
 *
 * @date    2006-12-11
 * @author  Erik Larsson
 *
 * TODO Fix more types of output: relay status, mail recieved etc.
 * Make it configurable to use UART, because it's not necessary on the final PCB
 *
 */

/* For eq's testing purpose */
#define EQLAZER 0

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

/* defines */
//#include <global_funcdefs> // TODO in future version use this (first fix the defs)

#ifdef EQLAZER
#include <eqlazer_funcdefs.h> // for eq's personal defs
#endif

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
	printf(  "   CAN LCD\n");
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
	txMsg.ExtendedFlag = 1;

    lcd_clrscr();

    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received and print to uart */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
            printf("data={ ");

            for (uint8_t i=0; i<rxMsg.DataLength; i++) {
                printf("%x ", rxMsg.Data.bytes[i]);
            }
            printf("}\n");

#ifdef EQLAZER /* More personalized code */
            if( (rxMsg.Id & 0x1E000000)>>25 == FUNCT_SENSORS){

// TODO fixa rätt umatning av sensordata: negativa tal och decimaldelen.
// Skriva om hela skiten
//
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
            }
#else /* Generally code */

        /* If temperature sensor data received print to LCD */
        if( rxMsg.Id  == 0x300 ){

                for( uint8_t i=0; i<(rxMsg.DataLength/2); i++ ){
                        snprintf( buffer, BUFFER_SIZE, "Sensor %i: %d,%d%cC", i, rxMsg.Data.bytes[i*2], rxMsg.Data.bytes[i*2+1], 0xdf ); // TODO fixa korrekt utmatning av negativa tal och decimaldel
                        lcd_gotoxy(0,1+i);
                        lcd_puts( buffer );
                }
            }
#endif
		}
	}
	
	return 0;
}
