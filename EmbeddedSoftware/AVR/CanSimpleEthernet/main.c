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
/* lib files */
#include <can.h>
//#include <serial.h>
#include <timebase.h>
#include <ip_arp_udp_tcp.h>
#include <enc28j60.h>
#include <timeout.h>
#include <avr_compat.h>
#include <net.h>

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
static uint8_t myip[4] = {193,11,254,22};
static uint16_t rmDumperPrt =1200; // remote port for dumping data
static uint16_t rmCan2SerPrt =1100; // remote port for Can2Serial-data
static uint16_t locDumperPrt =1200; // listen port 
static uint16_t locCan2SerPrt =1100; // listen port for Can2Serial-data
static uint8_t gotserver = 0;

// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX

static uint8_t remotemac[6];
static uint8_t remoteip[4];

#define BUFFER_SIZE 250
static uint8_t buf[BUFFER_SIZE+1];

#define SEND_BUFFER_SIZE 20
static char sendbuf[SEND_BUFFER_SIZE+1];
static uint8_t buffpoint;
 
/*----------------------------------------------------------------------------
 * Putchar for udp
 * Implements a small buffer, sends packet over udp if buffer is full or 
 * \n-char is sent.
 * Needs a timeout to force send. 
 *--------------------------------------------------------------------------*/
int udp_putchar(char c, FILE* stream) {
	if (gotserver != 0) {
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

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

	//ethernet vars
    uint16_t plen = 0;
    uint8_t ledi=0;
    uint8_t payloadlen=0;
	buffpoint = 0;
	
	//can vars
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = 3;
	txMsg.DataLength = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; //DataLength and the databytes are just what happens to be in the memory. They are never set.


 	uint32_t timeStamp = 0;
	
	Mcu_Init();
	Timebase_Init();
	//Serial_Init();

    //alla printf ska skriva till udp
    stdout = &mystdout;    //set the output stream 
    
	sei();

//	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
//		printf("FAILED!\n");
	}
	else {
//		printf("OK!\n");
	}

    /* enable PB0, reset as output */
    DDRB|= (1<<DDB0);
    /* set output to gnd, reset the ethernet chip */
    PORTB &= ~(1<<PB0);
    delay_ms(20);
    /* set output to Vcc, reset inactive */
    PORTB|= (1<<PB0);
    delay_ms(100);
    
    // LED
    /* enable PB1, LED as output */
    DDRD|= (1<<DDD6);
    /* set output to Vcc, LED off */
    PORTD|= (1<<PD6);

	//printf("Init enc\n");
    /*initialize enc28j60*/
    enc28j60Init(mymac);
    delay_ms(20);
    
    /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
    // LEDB=yellow LEDA=green
    //
    // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
    enc28j60PhyWrite(PHLCON,0x476);
    delay_ms(20);
    
    /* set output to GND, red LED on */
    PORTD &= ~(1<<PD6);
    ledi=1;
    
    //init the ethernet/ip layer:
    init_ip_arp_udp_tcp(mymac,myip,80);

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 2000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			//txMsg.Id++;
			//txMsg.Id = 3;
			//Can_Send(&txMsg);
			
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");
    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
	   		printf("}\n");
	   		
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
                        	printf("hej %x %x\n", buf[UDP_DST_PORT_H_P], buf[UDP_DST_PORT_L_P]);
                        if ((buf[UDP_DST_PORT_H_P]==((locCan2SerPrt>>8) & 0xff)) && (buf[UDP_DST_PORT_L_P] == (locCan2SerPrt & 0xff))) {
                        	printf("hoj\n");
                        } else if (buf[UDP_DATA_P]=='t' && payloadlen==5){
                	        uint8_t i=0;
                	        if (gotserver == 0) {
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
								
	                        	gotserver = 1;
	                        	make_udp_reply_from_request(buf,"Got server",10,rmDumperPrt);
                	        } else {
                	        	make_udp_reply_from_request(buf,"Already got server",18,rmDumperPrt);
                	        }
                        }
	                }			            
			    }
	        }
        }
        

                
		
	}
	
	return 0;
}
