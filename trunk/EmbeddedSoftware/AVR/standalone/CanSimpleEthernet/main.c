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

/* lib files */
#include <mcp2515.h>
//#include <serial.h>
#include <timebase.h>
#include <mcu.h>
#include <ip_arp_udp_tcp.h>
#include <enc28j60.h>
#include <timeout.h>
#include <avr_compat.h>
#include <net.h>

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x27};
//static uint8_t myip[4] = {192,168,0,50};
static uint8_t myip[4] = {193,11,254,26};
static uint16_t rmDumperPrt =1200; // remote port for dumping data
static uint16_t rmCan2SerPrt =1100; // remote port for Can2Serial-data
//static uint16_t locDumperPrt =1200; // listen port 
static uint16_t locCan2SerPrt =1100; // listen port for Can2Serial-data
static uint8_t gotdumpserver = 0;
static uint8_t gotcan2serserver = 0;

// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX

static uint8_t remotemac[6];
static uint8_t remoteip[4];

static uint8_t prgremotemac[6];
static uint8_t prgremoteip[4];

#define BUFFER_SIZE 250
static uint8_t buf[BUFFER_SIZE+1];

#define SEND_BUFFER_SIZE 20
static char sendbuf[SEND_BUFFER_SIZE+1];
static uint8_t buffpoint;
 
#define C2S_START_BYTE 253
#define C2S_END_BYTE 250

/* set to 0 for not sending timestamps on bus */
#define SENDTIMESTAMP	0

volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

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

void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.

	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

ISR(MCP_INT_VECTOR) {
	// Get first available message from controller and pass it to
	// application handler. If both RX buffers contain messages
	// we will get another interrupt as soon as this one returns.
	if (Can_Receive(&rxMsg) == CAN_OK) {
		// Callbacks are run with global interrupts disabled but
		// with controller flag cleared so another msg can be
		// received while this one is processed.
		Can_Process(&rxMsg);
	}
}


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

	//ethernet vars
    uint16_t plen = 0;
    uint8_t payloadlen=0;
	buffpoint = 0;
	
	//can vars
	Can_Message_t txMsg;
	txMsg.Id = 3;
	txMsg.DataLength = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;

	rxMsgFull = 0;
	
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

	if (Can_Init() != CAN_OK) {
	}

	sei();

    /*initialize enc28j60*/
    enc28j60Init(mymac);
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
		//Can_Service();
		
#if SENDTIMESTAMP == 1
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();
			/* send timestamp, to keep network alive */
			txMsg.Id = 0;
			txMsg.DataLength = 0;
			Can_Send(&txMsg);
			
		}
#endif
		
		/* check if any messages have been received */
		if (rxMsgFull) { //(Can_Receive(&rxMsg) == CAN_OK) {
			uint8_t i;
			
			/*if (gotdumpserver != 0) {
				printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
	    		printf("data={ ");
	    		for (i=0; i<rxMsg.DataLength; i++) {
	    			printf("%x ", rxMsg.Data.bytes[i]);
	    		}
		   		printf("}\n");
			}*/
	   		
	   		if (gotcan2serserver != 0) {
				//skicka ocks� som udppaket till rmCan2SerPrt
				char sendbuf2[17];
				
				sendbuf2[0] = C2S_START_BYTE;
				sendbuf2[1] = (uint8_t)rxMsg.Id;
				sendbuf2[2] = (uint8_t)(rxMsg.Id>>8);
				sendbuf2[3] = (uint8_t)(rxMsg.Id>>16);
				sendbuf2[4] = (uint8_t)(rxMsg.Id>>24);
				sendbuf2[5] = rxMsg.ExtendedFlag;
				sendbuf2[6] = rxMsg.RemoteFlag;
				sendbuf2[7] = rxMsg.DataLength;
	    		for (i=0; i<8; i++) {
	    			sendbuf2[8+i] = rxMsg.Data.bytes[i];
	    		}
				sendbuf2[16] = C2S_END_BYTE;
				
				MCP_INT_DISABLE();
				send_udp(buf, sendbuf2, 18, rmCan2SerPrt, prgremotemac, prgremoteip);
				MCP_INT_ENABLE();
	   		}
	   		
	   		rxMsgFull = 0;
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
        if(plen!=0){
	        // arp is broadcast if unknown but a host may also
	        // verify the mac address by sending it to 
	        // a unicast address.
	        if(eth_type_is_arp_and_my_ip(buf,plen)){
	                make_arp_answer_from_request(buf);
	        } else {
			    // check if ip packets (icmp or udp) are for us:
			    if(eth_type_is_ip_and_my_ip(buf,plen)!=0){
			        
			        if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V){
			                // a ping packet, let's send pong
			                make_echo_reply_from_request(buf,plen);
			                //txMsg.Id = 6;
			                //Can_Send(&txMsg);
			                
			        }
	                if (buf[IP_PROTO_P]==IP_PROTO_UDP_V){
                        payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
						uint8_t i = 0;
                        
                        //om port 1100 (programmeringsporten)
                        if ((buf[UDP_DST_PORT_H_P]==((locCan2SerPrt>>8) & 0xff)) && (buf[UDP_DST_PORT_L_P] == (locCan2SerPrt & 0xff))) {
			                while(i<6){
				                prgremotemac[i]=buf[ETH_SRC_MAC +i];
				                i++;
					        }
					        i=0;
			                while(i<4){
				                prgremoteip[i]=buf[IP_SRC_P +i];
				                i++;
					        }
					        gotcan2serserver = 1;

							if ((buf[UDP_DATA_P]==C2S_START_BYTE) && (buf[UDP_DATA_P+16]==C2S_END_BYTE) && (payloadlen==17)) {
								Can_Message_t cm;
								//cm.Id = 0;
								cm.DataLength = buf[UDP_DATA_P+7];
								cm.RemoteFlag = buf[UDP_DATA_P+6];
								cm.ExtendedFlag = buf[UDP_DATA_P+5];
								cm.Id = (uint32_t)buf[UDP_DATA_P+1] + ((uint32_t)buf[UDP_DATA_P+2] << 8) + ((uint32_t)buf[UDP_DATA_P+3] << 16) + ((uint32_t)buf[UDP_DATA_P+4] << 24);
								uint8_t i;
								for (i = 0; i < cm.DataLength; i++) {
									cm.Data.bytes[i] = buf[UDP_DATA_P+8+i];
								}
								Can_Send(&cm);
								
								/*if (gotdumpserver != 0) {
									printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", cm.Id, (uint16_t)(cm.DataLength), (uint16_t)(cm.ExtendedFlag), (uint16_t)(cm.RemoteFlag));
									printf("data={ ");
									for (i=0; i<cm.DataLength; i++) {
										printf("%x ", cm.Data.bytes[i]);
									}
	   								printf("}\n");
								}*/
							}
						//om port 1200 (dumparporten) 
                        } /*else if ((buf[UDP_DST_PORT_H_P]==((locDumperPrt>>8) & 0xff)) && (buf[UDP_DST_PORT_L_P] == (locDumperPrt & 0xff))) {
                	        if (gotdumpserver == 0) {
				                while(i<6){
					                remotemac[i]=buf[ETH_SRC_MAC +i];
					                i++;
						        }
						        i=0;
				                while(i<4){
					                remoteip[i]=buf[IP_SRC_P +i];
					                i++;
						        }
	                        	i=0;
								
	                        	gotdumpserver = 1;
	                        	make_udp_reply_from_request(buf,"Got server",10,rmDumperPrt);
                	        } else {
                	        	make_udp_reply_from_request(buf,"Already got server",18,rmDumperPrt);
                	        }
                        }*/
	                }			            
			    }
	        }
        }
        

                
		
	}
	
	return 0;
}
