/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright: GPL V2
 *
 * Ethernet remote device and sensor
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 *********************************************/
/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>

/* lib files */
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
static uint16_t myport =1200; // listen port for udp
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX

#define BUFFER_SIZE 250
static uint8_t buf[BUFFER_SIZE+1];

int main(void){

        
        uint16_t plen;
        uint8_t i=0;
        uint8_t payloadlen=0;
        
        // set the clock speed to 8MHz
        // set the clock prescaler. First write CLKPCE to enable setting of clock the
        // next four instructions.
        CLKPR=(1<<CLKPCE);
        CLKPR=0; // 8 MHZ
        
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

        /*initialize enc28j60*/
        enc28j60Init(mymac);
        delay_ms(20);
        
        /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
        // LEDB=yellow LEDA=green
        //
        // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
        // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
        enc28j60PhyWrite(PHLCON,0x476);
        delay_ms(20);
        
        /* set output to GND, red LED on */
        PORTD &= ~(1<<PD6);
        i=1;
        
        //init the ethernet/ip layer:
        init_ip_arp_udp_tcp(mymac,myip,80);

        while(1){
                // get the next new packet:
                plen = enc28j60PacketReceive(BUFFER_SIZE, buf);

                /*plen will ne unequal to zero if there is a valid 
                 * packet (without crc error) */
                if(plen==0){
                        continue;
                }
                // arp is broadcast if unknown but a host may also
                // verify the mac address by sending it to 
                // a unicast address.
                if(eth_type_is_arp_and_my_ip(buf,plen)){
                        make_arp_answer_from_request(buf);
                        continue;
                }
                // check if ip packets (icmp or udp) are for us:
                if(eth_type_is_ip_and_my_ip(buf,plen)==0){
                        continue;
                }
                
                if (i){
                        /* set output to Vcc, LED off */
                        PORTD|= (1<<PD6);
                        i=0;
                }else{
                        /* set output to GND, LED on */
                        PORTD &= ~(1<<PD6);
                        i=1;
                }

                        
                if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V){
                        // a ping packet, let's send pong
                        make_echo_reply_from_request(buf,plen);
                        continue;
                }
                if (buf[IP_PROTO_P]==IP_PROTO_UDP_V){
                        payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
                        if (buf[UDP_DATA_P]=='t' && payloadlen==5){
                                make_udp_reply_from_request(buf,"hello",6,myport);
                        }
                }
        }
        return (0);
}
