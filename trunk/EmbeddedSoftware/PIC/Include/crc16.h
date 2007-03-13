#ifndef CRC16_H
#define CRC16_H

#define uint16 unsigned int
#define uint8 unsigned char

void update_crc(uint16 *p_crc, uint8 data);
uint16 calc_crc(uint8 * p_data, uint16 n_bytes);

#endif
