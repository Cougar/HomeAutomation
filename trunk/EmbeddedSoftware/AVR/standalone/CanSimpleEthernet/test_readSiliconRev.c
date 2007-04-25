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
#include <stdlib.h>
#include <string.h>

/* lib files */
#include <ip_arp_udp_tcp.h>
#include <enc28j60.h>
#include <timeout.h>
#include <avr_compat.h>
#include <net.h>


// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x25};
static uint8_t myip[4] = {193,11,254,22};
static uint16_t myport =1200; // listen port for udp
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX

#define BUFFER_SIZE 550
static uint8_t buf[BUFFER_SIZE+1];

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf,uint8_t rev)
{
        char vstr[5];
        uint16_t plen;
        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
        plen=fill_tcp_data_p(buf,plen,PSTR("<center><p>ENC28J60 silicon rev is: "));
        itoa((enc28j60getrev()),vstr,10);
        plen=fill_tcp_data(buf,plen,vstr);
        plen=fill_tcp_data_p(buf,plen,PSTR("</center><hr><br>ver 2.2, tuxgraphics.org\n"));
        return(plen);
}

int main(void){

        
        uint16_t plen;
        uint16_t dat_p;
        uint8_t i=0;
        uint8_t payloadlen=0;
        char str[20];
        
        // set the clock speed to 8MHz
        // set the clock prescaler. First write CLKPCE to enable setting of clock the
        // next four instructions.
        CLKPR=(1<<CLKPCE);
        CLKPR=0; // 8 MHZ
        
        /* enable PB0, reset as output */
        DDRB|= (1<<DDB0);

        /* set output to gnd, reset the ethernet chip */
        PORTB &= ~(1<<PB0);
        delay_ms(30);
        /* set output to Vcc, reset inactive */
        PORTB|= (1<<PB0);
        delay_ms(200);
        
        /*initialize enc28j60*/
        enc28j60Init(mymac);
        delay_ms(20);
        
        // LED
        /* enable PB1, LED as output */
        DDRD|= (1<<DDD6);

        /* set output to Vcc, LED off */
        PORTD|= (1<<PD6);
        
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

                // tcp port 80 start
                if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==80){
                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
                                make_tcp_synack_from_syn(buf);
                                // make_tcp_synack_from_syn does already send the syn,ack
                                continue;
                        }
                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
                                init_len_info(buf); // init some data structures
                                // we can possibly have no data, just ack:
                                dat_p=get_tcp_data_pointer();
                                if (dat_p==0){
                                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
                                                // finack, answer with ack
                                                make_tcp_ack_from_any(buf);
                                        }
                                        // just an ack with no data, wait for next packet
                                        continue;
                                }
                                if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
                                        // head, post and other methods:
                                        //
                                        // for possible status codes see:
                                        // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                                        plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                                        goto SENDTCP;
                                }
                                // any GET, any url:
                                plen=print_webpage(buf,enc28j60getrev());
SENDTCP:
                                make_tcp_ack_from_any(buf); // send ack for http get
                                make_tcp_ack_with_data(buf,plen); // send data
                                continue;
                        }
                }
                // tcp port 80 end
                //
                // udp start, we listen on udp port 1200=0x4B0
                if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0){
                        payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
                        // you must sent a string starting with v
                        // e.g udpcom version 10.0.0.24
                        if (buf[UDP_DATA_P]=='v' ){
                                str[0]='v';
                                str[1]='e';
                                str[2]='r';
                                str[3]='=';
                                itoa((enc28j60getrev()),&(str[4]),10);
                                make_udp_reply_from_request(buf,str,strnlen(str,10),myport);
                        }
                }
        }
        return (0);
}
