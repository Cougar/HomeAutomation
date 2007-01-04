/**
 * CanBlinds.
 * Takes incomming packages and adjusts PWM output.
 * Input: -128 to +127
 *
* @date    2001-01-01
 * @author  Erik Larsson
 */

#define DEBUG           1 /* Prints debug messages to uart */

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
//#define PWM_MAX_PULSE       500 /* milliseconds */
//#define PWM_MIN_PULSE       1500
#define FAILSAFE_MODE       0x0F
#define FAILSAFE_TRESHOLD   60  /* Degrees when nodeBlinds goes into failsafe mode */

#define BLINDS_CMD_GET      0x1200
#define BLINDS_CMD_SEND     0x1201

#define BLINDS_CMD_ABS      0x01 /* Absolute position */
#define BLINDS_CMD_REL      0x02 /* Relative position */
#define BLINDS_CMD_START    0x03 /* Start turning RC-servo */
#define BLINDS_CMD_STOP     0x04 /* Stop turning RC-servo */
#define BLINDS_CMD_RESET    0x05

#define BLINDS_TURN_LEFT    0x00
#define BLINDS_TURN_RIGHT   0x01

#define STATUS_SEND_PERIOD  10000 /* milliseconds */
#define FAILSAFE_SEND_PERIOD    10000 /* milliseconds */
#define BOUNCE_TIME         10 /* milliseconds */
#define BUTTON_ID           0x1202
#define BUTTON1             1
#define BUTTON2             2

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
#if TWO_BUTTON_MODE
Can_Message_t txMsg_Button;
#endif

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void blindsFailsafe();

/*----------------------------------------------------------------------------
 * Interrupt routines
 * -------------------------------------------------------------------------*/
#if TWO_BUTTON_MODE
ISR( PCINT2_vect )
{
    /* 
     * Auxiliary button pressed and released
     * Check time between press and release to eliminate bounces
     */
    static uint32_t buttonTime[2];

    txMsg_Button.RemoteFlag = 0;
    txMsg_Button.ExtendedFlag = 1;

    if( (PIND & (1<<PD6)) == 0){
        /* Button pressed */
        buttonTime[0] = Timebase_CurrentTime();
#if DEBUG
        printf("Button 1 pressed\n");
#endif
    }else if( (PIND & (1<<PD6)) == 1){
        if( Timebase_PassedTimeMillis( buttonTime[0] ) >= BOUNCE_TIME ){
            /* No bounce, send message */
            txMsg_Button.Id = BUTTON_ID;
            txMsg_Button.DataLength = 1;
            txMsg_Button.Data.bytes[0] = BUTTON1;
#if DEBUG
            printf("Button 1 pressed: message sent\n");
#endif
            return;
        }else{
            /* Just bounces, ignore it */
#if DEBUG
            printf("Button 1 just bounced\n");
#endif
            return;
        }
    }
    if( (PIND & (1<<PD7)) == 0){
        /* Button pressed */
        buttonTime[1] = Timebase_CurrentTime();
#if DEBUG
        printf("Button 2 pressed\n");
#endif
    }else if( (PIND & (1<<PD7)) == 1){
        if( Timebase_PassedTimeMillis( buttonTime[1] ) >= BOUNCE_TIME ){
            /* No bounce, send message */
            txMsg_Button.Id = BUTTON_ID;
            txMsg_Button.DataLength = 1;
            txMsg_Button.Data.bytes[0] = BUTTON2;
#if DEBUG
            printf("Button 2 pressed: message sent\n");
#endif
            return;
        }else{
            /* Just bounces, ignore it */
#if DEBUG
            printf("Button 2 just bounced\n");
#endif
            return;
        }
    }
}
#endif

//ISR( ADC_vect )
//{
    /*
     * ADC conversion completed
     */
    // TODO använd denna rutin för att läsa värde när omvandlingen är klar

//}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    uint8_t blindsStatus; /* Position */
    uint32_t boardTemperature = 0;

    adcTemperatureInit();
    Timebase_Init();
#if DEBUG
    Serial_Init();
#endif

#if TWO_BUTTON_MODE
    /*
     * Initialize buttons
     * It is possible to use ie. sensors instead
     * but then change this
     */
    /* Set as input and enable pull-up */
    DDRD &= ~( (1<<DDD6)|(1<<DDD7) );
    PORTD |= (1<<PD6)|(1<<PD7);

    /* Enable IO-pin interrupt */
    PCICR |= (1<<PCIE1);
    /* Unmask PD6 and PD7 */
    PCMSK2 |= (1<<PCINT22) | (1<<PCINT23);
#endif
    sei();
#if DEBUG
    printf("\n------------------------------------------------------------\n");
    printf(  "   CAN: Blinds\n");
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
    txMsg.Id = BLINDS_CMD_SEND;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;
    txMsg.DataLength = 3;

    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            /* This node that control a servo is adressed */
            if( rxMsg.Id == BLINDS_CMD_GET ){

                if( rxMsg.Data.bytes[0] == BLINDS_CMD_ABS ){
                    /* Set absolute position to servo */

                }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_REL){
                    /* Set relative position to servo */

                }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_START ){
                    /* Start turning the servo */

                }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_STOP ){
                    /* Stop turning */

                }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_STATUS){
                    /* Send blinds status to CAN */

                    boardTemperature = getTC1047temperature();

                    if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                        blindsFailsafe();
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
            /* Send blinds status to CAN */

            boardTemperature = getTC1047temperature();

            if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                blindsFailsafe();
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

void blindsFailsafe() // TODO fixa failsafe
{
    uint8_t relayStatus;
    uint32_t boardTemperature = 0, timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = BLINDS_CMD_SEND;
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
            if( rxMsg.Id == BLINDS_CMD_GET ){
                if( rxMsg.Data.bytes[0] == BLINDS_CMD_RESET ){
                    /* Return from failsafe mode */
                    return;
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

