/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : mcp2515_bittime.h
 * Version : 0.9
 * 
 * Summary : MCP2515 CAN-timing values. Calculated with 
 *           a spreadsheet-based "Preprocessor" with information
 *           from the MCP2515 and AT90CAN128 datasheets.
 *
 *****************************************************************************/

#include <drivers/can/mcp2515/mcp2515_defs.h>
#include <drivers/can/mcp2515/mcp2515_cfg.h>

/*-----------------------*/
/* global settings       */
/*-----------------------*/

/* CFG3 */

// Start of Frame SOF (CLKOUT)
// either SOF_ENABLE or SOF_DISABLE
#define MCP_GENERAL_SOF (SOF_DISABLE)

// Wake up Filter (WAKFIL)
// either WAKFIL_ENABLE	or WAKFIL_DISABLE
#define MCP_GENERAL_WAKFIL (WAKFIL_DISABLE)


/*
 * The following bitrate settings use values close or equal to SJW = 1, SP = 75%.
 * Any deviations from these values are commented below.
 * 
 * All values calculated with:
 * 		http://www.kvaser.com/can/protocol/timing_calc.htm
 */
#if MCP_CLOCK_FREQ_MHZ == 24
	#define MCP_125KBPS_CFG1	0x05
	#define MCP_125KBPS_CFG2	0xAC
	#define MCP_125KBPS_CFG3	0x03
	#define MCP_250KBPS_CFG1	0x02
	#define MCP_250KBPS_CFG2	0xAC
	#define MCP_250KBPS_CFG3	0x03
	#define MCP_500KBPS_CFG1	0x01
	#define MCP_500KBPS_CFG2	0x9B
	#define MCP_500KBPS_CFG3	0x02
	#define MCP_1000KBPS_CFG1	0x00
	#define MCP_1000KBPS_CFG2	0x9B
	#define MCP_1000KBPS_CFG3	0x02
#elif MCP_CLOCK_FREQ_MHZ == 20
	#define MCP_125KBPS_CFG1	0x03
	#define MCP_125KBPS_CFG2	0xB6
	#define MCP_125KBPS_CFG3	0x04
	#define MCP_250KBPS_CFG1	0x01
	#define MCP_250KBPS_CFG2	0xB6
	#define MCP_250KBPS_CFG3	0x04
	#define MCP_500KBPS_CFG1	0x00
	#define MCP_500KBPS_CFG2	0xB6
	#define MCP_500KBPS_CFG3	0x04
	#define MCP_1000KBPS_CFG1	0x00	/* SP=70% */
	#define MCP_1000KBPS_CFG2	0x92
	#define MCP_1000KBPS_CFG3	0x02
#elif MCP_CLOCK_FREQ_MHZ == 16
	#define MCP_125KBPS_CFG1	0x03
	#define MCP_125KBPS_CFG2	0xAC
	#define MCP_125KBPS_CFG3	0x03
	#define MCP_250KBPS_CFG1	0x01
	#define MCP_250KBPS_CFG2	0xAC
	#define MCP_250KBPS_CFG3	0x03
	#define MCP_500KBPS_CFG1	0x00
	#define MCP_500KBPS_CFG2	0xAC
	#define MCP_500KBPS_CFG3	0x03
	#define MCP_1000KBPS_CFG1	0x00
	#define MCP_1000KBPS_CFG2	0x91
	#define MCP_1000KBPS_CFG3	0x01
#elif MCP_CLOCK_FREQ_MHZ == 12
	#define MCP_125KBPS_CFG1	0x02
	#define MCP_125KBPS_CFG2	0xAC
	#define MCP_125KBPS_CFG3	0x03
	#define MCP_250KBPS_CFG1	0x01
	#define MCP_250KBPS_CFG2	0x9B
	#define MCP_250KBPS_CFG3	0x02
	#define MCP_500KBPS_CFG1	0x00
	#define MCP_500KBPS_CFG2	0x9B
	#define MCP_500KBPS_CFG3	0x02
	#define MCP_1000KBPS_CFG1	0x00	/* SP=66.7% */
	#define MCP_1000KBPS_CFG2	0x88
	#define MCP_1000KBPS_CFG3	0x01
#elif MCP_CLOCK_FREQ_MHZ == 10
	#define MCP_125KBPS_CFG1	0x01
	#define MCP_125KBPS_CFG2	0xB6
	#define MCP_125KBPS_CFG3	0x04
	#define MCP_250KBPS_CFG1	0x00
	#define MCP_250KBPS_CFG2	0xB6
	#define MCP_250KBPS_CFG3	0x04
	#define MCP_500KBPS_CFG1	0x00	/* SP=70% */
	#define MCP_500KBPS_CFG2	0x92
	#define MCP_500KBPS_CFG3	0x02
	#define MCP_1000KBPS_CFG1	0x00	/* SP=60% */
	#define MCP_1000KBPS_CFG2	0x80
	#define MCP_1000KBPS_CFG3	0x01
#elif MCP_CLOCK_FREQ_MHZ == 8
	#define MCP_125KBPS_CFG1	0x01
	#define MCP_125KBPS_CFG2	0xAC
	#define MCP_125KBPS_CFG3	0x03
	#define MCP_250KBPS_CFG1	0x00
	#define MCP_250KBPS_CFG2	0xAC
	#define MCP_250KBPS_CFG3	0x03
	#define MCP_500KBPS_CFG1	0x00
	#define MCP_500KBPS_CFG2	0x91
	#define MCP_500KBPS_CFG3	0x01
	/* 1000kBPS not possible with 8MHz clock! */	
#else
	#error UNSUPPORTED MCP CLOCK FREQUENCY!
#endif

#if MCP_CAN_BITRATE_KBPS == 125
	#define MCP_BITRATE_CFG1	MCP_125KBPS_CFG1
	#define MCP_BITRATE_CFG2	MCP_125KBPS_CFG2
	#define MCP_BITRATE_CFG3	MCP_125KBPS_CFG3

#elif MCP_CAN_BITRATE_KBPS == 250
	#define MCP_BITRATE_CFG1	MCP_250KBPS_CFG1
	#define MCP_BITRATE_CFG2	MCP_250KBPS_CFG2
	#define MCP_BITRATE_CFG3	MCP_250KBPS_CFG3

#elif MCP_CAN_BITRATE_KPBS == 500
	#define MCP_BITRATE_CFG1	MCP_500KBPS_CFG1
	#define MCP_BITRATE_CFG2	MCP_500KBPS_CFG2
	#define MCP_BITRATE_CFG3	MCP_500KBPS_CFG3

#elif MCP_CAN_BITRATE_KBPS == 1000
	#if MCP_CLOCK_FREQ_MHZ > 8
		#define MCP_BITRATE_CFG1	MCP_1000KBPS_CFG1
		#define MCP_BITRATE_CFG2	MCP_1000KBPS_CFG2
		#define MCP_BITRATE_CFG3	MCP_1000KBPS_CFG3
	#else
		#error CAN bitrate of 1000 kbps only possible with clock freq > 8 MHz.
	#endif

#else
	#error MCP_CAN_BITRATE_KBPS not set properly.
#endif
