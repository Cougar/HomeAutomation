/**
 * CAN Relay.
 *
 *
 * @date    2006-12-17
 * @author  Erik Larsson
 *
 *   Observera! Applikationen är endast testad på labplatta med m88, pot och lysdiod
 *
 *   ADC=Vin*1024/Vref
 *   Vout=( 10 mV/C )( Temperature C ) + 500 mV
 *
 *  Transmitts: DATA: <temperature low byte> <temperature high byte> <relay status> <failsafe mode>
 *
 *
 */

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

#include <tc1047.h>

/* defines */
#define RELAY_OFF           0x01
#define RELAY_ON            0x02
#define FAILSAFE_MODE       0x0F
#define FAILSAFE_TRESHOLD   60  /* Degrees when nodeRelay goes into failsafe mode */

#define RELAY_CMD_GET       0x1200
#define RELAY_CMD_SEND      0x1201
#define RELAY_CMD_ON        0x01
#define RELAY_CMD_OFF       0x02
#define RELAY_CMD_TOGGLE    0x03
#define RELAY_CMD_STATUS    0x04
#define RELAY_CMD_RESET     0x05
#define STATUS_SEND_PERIOD  10000 /* milliseconds */
#define FAILSAFE_SEND_PERIOD  10000 /* milliseconds */

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
uint8_t relayOff();
uint8_t relayOn();
void relayFailsafe();

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    uint8_t relayStatus;
    uint32_t boardTemperature = 0;

    adcTemperatureInit();

    Timebase_Init();
#if DEBUG
    Serial_Init();
#endif
    sei();

    /* Turn relay off */
    relayStatus = relayOff();

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
#else
    Can_Init();
#endif
    uint32_t timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = RELAY_CMD_SEND;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;
    txMsg.DataLength = 3;

    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            /* This relay is adressed*/
            if( rxMsg.Id == RELAY_CMD_GET ){

                if( rxMsg.Data.bytes[0] == RELAY_CMD_ON ){
                    /* Turn relay on */
                    relayStatus = relayOn();

                }else if(rxMsg.Data.bytes[0] == RELAY_CMD_OFF){
                    /* Turn relay off */
                    relayStatus = relayOff();

                }else if(rxMsg.Data.bytes[0] == RELAY_CMD_TOGGLE ){
                    /* Toggle relay */

                    if(relayStatus == RELAY_OFF){
                        relayStatus = relayOn();
                    }else{
                        relayStatus = relayOff();
                    }

                }else if(rxMsg.Data.bytes[0] == RELAY_CMD_STATUS){
                    /* Send relay status to CAN */

                    boardTemperature = getTC1047temperature();

                    if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                        relayFailsafe();
                    }else{
                        txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
                        txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
                        txMsg.Data.bytes[2] = relayStatus;

                        Can_Send( &txMsg );
                    }
                }
            }
        }

        if( Timebase_PassedTimeMillis(timeStamp) >= STATUS_SEND_PERIOD ){
            timeStamp = Timebase_CurrentTime();
            /* Send relay status to CAN */

            boardTemperature = getTC1047temperature();

            if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                relayFailsafe();
            }else{
                txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
                txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
                txMsg.Data.bytes[2] = relayStatus;

                Can_Send( &txMsg );
#if DEBUG
                printf("Periodic message sent...\n");
                printf("Temperature: %d\nRelay status: %u\n", boardTemperature, relayStatus);
#endif
            }
        }
    }
    return 0;
}

uint8_t relayOff()
{
    /* Set as output and sink */
    PORTC &= ~(1<<PC1);
    DDRC |= (1<<DDC1);

#if DEBUG
    printf("relay turned off\n");
#endif

    return RELAY_OFF;
}

uint8_t relayOn()
{
    /* Set as input and enable pull-up */
    DDRC &= ~(1<<DDC1);
    PORTC |= (1<<PC1);

#if DEBUG
    printf("relay turned on\n");
#endif

    return RELAY_ON;
}

void relayFailsafe()
{
    uint8_t relayStatus;
    uint32_t boardTemperature = 0, timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = RELAY_CMD_SEND;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;
    txMsg.DataLength = 4;

    relayStatus = relayOff();

    while(1){
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            /* This relay is adressed*/
            if( rxMsg.Id == RELAY_CMD_GET ){
                if( rxMsg.Data.bytes[0] == RELAY_CMD_RESET ){
                    /* Return from failsafe mode */
                    return 0;
                }
            }
        }

        if( Timebase_PassedTimeMillis(timeStamp) >= FAILSAFE_SEND_PERIOD ){
            timeStamp = Timebase_CurrentTime();
            /* Send relay status to CAN */
#if DEBUG
            printf("Failsafe mode!\n");
#endif
            boardTemperature = getTC1047temperature();

            txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
            txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
            txMsg.Data.bytes[2] = relayStatus;
            txMsg.Data.bytes[3] = FAILSAFE_MODE;

            Can_Send( &txMsg );
        }
    }
}

