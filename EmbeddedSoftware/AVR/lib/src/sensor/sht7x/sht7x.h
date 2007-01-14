#ifndef SHT7X_H_
#define SHT7X_H_

/*-----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*-----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define HUMIDITY_DDR DDRB
#define HUMIDITY_PIN PINB
#define HUMIDITY_PORT PORTB
#define HUMIDITY_DATA (1<<PB0) //0x10
#define HUMIDITY_SCK (1<<PB1) //0x20
//adr command r/w
#define HUMIDITY_STATUS_REG_W 0x06 //000 0011 0
#define HUMIDITY_STATUS_REG_R 0x07 //000 0011 1
#define HUMIDITY_MEASURE_TEMP 0x03 //000 0001 1
#define HUMIDITY_MEASURE_HUMI 0x05 //000 0010 1
#define HUMIDITY_RESET 0x1e //000 1111 0

#define TRUE 1
#define FALSE !TRUE

#define ACK 1
#define noACK !ACK

uint8_t HumiError;
uint16_t Humidity, Temperature, Hum, Temp;

/*-----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
void HumidityInit(void);
void HumidityDelay(void);
void HumidityWriteByte(unsigned char value);
unsigned char HumidityReadByte(unsigned char ack);
void HumidityTransStart(void);
void HumidityConnectionReset(void);
void HumiditySoftReset(void);
unsigned char HumidityReadStatus(void);
void HumidityWriteStatus(unsigned char value);
unsigned int HumidityMeasure(unsigned char mode);
void HumidityCalc(float *p_humidity ,float *p_temperature);
void HumidityGet(float *p_humidity ,float *p_temperature);
void HumidityUpdate(void); 

#endif
