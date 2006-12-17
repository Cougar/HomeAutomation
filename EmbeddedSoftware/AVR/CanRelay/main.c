/**
 * CAN Test. This program sends a CAN message once every second. The ID of the
 * message is increased each time for testing purposes.
 * Added LCD output
 * 
 * @date	2006-11-21, LCD addition 2006-12-11
 * @author	Jimmy Myhrman, Erik Larsson
 *   
 *   Observera! Applikationen är helt otestad
 *   ADC=Vin*1024/Vref
 *   Vout=( 10 mV/C )( Temperature C ) + 500 mV
 *
 *  Transmitts: DATA: <temperature high byte> <temperature low byte> <relay status>
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

#define OFF     0x00
#define ON      0x01
#define Vref    5

/*------------------------------------------------------------------------------
 * Read relay state (OPEN/CLOSED) and temperature
 * ---------------------------------------------------------------------------*/
void sendRelayStatus( Can_Message_t* msg, uint8_t state )
{
    int16_t temp;

    /* Start measureing */
    ADCSRA |= (1<<ADSC);

    /* Wait for conversion to complete */
    while( ADCSRA & (1<<ADSC) ){}

    msg->Id = 0x1201; // temporary ID

    msg->Data.bytes[0] = state;

    /* Convert voltage to temperature */
    temp = (ADCL & (ADCH<<8));
    temp = temp*Vref/1024;
    temp = (temp - 0.5)/(0.01);

    msg->Data.bytes[1]= temp;
    msg->Data.bytes[2]= (temp>>8);
    msg->DataLength = 3;

    Can_Send( &msg );
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    uint8_t relayStatus;

    Timebase_Init();
    Serial_Init();
    sei();

    /* Turn relay off */
    PORTC &= ~(1<<PC1);
    DDRC |= (1<<DDC1);

    relayStatus = OFF;

    /* Initiate ADC for reading temperature sensor */

    /* Wake up ADC */
    PRR &= ~(1<<PRADC);
    /* Enable AREF and ADC7 */
    ADMUX = 0x07;
    ADCSRA |= (1<<ADEN);

    printf("\n------------------------------------------------------------\n");
    printf(  "   CAN: Relay\n");
    printf(  "------------------------------------------------------------\n");


    printf("CanInit...");
    if (Can_Init() != CAN_OK) {
        printf("FAILED!\n");
    }else{
        printf("OK!\n");
    }

    uint32_t timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = 0;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;


    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            /* This relay is adressed*/
            if( rxMsg.Id == 0x1200 ){

                if( rxMsg.Data.bytes[0] == 0x01 ){
                    /* Turn relay on */

                    /* Set as input and enable pull-up */
                    DDRC &= ~(1<<DDC1);
                    PORTC |= (1<<PC1);

                    relayStatus = ON;

                }else if(rxMsg.Data.bytes[0] == 0x02){
                    /* Turn relay off */

                    PORTC &= ~(1<<PC1);
                    DDRC |= (1<<DDC1);

                    relayStatus = OFF;

                }else if(rxMsg.Data.bytes[0] == 0x03){
                    /* Toggle relay */

                    if(relayStatus == ON){
                        PORTC &= ~(1<<PC1);
                        DDRC |= (1<<DDC1);

                        relayStatus = OFF;

                    }else{
                        DDRC &= ~(1<<DDC1);
                        PORTC |= (1<<PC1);

                        relayStatus = ON;
                    }

                }else if(rxMsg.Data.bytes[0] == 0x04){
                    /* Get relay status (ON/OFF?, temperature) */
                    sendRelayStatus( &txMsg, relayStatus );
                }
            }
        }

        // TODO
        // ska status på relä och temperatur skickas periodiskt?
        // tätare intervall om temperaturen kommer över en viss gräns?

        if( Timebase_PassedTimeMillis(timeStamp) >=5000){
            timeStamp = Timebase_CurrentTime();
            /* Get relay status (ON/OFF?, temperature) */

             sendRelayStatus( &txMsg, relayStatus );
        }
	}
	return 0;
}

