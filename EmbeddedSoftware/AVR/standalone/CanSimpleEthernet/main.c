/**
 * 
 * 
 * 
 * @date	2006-11-21
 * @author	Jimmy Myhrman, Anders Runeson
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
#include <string.h>

#include <canid.h>
#include <config.h>
/* lib files */
#if USE_STDCAN == 1
#include <stdcan.h>
#else
#include <mcp2515.h>
#endif
//#include <serial.h>
#include <timebase.h>
#include <mcu.h>
#include <ip_arp_udp_tcp.h>
#include <enc28j60.h>
#include <timeout.h>
#include <avr_compat.h>
#include <net.h>

static uint8_t mymac[6] = {ENC28J60_MAC1,ENC28J60_MAC2,ENC28J60_MAC3,ENC28J60_MAC4,ENC28J60_MAC5,ENC28J60_MAC6};
//static uint8_t myip[4] = {192,168,0,50};
static uint8_t myip[4] = {ENC28J60_IP1,ENC28J60_IP2,ENC28J60_IP3,ENC28J60_IP4};
static uint16_t rmCan2SerPrt =1100; // remote port for Can2Serial-data
static uint16_t locCan2SerPrt =1100; // listen port for Can2Serial-data
static uint8_t gotcan2serserver = 0;

static uint8_t prgremotemac[6];
static uint8_t prgremoteip[4];

#define BUFFER_SIZE 250
static uint8_t buf[BUFFER_SIZE+1];

static uint8_t buffpoint;
 
#define C2S_START_BYTE 253
#define C2S_END_BYTE 250
#define C2S_PING_BYTE 251

#if USE_STDCAN == 0
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag
#endif


/*
#define SEND_BUFFER_SIZE 20
static char sendbuf[SEND_BUFFER_SIZE+1];
static uint16_t locDumperPrt =1200; // listen port 
static uint8_t gotdumpserver = 0;
static uint16_t rmDumperPrt =1200; // remote port for dumping data
static uint8_t remotemac[6];
static uint8_t remoteip[4];
*/
/*----------------------------------------------------------------------------
 * Putchar for udp
 * Implements a small buffer, sends packet over udp if buffer is full or 
 * \n-char is sent.
 * Needs a timeout to force send. 
 *--------------------------------------------------------------------------*/
/*int udp_putchar(char c, FILE* stream) {
	if (gotdumpserver != 0) {
        sendbuf[buffpoint] = c;
        buffpoint++;
        
        if (c == '\n' || buffpoint > SEND_BUFFER_SIZE) {
        	send_udp(buf, sendbuf, buffpoint, rmDumperPrt, remotemac, remoteip);
        	buffpoint = 0;
        }
	}
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(udp_putchar, NULL, _FDEV_SETUP_WRITE); 
*/

#if USE_STDCAN == 0
void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.

	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}
#endif

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

#if MCP_CS_BIT != PB2
	/* If slave select is not set as output it might change SPI hw to slave
	 * See ch 18.3.2 (18.3 SS Pin Functionality) in ATmega48/88/168-datasheet */
	DDRB |= (1<<PB2);
#endif


	//ethernet vars
    uint16_t plen = 0;
    uint8_t payloadlen=0;
	buffpoint = 0;
	
	//can vars
	
 	uint32_t timeStamp = 0;

	//uint8_t sw=0;
	// LED
    /* enable PB1, LED as output */
    //DDRB|= (1<<DDB0);
    
    /* set output to Vcc, LED off */
    //PORTB|= (1<<PB0);
	
	Mcu_Init();
	Timebase_Init();

    //alla printf ska skriva till udp
    //stdout = &mystdout;    //set the output stream 

#if USE_STDCAN == 0
	Can_Init();
	Can_Message_t txMsg;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	rxMsgFull = 0;
#else
	StdCan_Init(0);
	StdCan_Msg_t txMsg;
	StdCan_Msg_t rxMsg;
#endif

	sei();

    /* initialize enc28j60 */
	if (enc28j60Init(mymac) != 1) {
		/* Great place to set an IO for debug */
	}
    delay_ms(20);
    
    /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
    // LEDB=yellow LEDA=green
    //
    // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
    enc28j60PhyWrite(PHLCON,0x476);
    delay_ms(20);
    
    //init the ethernet/ip layer:
    init_ip_arp_udp_tcp(mymac,myip,80);

	/* main loop */
	while (1) {
		/* service the CAN routines */
		
#if SENDTIMESTAMP == 1
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();

			/* send timestamp, to keep network alive */
			txMsg.Id = (CAN_NMT << CAN_SHIFT_CLASS) | (CAN_NMT_TIME << CAN_SHIFT_NMT_TYPE);
#if USE_STDCAN == 0
			txMsg.DataLength = 0;
			Can_Send(&txMsg);
#else
			txMsg.Length = 0;
			StdCan_Put(&txMsg);
#endif
		}
#endif
		
		/* check if any messages have been received */
#if USE_STDCAN == 0
		if (rxMsgFull) {
#else
		if (StdCan_Get(&rxMsg) == StdCan_Ret_OK) {
#endif
			uint8_t i;
			/* Do we have any ip to send the can data to yet? */
	   		if (gotcan2serserver != 0) {
				/* Send can frame raw in an udp packet */
				char sendbuf2[17];
				
				sendbuf2[0] = C2S_START_BYTE;
				sendbuf2[1] = (uint8_t)rxMsg.Id;
				sendbuf2[2] = (uint8_t)(rxMsg.Id>>8);
				sendbuf2[3] = (uint8_t)(rxMsg.Id>>16);
				sendbuf2[4] = (uint8_t)(rxMsg.Id>>24);
#if USE_STDCAN == 0
				sendbuf2[5] = rxMsg.ExtendedFlag;
				sendbuf2[6] = rxMsg.RemoteFlag;
				sendbuf2[7] = rxMsg.DataLength;
	    		for (i=0; i<8; i++) {
	    			sendbuf2[8+i] = rxMsg.Data.bytes[i];
	    		}
#else
				sendbuf2[5] = 1;
				sendbuf2[6] = 0;
				sendbuf2[7] = rxMsg.Length;
	    		for (i=0; i<8; i++) {
	    			sendbuf2[8+i] = rxMsg.Data[i];
	    		}
#endif
				sendbuf2[16] = C2S_END_BYTE;
				
#ifndef ENC28J60_USART_SPI_MODE 
				//MCP_INT_DISABLE();
				cli();
#endif
				send_udp(buf, sendbuf2, 18, rmCan2SerPrt, prgremotemac, prgremoteip);
#ifndef ENC28J60_USART_SPI_MODE 
				sei();
				//MCP_INT_ENABLE();
#endif
	   		}
	   		
#if USE_STDCAN == 0
			rxMsgFull = 0;
#endif
		}
		
		//test-debug-kod
		if ((enc28j60Read(EIR) & EIR_TXERIF)) {
			//printf("EIR_TXERIF set!\n");
			if ((enc28j60Read(ECON1) & ECON1_TXRTS)) {
				//printf("ECON1_TXRTS set!\n");
			}
		}
		if ((enc28j60Read(EIR) & EIR_TXIF)) {
			//printf("EIR_TXIF set! ");
		}
		if ((enc28j60Read(ESTAT) & ESTAT_TXABRT)) {
			//printf("ESTAT_TXABRT set!\n");
			//printf("MACLCON1: %i\n", enc28j60Read(MACLCON1));
			enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ESTAT, ESTAT_TXABRT);
		}
		
        // get the next new packet:
        plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

        /*plen will ne unequal to zero if there is a valid 
         * packet (without crc error) */
        if (plen!=0){
	        // arp is broadcast if unknown but a host may also
	        // verify the mac address by sending it to 
	        // a unicast address.
	        if (eth_type_is_arp_and_my_ip(buf,plen)) {
	                make_arp_answer_from_request(buf);
	        } else {
			    /* check if ip packets (icmp or udp) are for us by checking eth type and receiver ip adress: */
			    if (eth_type_is_ip_and_my_ip(buf,plen)!=0) {
			        
	                /* a ping packet, let's send pong */
			        if (buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V) {
			                make_echo_reply_from_request(buf,plen);
			        }
			        
			        /* If packet is an udp packet */
	                if (buf[IP_PROTO_P]==IP_PROTO_UDP_V) {
                        payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
						uint8_t i = 0;
                        
                        /* if port is can2serial-port (default 1100) */
                        if ((buf[UDP_DST_PORT_H_P]==((locCan2SerPrt>>8) & 0xff)) && (buf[UDP_DST_PORT_L_P] == (locCan2SerPrt & 0xff))) {
			                while(i<6) {
				                prgremotemac[i]=buf[ETH_SRC_MAC +i];
				                i++;
					        }
					        i=0;
			                while(i<4) {
				                prgremoteip[i]=buf[IP_SRC_P +i];
				                i++;
					        }
					        gotcan2serserver = 1;
					        
					        /* If data is a raw can packet */
							if ((buf[UDP_DATA_P]==C2S_START_BYTE) && (buf[UDP_DATA_P+16]==C2S_END_BYTE) && (payloadlen==17)) 
							{
#if USE_STDCAN == 0
								Can_Message_t cm;
								cm.DataLength = buf[UDP_DATA_P+7];
								
								if (cm.DataLength > 8)
								{
									cm.DataLength = 8;
								}
								
								cm.RemoteFlag = buf[UDP_DATA_P+6];
								cm.ExtendedFlag = buf[UDP_DATA_P+5];
								cm.Id = (uint32_t)buf[UDP_DATA_P+1] + ((uint32_t)buf[UDP_DATA_P+2] << 8) + 
										((uint32_t)buf[UDP_DATA_P+3] << 16) + ((uint32_t)buf[UDP_DATA_P+4] << 24);
								uint8_t i;
								for (i = 0; i < cm.DataLength; i++) {
									cm.Data.bytes[i] = buf[UDP_DATA_P+8+i];
								}
								Can_Send(&cm);
#else
								static StdCan_Msg_t cm;
								cm.Length = buf[UDP_DATA_P+7];
								
								if (cm.Length > 8)
								{
									cm.Length = 8;
								}
								
								cm.Id = (uint32_t)buf[UDP_DATA_P+1] + ((uint32_t)buf[UDP_DATA_P+2] << 8) + 
										((uint32_t)buf[UDP_DATA_P+3] << 16) + ((uint32_t)buf[UDP_DATA_P+4] << 24);
								uint8_t i;
								for (i = 0; i < cm.Length; i++) {
									cm.Data[i] = buf[UDP_DATA_P+8+i];
								}
								StdCan_Put(&cm);
#endif
							}
							/* of ir data is an udp ping */
							else if ((buf[UDP_DATA_P]==C2S_PING_BYTE) && (payloadlen<=2) && (payloadlen>0))
							{
								char sendbuf3[1];
								sendbuf3[0] = C2S_PING_BYTE;
				
#ifndef ENC28J60_USART_SPI_MODE 
								cli();
#endif
								send_udp(buf, sendbuf3, 1, rmCan2SerPrt, prgremotemac, prgremoteip);
#ifndef ENC28J60_USART_SPI_MODE 
								sei();
#endif
							}
                        }
	                }			            
			    }
	        }
        }
	}
	return 0;
}

