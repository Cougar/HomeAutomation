/*
 * Defines functs, funccs and other global definitions
 * This is the one for eqnet
 *
 */

/* TODO temporary FUNCT, NID (until global_funcdefs.h is fixed) */
#define FUNCT_SENSORS                           0x01
#define NID                                     0x05
#define FUNCC_DEFAULT                           0x00

/* <FUNCC> 10 bits, 0x3FF */
#define FUNCC_SENSORS_IR                        0x001

#define FUNCC_SENSORS_TEMPERATURE_INSIDE        0x002
#define FUNCC_SENSORS_TEMPERATURE_OUTSIDE       0x003
#define FUNCC_SENSORS_TEMPERATURE_FREEZER       0x004
#define FUNCC_SENSORS_TEMPERATURE_REFRIGERATOR  0x005

/* LCD: strings */
#define LCD_SENSOR_TEMPERATURE_INSIDE           "Inne      "
#define LCD_SENSOR_TEMPERATURE_OUTSIDE          "Ute       "
#define LCD_SENSOR_TEMPERATURE_FREEZER          "Frys      "
#define LCD_SENSOR_TEMPERATURE_REFRIGERATOR     "Kyl       "

/* LCD: layout */
#define LCD_TOP                                 0,0

#define LCD_LINE0_0                             0,0
#define LCD_LINE0_1                             4,0
#define LCD_LINE0_2                             10,0
#define LCD_LINE0_3                             14,0

#define LCD_LINE1_0                             0,1
#define LCD_LINE1_1                             4,1
#define LCD_LINE1_2                             10,1
#define LCD_LINE1_3                             14,1

#define LCD_LINE2_0                             0,2
#define LCD_LINE2_1                             4,2
#define LCD_LINE2_2                             10,2
#define LCD_LINE2_3                             14,2

#define LCD_LINE3_0                             0,3
#define LCD_LINE3_1                             4,3
#define LCD_LINE3_2                             10,3
#define LCD_LINE3_3                             14,3

/* DS18S20 serial ID */
#define DS18S20_FREEZER                         0x10A7022601080005
#define DS18S20_OUTSIDE                         0x100901E50008001E
#define DS18S20_INSIDE_1                        0x1061C125010800CC
//#define DS18S20_INSIDE_2
//#define DS18S20_REFRIGERATOR
#define DS18S20_UNDEF                           0x00

/* Node IDs */
#define NODE_SENSOR_1                           0x0F
#define NODE_LCD_1                              0x02

