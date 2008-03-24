/*********************************************************************
 *
 *               Microchip TCP/IP Stack FSM Implementation on PIC18
 *
 *********************************************************************
 * FileName:        StackTsk.c
 * Dependencies:    StackTsk.H
 *                  ARPTsk.h
 *                  MAC.h
 *                  IP.h
 *                  ICMP.h
 *                  Tcp.h
 *                  http.h
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     12/5/02     Modified UDPProcess() and TCPProcess()
 *                                  to include localIP as third param.
 *                                  This was done to allow these functions
 *                                  to calculate checksum correctly.
 * Nilesh Rajbharti     7/26/04     Added code in StackTask() to not
 *                                  clear statically IP address if link is
 *                                  removed and DHCP module is disabled
 *                                  at runtime.
********************************************************************/

#define STACK_INCLUDE

#include "..\Include\StackTsk.h"
#include "..\Include\ARPTsk.h"
#include "..\Include\MAC.h"
#include "..\Include\IP.h"

#if defined(STACK_USE_TCP)
#include "..\Include\TCP.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "..\Include\HTTP.h"
#endif

#if defined(STACK_USE_ICMP)
    #include "..\Include\ICMP.h"
#endif

#if defined(STACK_USE_UDP)
    #include "..\Include\UDP.h"
#endif

#if defined(STACK_USE_DHCP)
    #include "..\Include\DHCP.h"
#endif

	// myDHCPBindCount defined in MainDemo.c;  Used to force an IP 
	// address display update for IP Gleaning
    extern BYTE myDHCPBindCount;


/*
 * Stack FSM states.
 */
typedef enum _SM_STACK
{
    SM_STACK_IDLE,
    SM_STACK_MAC,
    SM_STACK_IP,
    SM_STACK_ICMP,
    SM_STACK_ICMP_REPLY,
    SM_STACK_ARP,
    SM_STACK_TCP,
    SM_STACK_UDP
} SM_STACK;
static SM_STACK smStack;

NODE_INFO remoteNode;



/*********************************************************************
 * Function:        void StackInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Stack and its componets are initialized
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines are used.
 *
 ********************************************************************/
void StackInit(void)
{
    smStack                     = SM_STACK_IDLE;

#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP)
    /*
     * If DHCP or IP Gleaning is enabled,
     * startup in Config Mode.
     */
    AppConfig.Flags.bInConfigMode = TRUE;

#endif

    MACInit();

    ARPInit();

#if defined(STACK_USE_UDP)
    UDPInit();
#endif

#if defined(STACK_USE_TCP)
    TCPInit();
#endif

}


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
 *                  This function must be called periodically to
 *                  ensure timely responses.
 *
 ********************************************************************/
void StackTask(void)
{
    static WORD dataCount;

#if defined(STACK_USE_ICMP)
    static BYTE data[MAX_ICMP_DATA_LEN];
    static WORD ICMPId;
    static WORD ICMPSeq;
#endif
    IP_ADDR tempLocalIP;


    union
    {
        BYTE MACFrameType;
        BYTE IPFrameType;
        ICMP_CODE ICMPCode;
    } type;


    BOOL lbContinue;

	do
    {
        lbContinue = FALSE;

        switch(smStack)
        {
        case SM_STACK_IDLE:
        case SM_STACK_MAC:

            if ( !MACGetHeader(&remoteNode.MACAddr, &type.MACFrameType) )
            {
                #if defined(STACK_USE_DHCP)
                    // Normally, an application would not include  DHCP module
                    // if it is not enabled. But in case some one wants to disable
                    // DHCP module at run-time, remember to not clear our IP
                    // address if link is removed.
                    if ( AppConfig.Flags.bIsDHCPEnabled )
                    {
                        if ( !MACIsLinked() )
                        {
							AppConfig.MyIPAddr.Val = 0x00000000ul;
                            AppConfig.Flags.bInConfigMode = TRUE;
                            DHCPReset();
                        }
                    }
                #endif

                break;
            }

            lbContinue = TRUE;
            if ( type.MACFrameType == MAC_IP )
                smStack = SM_STACK_IP;
            else if ( type.MACFrameType == MAC_ARP )
                smStack = SM_STACK_ARP;
            else
                MACDiscardRx();
            break;

        case SM_STACK_ARP:
            if ( ARPProcess() )
                smStack = SM_STACK_IDLE;
            break;

        case SM_STACK_IP:
            if ( IPGetHeader(&tempLocalIP,
                             &remoteNode,
                             &type.IPFrameType,
                             &dataCount) )
            {
                lbContinue = TRUE;
                if ( type.IPFrameType == IP_PROT_ICMP )
                {
                    smStack = SM_STACK_ICMP;

#if defined(STACK_USE_IP_GLEANING)
                    if(AppConfig.Flags.bInConfigMode && AppConfig.Flags.bIsDHCPEnabled)
                    {
                        /*
                         * Accoriding to "IP Gleaning" procedure,
                         * when we receive an ICMP packet with a valid
                         * IP address while we are still in configuration
                         * mode, accept that address as ours and conclude
                         * configuration mode.
                         */
                        if( tempLocalIP.Val != 0xffffffff )
                        {
                            AppConfig.Flags.bInConfigMode = FALSE;
							AppConfig.MyIPAddr = tempLocalIP;
							myDHCPBindCount--;
                        }
                    }
#endif
                }

#if defined(STACK_USE_TCP)
                else if ( type.IPFrameType == IP_PROT_TCP )
                    smStack = SM_STACK_TCP;
#endif

#if defined(STACK_USE_UDP)
                else if ( type.IPFrameType == IP_PROT_UDP )
                    smStack = SM_STACK_UDP;
#endif

                else	// Unknown/unsupported higher level protocol
                {
                    lbContinue = FALSE;
                    MACDiscardRx();

                    smStack = SM_STACK_IDLE;
                }
            }
            else	// Improper IP header version or checksum
            {
                MACDiscardRx();
                smStack = SM_STACK_IDLE;
            }
            break;

#if defined(STACK_USE_UDP)
        case SM_STACK_UDP:
            if ( UDPProcess(&remoteNode, &tempLocalIP, dataCount) )
                smStack = SM_STACK_IDLE;
            break;
#endif

#if defined(STACK_USE_TCP)
        case SM_STACK_TCP:
            if ( TCPProcess(&remoteNode, &tempLocalIP, dataCount) )
                smStack = SM_STACK_IDLE;
            break;
#endif

        case SM_STACK_ICMP:
            smStack = SM_STACK_IDLE;

#if defined(STACK_USE_ICMP)
            if ( dataCount <= (MAX_ICMP_DATA_LEN+8) )
            {
                if ( ICMPGet(&type.ICMPCode,
                             data,
                             (BYTE*)&dataCount,
                             &ICMPId,
                             &ICMPSeq) )
                {
					if ( type.ICMPCode == ICMP_ECHO_REQUEST )
					{
						lbContinue = TRUE;
						smStack = SM_STACK_ICMP_REPLY;
					}
                }
            }
#endif
            MACDiscardRx();
            break;

#if defined(STACK_USE_ICMP)
        case SM_STACK_ICMP_REPLY:
            if ( ICMPIsTxReady() )
            {
                ICMPPut(&remoteNode,
                        ICMP_ECHO_REPLY,
                        data,
                        (BYTE)dataCount,
                        ICMPId,
                        ICMPSeq);

                smStack = SM_STACK_IDLE;
            }
            break;
#endif

        }

    } while(lbContinue);

#if defined(STACK_USE_TCP)
    // Perform timed TCP FSM.
    TCPTick();
#endif


#if defined(STACK_USE_DHCP)
    /*
     * DHCP must be called all the time even after IP configuration is
     * discovered.
     * DHCP has to account lease expiration time and renew the configuration
     * time.
     */
    DHCPTask();

    if(DHCPIsBound())
        AppConfig.Flags.bInConfigMode = FALSE;

#endif
}

