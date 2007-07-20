/*********************************************************************
 *
 *                  Microchip TCP/IP Stack Definitions
 *
 *********************************************************************
 * FileName:        StackTsk.h
 * Dependencies:    Compiler.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/10/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     8/7/03  Rev 2.21 - TFTP Client addition
 * Howard Schlunder		9/30/04	Added MCHP_MAC, MAC_POWER_ON_TEST, 
 								EEPROM_BUFFER_SIZE, USE_LCD
 * Howard Schlunder		8/09/06	Removed MCHP_MAC, added STACK_USE_NBNS, 
 *								STACK_USE_DNS, and STACK_USE_GENERIC_TCP_EXAMPLE
 ********************************************************************/
#ifndef STACK_TSK_H
#define STACK_TSK_H

#include "..\Include\GenericTypeDefs.h"
#include "..\Include\Compiler.h"

/*
 * This value is used by TCP to implement timeout actions.
 * If SNMP module is in use, this value should be 100 as required
 * by SNMP protocol unless main application is providing separate
 * tick which 10mS.
 */
#define TICKS_PER_SECOND               (100)        // 10ms

#if (TICKS_PER_SECOND < 10 || TICKS_PER_SECOND > 255)
#error Invalid TICKS_PER_SECONDS specified.
#endif

/*
 * Manually select prescale value to achieve necessary tick period
 * for a given clock frequency.
 */
#define TICK_PRESCALE_VALUE             (256)

#if (TICK_PRESCALE_VALUE != 2 && \
     TICK_PRESCALE_VALUE != 4 && \
     TICK_PRESCALE_VALUE != 8 && \
     TICK_PRESCALE_VALUE != 16 && \
     TICK_PRESCALE_VALUE != 32 && \
     TICK_PRESCALE_VALUE != 64 && \
     TICK_PRESCALE_VALUE != 128 && \
     TICK_PRESCALE_VALUE != 256 )
#error Invalid TICK_PRESCALE_VALUE specified.
#endif

#if defined(WIN32)
    #undef TICKS_PER_SECOND
    #define TICKS_PER_SECOND        (1)
#endif


/*
 * This value is specific to the Microchip Ethernet controllers.  
 * If a different Ethernet controller is used, this define is not
 * used.  If a Microchip controller is used and a self memory test 
 * should be done when the MACInit() function is called, 
 * uncomment this define.  The test requires ~512 bytes of 
 * program memory.
 */
//#define MAC_POWER_ON_TEST


/*
 * This value is specific to the Microchip Ethernet controllers.  
 * If a different Ethernet controller is used, this define is not
 * used.  Ideally, when MAC_FILTER_BROADCASTS is defined, all 
 * broadcast packets that are received would be discarded by the 
 * hardware, except for ARP requests for our IP address.  This could 
 * be accomplished by filtering all broadcasts, but allowing the ARPs
 * using the patter match filter.  The code for this feature has been
 * partially implemented, but it is not complete nor tested, so this
 * option should remain unused in this stack version.
 */
//#define MAC_FILTER_BROADCASTS


/*
 * SPI Serial EEPROM buffer size.  To enhance performance while
 * cooperatively sharing the SPI bus with other peripherals, bytes 
 * read and written to the memory are locally buffered.  This 
 * parameter has no effect if spieeprom.c is not included in the 
 * project.  Legal sizes are 1 to the EEPROM page size.
 */
#define EEPROM_BUFFER_SIZE    			(12)


/*
 * This value is for Microchip 24LC256 - 256kb serial EEPROM
 */
#define EEPROM_CONTROL                  (0xa0)


/*
 * Number of bytes to be reserved before MPFS storage is to start.
 *
 * These bytes host application configurations such as IP Address,
 * MAC Address, and any other required variables.
 *
 * After making any change to this variable, MPFS.exe must be
 * executed with correct block size.
 * See MPFS.exe help message by executing MPFS /?
 */
#define MPFS_RESERVE_BLOCK              (64)



/*
 * Modules to include in this project
 * For demo purpose only, each sample project defines one or more
 * of following defines in compiler command-line options. (See
 * each MPLAB Project Node Properties under "Project->Edit Project" menu.
 * In real applcation, user may want to define them here.
 */
#define STACK_USE_ICMP
#define STACK_USE_HTTP_SERVER
#define STACK_USE_SGP_SERVER
#define STACK_USE_TIMESYNC
//#define STACK_USE_SLIP		// Not currently supported
//#define STACK_USE_IP_GLEANING
//#define STACK_USE_DHCP
//#define STACK_USE_SNMP_SERVER
//#define STACK_USE_TFTP_CLIENT
#define STACK_USE_DNS					// Domain Name Service Client
//#define STACK_USE_NBNS					// NetBIOS Name Service Server

/*
 * Following low level modules are automatically enabled/disabled based on high-level
 * module selections.
 * If you need them with your custom application, enable it here.
 */
//#define STACK_USE_TCP
//#define STACK_USE_UDP

// Uncomment following line if SNMP TRAP support is required
//#define SNMP_TRAP_DISABED

/*
 * When SLIP is used, DHCP is not supported.
 */
#if defined(STACK_USE_SLIP)
#undef STACK_USE_DHCP
#endif



/*
 * Comment following line if StackTsk should wait for acknowledgement
 * from remote host before transmitting another packet.
 * Commenting following line may reduce throughput.
 */
//#define TCP_NO_WAIT_FOR_ACK


/*
 * Uncomment following line if this stack will be used in CLIENT
 * mode.  In CLIENT mode, some functions specific to client operation
 * are enabled.
 */
//#define STACK_CLIENT_MODE


/*
 * If html pages are stored in internal program memory,
 * uncomment MPFS_USE_PRGM and comment MPFS_USE_EEPROM
 * If html pages are stored in external eeprom memory,
 * comment MPFS_USE_PRGM and uncomment MPFS_USE_EEPROM
 */
//#define MPFS_USE_PGRM
//#define MPFS_USE_EEPROM

#if defined(MPFS_USE_PGRM) && defined(MPFS_USE_EEPROM)
#error Invalid MPFS Storage option specified.
#endif

#if !defined(MPFS_USE_PGRM) && !defined(MPFS_USE_EEPROM)
#error You have not specified MPFS storage option.
#endif


/*
 * When HTTP is enabled, TCP must be enabled.
 */
#if defined(STACK_USE_HTTP_SERVER)
    #if !defined(STACK_USE_TCP)
        #define STACK_USE_TCP
    #endif
#endif

/*
 * When FTP is enabled, TCP must be enabled.
 */
#if defined(STACK_USE_FTP_SERVER)
    #if !defined(STACK_USE_TCP)
        #define STACK_USE_TCP
    #endif
#endif


#if (defined(STACK_USE_FTP_SERVER) || \
	 defined(STACK_USE_SNMP_SERVER) || \
	 defined(STACK_USE_DNS) || \
	 defined(STACK_USE_GENERIC_TCP_EXAMPLE) || \
	 defined(STACK_USE_TFTP_CLIENT) )
	#if !defined(STACK_CLIENT_MODE)
	    #define STACK_CLIENT_MODE
	#endif
#endif

/*
 * When DHCP is enabled, UDP must also be enabled.
 */
#if defined(STACK_USE_DHCP) || \
	defined(STACK_USE_DNS) || \
	defined(STACK_USE_NBNS) || \
	defined(STACK_USE_SNMP_SERVER) || \
	defined(STACK_USE_TFTP_CLIENT) || \
	defined(STACK_USE_ANNOUNCE)
    #if !defined(STACK_USE_UDP)
        #define STACK_USE_UDP
    #endif
#endif

/*
 * When IP Gleaning is enabled, ICMP must also be enabled.
 */
#if defined(STACK_USE_IP_GLEANING)
    #if !defined(STACK_USE_ICMP)
        #define STACK_USE_ICMP
    #endif
#endif


/*
 * DHCP requires unfragmented packet size of at least 328 bytes,
 * and while in SLIP mode, our maximum packet size is less than
 * 255.  Hence disallow DHCP module while SLIP is in use.
 * If required, one can use DHCP while SLIP is in use by modifying
 * C18 linker scipt file such that C18 compiler can allocate
 * a static array larger than 255 bytes.
 * Due to very specific application that would require this,
 * sample stack does not provide such facility.  Interested users
 * must do this on their own.
 */
#if defined(STACK_USE_SLIP)
    #if defined(STACK_USE_DHCP)
        #error DHCP cannot be used when SLIP is enabled.
    #endif
#endif


//
// Default Address information - If not found in data EEPROM.
//
#define MY_DEFAULT_HOST_NAME			"ETEMP"

#define MY_DEFAULT_MAC_BYTE1            (0x00)
#define MY_DEFAULT_MAC_BYTE2            (0x10)
#define MY_DEFAULT_MAC_BYTE3            (0xA7)
#define MY_DEFAULT_MAC_BYTE4            (0x02)
#define MY_DEFAULT_MAC_BYTE5            (0xF2)
#define MY_DEFAULT_MAC_BYTE6            (0xBF)

#define MY_DEFAULT_IP_ADDR_BYTE1        (193)
#define MY_DEFAULT_IP_ADDR_BYTE2        (11)
#define MY_DEFAULT_IP_ADDR_BYTE3        (249)
#define MY_DEFAULT_IP_ADDR_BYTE4        (228)

#define MY_DEFAULT_MASK_BYTE1           (255)
#define MY_DEFAULT_MASK_BYTE2           (255)
#define MY_DEFAULT_MASK_BYTE3           (255)
#define MY_DEFAULT_MASK_BYTE4           (0)

#define MY_DEFAULT_GATE_BYTE1           MY_DEFAULT_IP_ADDR_BYTE1
#define MY_DEFAULT_GATE_BYTE2           MY_DEFAULT_IP_ADDR_BYTE2
#define MY_DEFAULT_GATE_BYTE3           MY_DEFAULT_IP_ADDR_BYTE3
#define MY_DEFAULT_GATE_BYTE4           (1)

#define MY_DEFAULT_DNS_BYTE1			MY_DEFAULT_GATE_BYTE1
#define MY_DEFAULT_DNS_BYTE2			MY_DEFAULT_GATE_BYTE2
#define MY_DEFAULT_DNS_BYTE3			MY_DEFAULT_GATE_BYTE3
#define MY_DEFAULT_DNS_BYTE4			MY_DEFAULT_GATE_BYTE4

/*
 * TCP configurations
 * To minmize page update, match number of sockets and
 * HTTP connections with different page sources in a
 * page.
 * For example, if page contains reference to 3 more pages,
 * browser may try to open 4 simultaneous HTTP connections,
 * and to minimize browser delay, set HTTP connections to
 * 4, MAX_SOCKETS to 4.
 * If you are using other applications, you should
 * keep at least one socket available for them.
 */

// Maximum TCP sockets to be defined.
// Note that each TCP socket consumes 38 bytes of RAM.
#define MAX_SOCKETS         (6u)

// Maximum avaialble UDP Sockets
#define MAX_UDP_SOCKETS     (4u)


#if (MAX_SOCKETS <= 0 || MAX_SOCKETS > 255)
#error Invalid MAX_SOCKETS value specified.
#endif

#if (MAX_UDP_SOCKETS <= 0 || MAX_UDP_SOCKETS > 255 )
#error Invlaid MAX_UDP_SOCKETS value specified
#endif


#if !defined(STACK_USE_SLIP)
	// The MAC_TX_BUFFER_COUNT must be equal to MAX_SOCKETS + 1
	// (for high priority messages), or else calls to TCPPut may 
	// fail when multiple TCP sockets have data pending in the 
	// output buffer that hasn't been acked.  Changing this value
	// is recommended only if the rammifications of doing so are 
	// properly understood.  
	#if defined(NON_MCHP_MAC)
    	#define MAC_TX_BUFFER_SIZE          (1024)
    	#define MAC_TX_BUFFER_COUNT         (1)
	#else
	    #define MAC_TX_BUFFER_SIZE          (576)
    	#define MAC_TX_BUFFER_COUNT         (MAX_SOCKETS+1)
	#endif
#else
/*
 * For SLIP, there can only be one transmit and one receive buffer.
 * Both buffer must fit in one bank.  If bigger buffer is required,
 * you must manually locate tx and rx buffer in different bank
 * or modify your linker script file to support arrays bigger than
 * 256 bytes.
 */
    #define MAC_TX_BUFFER_SIZE          (250)
    #define MAC_TX_BUFFER_COUNT         (1)
#endif
// Rests are Receive Buffers

#define MAC_RX_BUFFER_SIZE              (MAC_TX_BUFFER_SIZE)

#if (MAC_TX_BUFFER_SIZE <= 0 || MAC_TX_BUFFER_SIZE > 1500 )
#error Invalid MAC_TX_BUFFER_SIZE value specified.
#endif

#if ( (MAC_TX_BUFFER_SIZE * MAC_TX_BUFFER_COUNT) > (5* 1024) )
#error Warning, receive buffer is small.  Excessive packet loss may occur.
#endif

/*
 * Maximum numbers of simultaneous HTTP connections allowed.
 * Each connection consumes 10 bytes.
 */
#define MAX_HTTP_CONNECTIONS            (3u)

#if (MAX_HTTP_CONNECTIONS <= 0 || MAX_HTTP_CONNECTIONS > 255 )
#error Invalid MAX_HTTP_CONNECTIONS value specified.
#endif

#define AVAILABLE_SOCKETS (MAX_SOCKETS)
#if defined(STACK_USE_HTTP_SERVER)
    #define AVAILABLE_SOCKETS2 (AVAILABLE_SOCKETS - MAX_HTTP_CONNECTIONS)
#else
    #define AVAILABLE_SOCKETS2  (MAX_SOCKETS)
#endif

/*
 * When using FTP, you must have at least two sockets free
 */
#if defined(STACK_USE_FTP_SERVER)
    #define AVAILABLE_SOCKETS3 (AVAILABLE_SOCKETS2 - 2)
#else
    #define AVAILABLE_SOCKETS3  (AVAILABLE_SOCKETS2)
#endif

#if AVAILABLE_SOCKETS3 < 0 || AVAILABLE_SOCKETS3 > MAX_SOCKETS
    #error MAX_SOCKETS is too low to accommodate MAX_HTTP_CONNECTIONS and/or STACK_USE_FTP_SERVER
    #error Either increase MAX_SOCKETS in StackTsk.h or decrease module socket usage.
#endif


#define AVAILABLE_UDP_SOCKETS       (MAX_UDP_SOCKETS)
#if defined(STACK_USE_DHCP)
    #define AVAILABLE_UDP_SOCKETS2       (AVAILABLE_UDP_SOCKETS - 1)
#else
    #define AVAILABLE_UDP_SOCKETS2      AVAILABLE_UDP_SOCKETS
#endif

#if defined(STACK_USE_SNMP_SERVER)
    #define AVAILABLE_UDP_SOCKETS3      (AVAILABLE_UDP_SOCKETS2 - 1)
#else
    #define AVAILABLE_UDP_SOCKETS3      AVAILABLE_UDP_SOCKETS2
#endif

#if defined(STACK_USE_TFTP_CLIENT)
    #define AVAILABLE_UDP_SOCKETS4      (AVAILABLE_UDP_SOCKETS2)
#else
    #define AVAILABLE_UDP_SOCKETS4      AVAILABLE_UDP_SOCKETS3
#endif


#if AVAILABLE_UDP_SOCKETS4 < 0 || AVAILABLE_UDP_SOCKETS4 > MAX_UDP_SOCKETS
    #error MAX_UDP_SOCKETS is too low to accommodate DHCP, SNMP, and/or TFTP client
    #error Either increase MAX_UDP_SOCKETS in StackTsk.h or decrease module UDP socket usage.
#endif


typedef BYTE BUFFER;

typedef struct _MAC_ADDR
{
    BYTE v[6];
} MAC_ADDR;

typedef union _IP_ADDR
{
    BYTE        v[4];
    DWORD       Val;
} IP_ADDR;


typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;
    IP_ADDR     IPAddr;
} NODE_INFO;

typedef struct _APP_CONFIG
{
    IP_ADDR     MyIPAddr;
    MAC_ADDR    MyMACAddr;
    IP_ADDR     MyMask;
    IP_ADDR     MyGateway;
	IP_ADDR		PrimaryDNSServer;
    struct
    {
        unsigned char bIsDHCPEnabled : 1;
        unsigned char bInConfigMode : 1;
    } Flags;
    WORD_VAL    SerialNumber;
    IP_ADDR     SMTPServerAddr;     // Not used.
    IP_ADDR     TFTPServerAddr;     // Not used.
	BYTE		NetBIOSName[16];
} APP_CONFIG;


#ifndef THIS_IS_STACK_APPLICATION
    extern APP_CONFIG AppConfig;
#endif

#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP)
    #define StackIsInConfigMode()   (AppConfig.Flags.bInConfigMode)
#else
    #define StackIsInConfigMode()   (FALSE)
#endif


/*********************************************************************
 * Function:        void StackInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Stack and its componentns are initialized
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines be used.
 *
 ********************************************************************/
void StackInit(void);


/*********************************************************************
 * Function:        void StackTask(void)
 *
 * PreCondition:    StackInit() is already called.
 *
 * Input:           None
 *
 * Output:          Stack FSM is executed.
 *
 * Side Effects:    None
 *
 * Note:            This FSM checks for new incoming packets,
 *                  and routes it to appropriate stack components.
 *                  It also performs timed operations.
 *
 *                  This function must be called periodically called
 *                  to make sure that timely response.
 *
 ********************************************************************/
void StackTask(void);


#endif
