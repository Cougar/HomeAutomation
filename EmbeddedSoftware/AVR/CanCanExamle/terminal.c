/******************************************************************************
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *****************************************************************************
 *
 * File    : terminal.c
 * Version : 0.9
 * 
 * Summary : Terminal
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "termio.h"
#include "terminal.h"

#include "debughelper.h"
#include "gpio.h"
#include "mcp2515.h"

#define CEOS		'\0'
#define CENTER		'\r'
#define CNEWLINE	'\n'
#define CBS			'\b'
#define CBLANK		' '

#define CMAXCL			50
#define strncmp_flash(sram,sflash,n) strncmp_P(sram,PSTR(sflash),n)

#define GET_TERMSTRING_STATE_INIT    (0)
#define GET_TERMSTRING_STATE_SAMPLE  (1)
#define GET_TERMSTRING_STATE_RETURN  (2)
#define GET_TERMSTRING_STATE_BREAK   (3)

uint8_t gTimedSend = CANAUTOOFF;

static void ShowHelp(void)
{
	term_puts_P("exit - exit from shell\n");
	term_puts_P("show sys - print system values\n");
	term_puts_P("led on|off - all LEDs on/off\n");
	term_puts_P("led toogle num - toogle LED num (1-3)\n");
	term_puts_P("mcp stat - show mcp status byte\n");
	term_puts_P("mcp error - show mcp error counters and error flag\n");
	term_puts_P("can read - print all available messages\n");
	term_puts_P("can send ext|std num - send can mesg std. or extended with 3rd byte=num\n");
	term_puts_P("can timed on|off - (de)activte sending messages (to debug receiver)\n");
	term_puts_P("can auto on|off - (de)activte autoprocessing of incoming messages\n\n");

}

static void DumpConfigValues(void)
{
	term_puts_P("Config-Dump :\n");
	term_puts_P("Auto Processing of CAN-Message: ");
	if (gCANAutoProcess) {
		term_puts_P("On");
	}
	else {
		term_puts_P("Off");
	}
	term_puts_P("\n");
	
	term_puts_P("Auto Send of CAN-Message: ");
	(gTimedSend) ? term_puts_P("On") : term_puts_P("Off");
	term_puts_P("\n");
}

static void GetConsoleString(uint8_t *state, char* s, uint8_t mxc)
{
	static uint8_t len; // current length
	
	uint16_t getcres;
	uint8_t c;
	
	if (*state == GET_TERMSTRING_STATE_INIT ) {
		*state = GET_TERMSTRING_STATE_SAMPLE;
		s[0] = CEOS;
		len = 0;
	}

	getcres = term_getc();
		
	if ( (getcres & 0xff00) == 0 ) {
		// got a character
		//Debug_ByteToUart_p(PSTR("getMSB"), getcres>>8);
		//Debug_ByteToUart_p(PSTR("getLSB"), getcres & 0xff);
		c = (uint8_t) (getcres & 0x00ff);
		switch (c) {
		case CBS: 
			if (len>0) {
				s[--len]=CEOS;
				term_putc(CBS);
				term_putc(CBLANK);
				term_putc(CBS);
			}
			break;
		case CENTER: 
			s[len]=CEOS;
			// term_putc(CNEWLINE);
			*state = GET_TERMSTRING_STATE_RETURN;
			break;
		default: 
			if (len<mxc-1) {
				s[len++]=c;
				term_putc(c);
			}
			break;
		}
	}
}

static char* GetCLParmStr(char cl[], uint8_t n)
{
	uint8_t i=0, ac=0;

	// skip leading whitepaces if any
	while( (cl[i]!= CEOS) && (cl[i]==CBLANK)) i++;
	
	// go to n-th arg (0 is the command)
	for(ac=0; ac<n; ac++)
	{
		// find first character of argument
		while( (cl[i]!= CEOS) && (cl[i]!=CBLANK)) i++;
		// skip whitespaces separating arguments
		while( (cl[i]!= CEOS) && (cl[i]==CBLANK)) i++;
	}
	// return address of first character in n-th argument
	return &cl[i];
}

#define ERRUNKNOWN 2
static void TellError(uint8_t ecode)
{
	switch (ecode) {
	case (ERRUNKNOWN):
		term_puts_P("Command or Option unknown\n");
		break;
	default:
		term_puts_P("Invalid Error-Code\n");
		break;
	}
}

void CommandShell(uint8_t *state)
{
	static uint8_t cl[CMAXCL];
	static uint8_t getState;
	
	CanMessage msg;
	uint8_t i;
	// char *s;
	
	if (*state == COMMANDSHELL_MODEINIT) {
		term_puts_P("\nMTSHELL>");
		getState = GET_TERMSTRING_STATE_INIT;
		*state = COMMANDSHELL_MODERUN;
	}
	
	else if (*state == COMMANDSHELL_REDRAW) {
		term_puts_P("\nMTSHELL>");
		term_puts(cl);
		*state = COMMANDSHELL_MODERUN;
	}

	else {
		GetConsoleString(&getState,cl,CMAXCL);
	}
	
	if (getState == GET_TERMSTRING_STATE_RETURN) {

		getState = GET_TERMSTRING_STATE_INIT;
		*state = COMMANDSHELL_MODEINIT;
		
		term_put_cr();
		
		if ( !strncmp_flash(cl,"help",4) ) {
			ShowHelp();
		}
	
		else if ( !strncmp_flash(cl,"exit",4) ) {
			term_puts_P("\nShell exit\n");
			*state = COMMANDSHELL_MODEEXIT;
		}

		else if ( !strncmp_flash(cl,"show",4) ) {
			if (!strncmp_flash(GetCLParmStr(cl,1),"sys",3)) {
				DumpConfigValues();
			}
			else {
				TellError(ERRUNKNOWN);
			}
		}
		
		else if ( !strncmp_flash(cl,"led",3) ) {
			if (!strncmp_flash(GetCLParmStr(cl,1),"off",2)) {
				gpio_clear_led ( LED0 | LED1 | LED2 );
			}
			else if (!strncmp_flash(GetCLParmStr(cl,1),"on",2)) {
				gpio_set_led ( LED0 | LED1 | LED2 );
			}
			else if (!strncmp_flash(GetCLParmStr(cl,1),"toggle",1)) {
				i=atoi(GetCLParmStr(cl,2));
				if (i<4) {
					i = (1<<i);
					// Debug_ByteToUart_P("toogle",i);
					gpio_toggle_led ( i );
				}
			}
		}

		else if ( !strncmp_flash(cl,"mcp",3) ) {
			if (!strncmp_flash(GetCLParmStr(cl,1),"stat",3)) {
				Debug_ByteToUart_p(PSTR("MCP-Status-Byte"),mcp2515_readStatus());
			}
			else if (!strncmp_flash(GetCLParmStr(cl,1),"error",3)) {
				Debug_ByteToUart_p(PSTR("MCP Transmit Error Count"),
					mcp2515_readRegister(MCP_TEC));
				Debug_ByteToUart_p(PSTR("MCP Receiver Error Count"),
					mcp2515_readRegister(MCP_REC));
				Debug_ByteToUart_p(PSTR("MCP Error Flag"),
					mcp2515_readRegister(MCP_EFLG));
			}
			else TellError(ERRUNKNOWN);
		}

		else if ( !strncmp_flash(cl,"can",3) ) {
			if (!strncmp_flash(GetCLParmStr(cl,1),"read",4)) {
				while ( can_readMessage(&msg) == CAN_OK ) {
					can_dumpMessage(&msg);
				}	
			}
			else if (!strncmp_flash(GetCLParmStr(cl,1),"send",3)) {
				i=atoi(GetCLParmStr(cl,3));
				if (!strncmp_flash(GetCLParmStr(cl,2),"std",3)) {
					can_testTransmit(CAN_STDID, i);
				}
				else if (!strncmp_flash(GetCLParmStr(cl,2),"ext",3)) {
					can_testTransmit(CAN_EXTID, i);
				}
				else {
					TellError(ERRUNKNOWN);
				}
			}
			
			else if (!strncmp_flash(GetCLParmStr(cl,1),"auto",3)) {
				if (!strncmp_flash(GetCLParmStr(cl,2),"off",2)) {
					gCANAutoProcess = CANAUTOOFF;
				}
				else if (!strncmp_flash(GetCLParmStr(cl,2),"on",2)) {
					gCANAutoProcess = CANAUTOON;
				}
				else {
					TellError(ERRUNKNOWN);
				}
			}
			
			else if (!strncmp_flash(GetCLParmStr(cl,1),"timed",3)) {
				if (!strncmp_flash(GetCLParmStr(cl,2),"off",2)) {
					gTimedSend = CANAUTOOFF;
				}
				else if (!strncmp_flash(GetCLParmStr(cl,2),"on",2)) {
					gTimedSend = CANAUTOON;
				}
				else {
					TellError(ERRUNKNOWN);
				}
			}
			
			else TellError(ERRUNKNOWN);
		}
		
		else {
			TellError(ERRUNKNOWN);
		}
		
		cl[0] = CEOS;

	} // endif enter-key
}

