#include <p18cxxx.h>
#include <crc16.h>

// *****************************************************************************
// Update the CRC for transmitted and received data using
// the CCITT 16bit algorithm (X^16 + X^12 + X^5 + X^0)
//
// Adapted by R Andrag from an 
// asm routine by John C. Wren (google to find the original code)
// roland.andrag at gmail.com
//
// To check your crc implementation use:
// http://www.zorc.breitbandkatze.de/crc.html
// Web calculator by Sven Reifegerste
// CRC order = 16
// CRC polynomial = 1021 hex
// Initial value = FFFF hex
// direct CRC
// Final XOR value = 0
// Don't reverse data bytes or CRC result
// calculator allows you to enter hex bytes by prefixing %
// append 00 00 to your mesage when you calulate the crc
// i.e. crc should work out to zero on: message:00:00:crc_h:crc_l
// e.g. %01%02%00%00 gives a crc of 9c14
//      %01%02%00%00%9c%14 gives a crc of 0000
// %01%02%9c%14 gives a crc of 9327 hex
// *****************************************************************************
void update_crc(uint16 *p_crc, uint8 data) {
    *p_crc  = (*p_crc >> 8) | (*p_crc << 8);
    *p_crc ^= data;
    *p_crc ^= (*p_crc & 0xff) >> 4;
    *p_crc ^= *p_crc << 12;
    *p_crc ^= (*p_crc & 0xff) << 5;
}


uint16 calc_crc(uint8 * p_data, uint16 n_bytes) {
    uint16 crc = 0xFFFF;
	
    for (; n_bytes > 0; n_bytes--) {
        update_crc(&crc, *(p_data++));
    }
	
    return crc;
}
