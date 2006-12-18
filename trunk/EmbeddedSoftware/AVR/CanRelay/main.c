/**
 * CAN Relay.
 *
 *
 * @date    2006-12-17
 * @author  Erik Larsson
 *
 *   Observera! Applikationen är testad på labplatta med m88, pot och lysdiod
 *
 *   ADC=Vin*1024/Vref
 *   Vout=( 10 mV/C )( Temperature C ) + 500 mV
 *
 *  Transmitts: DATA: <temperature high byte> <temperature low byte> <relay status>
 *
 *  TODO fixa så status/temp ligger i en funktion
 *  temperaturomvandlingen fungerar fint, men sparas i Data på ett fuckat sätt
 *  fixa så adcn inte pollas hela tiden, det tar för mycket tid
 *
 */

/*
 * For testing with ATmega88 PDIP enable this fetaure
 * Since PDIP doesn't have ADC7 it will use the ADC0 instead
 */
#define PDIP    1
#define DEBUG   0 /* Prints debug messages to uart */

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
/* defines */
#define OFF     0x01
#define ON      0x02
#define Vref    5

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    uint8_t relayStatus;
    uint32_t boardTemperature;

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
#if PDIP
    /* Enable AREF and ADC0 (For PDIP package) */
    ADMUX = 0x00; // TODO 0x00 för aref, 0xc0 för internal 1,1
#else
    /* Enable AREF and ADC7 (For TQFP package) */
    ADMUX = 0x07;
#endif
    ADCSRA |= (1<<ADEN);

#if DEBUG
    printf("\n------------------------------------------------------------\n");
    printf(  "   CAN: Relay\n");
    printf(  "------------------------------------------------------------\n");


    printf("CanInit...");
    if (Can_Init() != CAN_OK) {
        printf("FAILED!\n");
    }else{
        printf("OK!\n");
    }
#endif
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
#if DEBUG
                    printf("turn on\n");
#endif

                }else if(rxMsg.Data.bytes[0] == 0x02){
                    /* Turn relay off */

                    PORTC &= ~(1<<PC1);
                    DDRC |= (1<<DDC1);

                    relayStatus = OFF;
#if DEBUG
                    printf("turn off\n");
#endif

                }else if(rxMsg.Data.bytes[0] == 0x03){
                    /* Toggle relay */

                    if(relayStatus == ON){
                        PORTC &= ~(1<<PC1);
                        DDRC |= (1<<DDC1);
#if DEBUG
                        printf("toggle: turn off\n");
#endif

                        relayStatus = OFF;

                    }else{
                        DDRC &= ~(1<<DDC1);
                        PORTC |= (1<<PC1);
#if DEBUG
                        printf("toggle: turn on\n");
#endif

                        relayStatus = ON;
                    }

                }else if(rxMsg.Data.bytes[0] == 0x04){
                    /* Get relay status (ON/OFF?, temperature) */
                    ADCSRA |= (1<<ADSC);

                    /* Wait for conversion to complete */
                    while( ADCSRA & (1<<ADSC) ){}

                    txMsg.Id = 0x1201; // temporary ID

                    txMsg.Data.bytes[3] = relayStatus;

                    /* Convert voltage to temperature */

                    boardTemperature = ADCW;

                    boardTemperature = boardTemperature*Vref;
                    boardTemperature = (boardTemperature*100/1024)-50;

                    /* Save and send */
                    txMsg.Data.bytes[0]= (uint32_t)boardTemperature&0x00FF;
                    txMsg.Data.bytes[1]= ((uint32_t)boardTemperature&0xFF00)>>8;

                    txMsg.DataLength = 3;

                    Can_Send( &txMsg );
                }
            }
        }

        // TODO
        // ska status på relä och temperatur skickas periodiskt?
        // tätare intervall om temperaturen kommer över en viss gräns?

        if( Timebase_PassedTimeMillis(timeStamp) >=10000){
            timeStamp = Timebase_CurrentTime();
            /* Get relay status (ON/OFF?, temperature) and send */

            ADCSRA |= (1<<ADSC);

            /* Wait for conversion to complete */
            while( ADCSRA & (1<<ADSC) ){}

            txMsg.Id = 0x1201; // temporary ID

            txMsg.Data.bytes[3] = relayStatus;

            /* Convert voltage to temperature */

            boardTemperature = ADCW;

            boardTemperature = boardTemperature*Vref;
            boardTemperature = (boardTemperature*100/1024)-50;

            /* Save and send */
            txMsg.Data.bytes[0]= (uint32_t)boardTemperature&0x00FF;
            txMsg.Data.bytes[1]= ((uint32_t)boardTemperature&0xFF00)>>8;

            txMsg.DataLength = 3;
            Can_Send( &txMsg );
#if DEBUG
            printf("Message sent...\n");
            printf("Temperature: %d\nRelay status: %x, %u\n", boardTemperature, relayStatus, relayStatus);
#endif

        }
    }
    return 0;
}

