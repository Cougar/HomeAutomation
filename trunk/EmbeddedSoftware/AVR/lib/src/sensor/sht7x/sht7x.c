/*
 * Humidity & Temperature sensor SHT7x. (71/75)
 *
 * Sensor: http://www.sensirion.com/en/02_sensors/03_humidity/00_humidity_temperature_sensor/06_humidity_sensor_sht75.htm
 * Origin of code: http://www.avrfreaks.net/index.php?module=PNphpBB2&file=viewtopic&t=31562&highlight=
 *
 * Copied from avrfreaks and rewritten by Erik Larsson.
 * Date: 2007-01-14
 *
 */
#include <avr/io.h>
#include <sht7x.h>

/*
Name: void HumidityDelay(void)

Description: Delay for SCLK

Input: none

Output: none


Misc:
******************************************************************************/
void HumidityDelay(void)
{
int i;

for (i=0;i<40;i++); /* Probably not that important because BIOS will take som time to start */
}

/******************************************************************************

Name: void HumidityInit(void)

Description: Initialise port and constant used by
the Humidity & Temp hardware


Input: none

Output: none

Misc:
******************************************************************************/
void HumidityInit(void)
{
HUMIDITY_DDR |= HUMIDITY_DATA + HUMIDITY_SCK;
HUMIDITY_PORT |= HUMIDITY_DATA + HUMIDITY_SCK;
HumiditySoftReset();
}

/******************************************************************************

Name: unsigned char HumidityWriteByte(unsigned char value)

Description: writes a byte on the Sensibus and
checks the acknowledge

Input: unsigned char value -> byte to write

Output: none


Misc: If the device don't ACK set HumiError to TRUE;
******************************************************************************/
void HumidityWriteByte(unsigned char value)
{
unsigned char i;

HUMIDITY_DDR |= HUMIDITY_DATA; //DATA-line in output

for (i=0x80;i>0;i/=2) //shift bit for masking
{
HumidityDelay();
if (i & value) HUMIDITY_PORT |= HUMIDITY_DATA;
else HUMIDITY_PORT &= ~HUMIDITY_DATA;
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK;
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK;
}

HUMIDITY_PORT |= HUMIDITY_DATA; //release DATA-line
HUMIDITY_DDR &= ~HUMIDITY_DATA; //DATA-line in input
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK; //clk #9 for ack
HumidityDelay();
if (HUMIDITY_PIN & HUMIDITY_DATA) HumiError = TRUE;
HUMIDITY_PORT &= ~HUMIDITY_SCK;
HumidityDelay();
}

/******************************************************************************

Name: unsigned char HumidityReadByte(unsigned char ack)

Description: reads a byte form the Sensibus and gives
an acknowledge in case of "ack=1"

Input: unsigned char ack -> ack status

Output: unsigned char value readed

Misc:
******************************************************************************/
unsigned char HumidityReadByte(unsigned char ack)
{
unsigned char i,val=0;

HUMIDITY_PORT |= HUMIDITY_DATA; //release DATA-line
HUMIDITY_DDR &= ~HUMIDITY_DATA; //DATA-line in input

for (i=0x80;i>0;i/=2) //shift bit for masking
{
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK; //clk for SENSI-BUS
HumidityDelay();
if (HUMIDITY_PIN & HUMIDITY_DATA) val=(val | i); //read bit
HUMIDITY_PORT &= ~HUMIDITY_SCK;
}

if (ack == 1)
{
HUMIDITY_DDR |= HUMIDITY_DATA; //DATA-line in output
HUMIDITY_PORT &= ~HUMIDITY_DATA; //"ack==1" pull down DATA-Line
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK;
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK;
}
else
{
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK;
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK;
}
return val;
}

/******************************************************************************

Name: unsigned char HumidityTransStart(void)

Description: Generates a transmission start

Input: none

Output: none

Misc:
_____ ________
DATA: |_______|
___ ___
SCK : ___| |___| |______
******************************************************************************/
void HumidityTransStart(void)
{
HUMIDITY_DDR |= HUMIDITY_DATA; //DATA-line in output

HUMIDITY_PORT |= HUMIDITY_DATA; //DATA=1
HUMIDITY_PORT &= ~HUMIDITY_SCK; //SCK=0
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK; //SCK=1
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_DATA; //DATA=0
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK; //SCK=0
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK; //SCK=1
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_DATA; //DATA=1
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK; //SCK=0
HumidityDelay();
}

/******************************************************************************

Name: void HumidityConnectionReset(void)

Description: communication Reset: DATA-line=1 and at
least 9 SCK cycles followed by transstart

Input: none

Output: none

Misc:
_____________________________________________________ ________
DATA: |_______|
_ _ _ _ _ _ _ _ _ ___ ___
SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______| |___| |______
******************************************************************************/
void HumidityConnectionReset(void)
{
unsigned char i;

HUMIDITY_DDR |= HUMIDITY_DATA; //DATA-line in output
HUMIDITY_PORT |= HUMIDITY_DATA; //DATA=1
HUMIDITY_PORT &= ~HUMIDITY_SCK; //SCK=0

for (i=0;i<9;i++) //9 SCK cycles
{
HumidityDelay();
HUMIDITY_PORT |= HUMIDITY_SCK; //SCK=1
HumidityDelay();
HUMIDITY_PORT &= ~HUMIDITY_SCK; //SCK=0
}
HumidityTransStart(); //transmission start
HumiError = FALSE;
}

/******************************************************************************

Name: void HumiditySoftReset(void)

Description: Resets the sensor by a softReset

Input: none

Output: none

Misc:

******************************************************************************/
void HumiditySoftReset(void)
{
int i;
HumidityConnectionReset(); //Reset communication
HumidityWriteByte(HUMIDITY_RESET); //send Reset-command to sensor
for (i=0;i<100;i++) HumidityDelay(); // 11ms delay
}

/******************************************************************************

Name: unsigned char HumidityReadStatus(void)

Description: reads the status register with checksum (8-bit)

Input: none

Output: unsigned char status byte

Misc:

******************************************************************************/
unsigned char HumidityReadStatus(void)
{
unsigned char value,tmp;

HumidityTransStart(); //transmission start
HumidityWriteByte(HUMIDITY_STATUS_REG_R); //send command to sensor
value = HumidityReadByte(ACK); //read status register (8-bit)
tmp=HumidityReadByte(noACK); //dummy read checksum (8-bit)
return value;
}

/******************************************************************************

Name: void HumidityWriteStatus(unsigned char value)

Description: writes the status register with checksum (8-bit)

Input: unsigned char value to write in the status register

Output: none

Misc:

******************************************************************************/
void HumidityWriteStatus(unsigned char value)
{
HumidityTransStart(); //transmission start
HumidityWriteByte(HUMIDITY_STATUS_REG_W); //send command to sensor
HumidityWriteByte(value); //send value of status register
}

/******************************************************************************

Name: unsigned int HumidityMeasure(unsigned char mode)

Description: makes a measurement (humidity/temperature)

Input: mode -> HUMI or TEMP

Output: unsinged int Value mesured

Misc:

******************************************************************************/
unsigned int HumidityMeasure(unsigned char mode)
{
unsigned char msb,lsb,tmp;
//int i,j;
uint16_t i,j;

HumidityTransStart(); //transmission start
switch (mode) //send command to sensor
{
case HUMIDITY_MEASURE_TEMP : HumidityWriteByte(HUMIDITY_MEASURE_TEMP);
break;
case HUMIDITY_MEASURE_HUMI : HumidityWriteByte(HUMIDITY_MEASURE_HUMI);
break;
default : break;
}

HUMIDITY_PORT |= HUMIDITY_DATA; //release DATA-line
HUMIDITY_DDR &= ~HUMIDITY_DATA; //DATA-line in input

for (i=0;i<=65530;i++)
{
for (j=0;j<10;j++); /* Maybe it works */
if((HUMIDITY_PIN & HUMIDITY_DATA) == 0) break; //wait until sensor
} //has finished the measure
if(i > 65530)
{
HumiError=1; // or timeout (~2 sec.) is reached
return 0xffff;
}

msb =HumidityReadByte(ACK); //read the first byte (MSB)
lsb=HumidityReadByte(ACK); //read the second byte (LSB)
tmp =HumidityReadByte(noACK); //dummy read checksum
return (msb<<8)+lsb;
}

/******************************************************************************

Name: void HumidityCalc(float *p_humidity ,float *p_temperature)

Description: calculates temperature [°C] and humidity [%RH]

Input: humi [Ticks] (12 bit)
temp [Ticks] (14 bit)

Output: humi [%RH]
temp [°C]

Misc:

******************************************************************************/
void HumidityCalc(float *p_humidity ,float *p_temperature)
{
float C1=-4.0; // for 12 Bit
float C2=+0.0405; // for 12 Bit
float C3=-0.0000028; // for 12 Bit
float T1=+0.01; // for 14 Bit @ 3V
float T2=+0.00008; // for 14 Bit @ 3V

float rh=*p_humidity; // rh: Humidity [Ticks] 12 Bit
float t=*p_temperature; // t: Temperature [Ticks] 14 Bit
float rh_lin; // rh_lin: Humidity linear
float rh_true; // rh_true: Temperature compensated humidity
float t_C; // t_C : Temperature [°C]

t_C=t*0.01 - 39.6; //calc. temperature from ticks to [°C]
rh_lin=C3*rh*rh + C2*rh + C1; //calc. humidity from ticks to [%RH]
rh_true=(t_C-25)*(T1+T2*rh)+rh_lin; //calc. temp compensated humidity [%RH]
if(rh_true>100)rh_true=100; //cut if the value is outside of
if(rh_true<0.1)rh_true=0.1; //the physical possible range

*p_temperature=t_C; //return temperature [°C]
*p_humidity=rh_true; //return humidity[%RH]
}

/******************************************************************************

Name: void HumidityGet(float *p_humidity ,float *p_temperature)

Description: measure humidity [ticks](12 bit) and temperature [ticks](14 bit)

Input: humi [%RH]
temp [°C]

Output: humi [%RH]
temp [°C]

Misc:

******************************************************************************/
void HumidityGet(float *p_humidity ,float *p_temperature)
{
unsigned int humi_val,temp_val;

humi_val = HumidityMeasure(HUMIDITY_MEASURE_HUMI); //measure humidity
temp_val = HumidityMeasure(HUMIDITY_MEASURE_TEMP); //measure temperature

if(HumiError == TRUE) HumidityConnectionReset(); //in case of an error: Reset
else
{
*p_humidity=(float)humi_val; //converts integer to float
*p_temperature=(float)temp_val; //converts integer to float
HumidityCalc(p_humidity,p_temperature); //calculate humidity, temperature
}
}

/******************************************************************************

Name: void HumidityUpdate(void)

Description:

Input:

Output:
Misc:

******************************************************************************/
void HumidityUpdate(void)
{
if (HumiError == TRUE) HumidityInit();
HumidityGet(&Humidity,&Temperature);
Hum = Humidity * 10;
Temp = Temperature * 10;
}
