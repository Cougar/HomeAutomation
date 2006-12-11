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

#define BUFFER_SIZE 20

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    char buffer[ BUFFER_SIZE ];
    int num=1234;

	Timebase_Init();
	Serial_Init();
	sei();
	lcd_init( LCD_DISP_ON );

	printf("\n------------------------------------------------------------\n");
	printf(  "   CAN Test: Periodic Transmission\n");
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
	
//    snprintf(buffer, BUFFER_SIZE, "yo %d 98765432109\n", num);
//    lcd_puts(buffer);
//    lcd_puts("test\n");
//    lcd_puts(buffer);

	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; //DataLength and the databytes are just what happens to be in the memory. They are never set.
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			txMsg.Id++;
			Can_Send(&txMsg);
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");

    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");

            if( rxMsg.Id == 0x3 ){
//                lcd_gotoxy(0,1);
                lcd_clrscr();
 //               lcd_putc(0xdf);
                lcd_puts("Temperature data:\n");

                for( uint8_t i=0; i<(rxMsg.DataLength/2); i++ ){
                        snprintf( buffer, BUFFER_SIZE, "Sensor %i: %d,%d%cC", i, rxMsg.Data.bytes[i*2], rxMsg.Data.bytes[i*2+1], 0xdf );
                        lcd_gotoxy(0,1+i);
                        lcd_puts( buffer );
                }
            }
		}
	}
	
	return 0;
}
