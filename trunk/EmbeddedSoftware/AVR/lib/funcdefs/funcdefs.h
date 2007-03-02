/*
 * Defines functs, funccs and other global definitions
 *
 * IDENT: <CLASS><TYPE><SID> DATA: <data7>...<data0>
 *
 * <CLASS>, 4 bits = Function type.
 * <TYPE>, 17 bits = Function code.
 * <SID>, 8 bits = Sender ID.
 */


/* Definitions for masking */
#define CLASS_MASK	0x1E000000L		/* bit[28..25] */
#define TYPE_MASK	0x01FFFF00L		/* bit[24..8] */
#define SID_MASK	0x000000FFL		/* bit[7..0] */

#define CLASS_MASK_BITS	25
#define TYPE_MASK_BITS	8
#define SID_MASK_BITS	0

/**************************************************************
 * <CLASS> 4 bits 0xFL 
 *************************************************************/
#define CLASS_SNS	0x4L
#define CLASS_ACT	0x5L

/****************************************************************
 * <TYPE> 17 bits 0x1FFFFL
 ***************************************************************/

/* SNS: Sensor data */

/* <SNS_TYPE><SNS_ID>
 *
 * <SNS_TYPE>, 8 bits = Sensor type.
 * <SNS_ID>, 9 bits = Sensor ID. Ex outdoor ds18s20, NodeRelay tc1047.
 */
/* SNS masking */
#define SNS_TYPE_MASK	0x1FE0000L	/* bit[24..17] */
#define SNS_ID_MASK		0x001FF00L	/* bit[16..8] */

#define SNS_TYPE_BITS	17
#define SNS_ID_BITS		8

/* <SNS_TYPE> 8 bits 0xFFL */
#define SNS_TEMP_DS18S20		0x08L
#define SNS_TEMP_TC1047			0x09L
#define SNS_IR_RECEIVE			0x0AL
#define SNS_ACT_STATUS_RELAY	0x0BL
#define SNS_ACT_STATUS_SERVO	0x0CL
#define SNS_ACT_STATUS_DIMMER	0x0DL
#define SNS_ACT_BUTTON			0x0EL

/* <SNS_ID> 9 bits 0x1FFL */
//idn för alla sensorer
//eqlazers idn:
#define SNS_DS18S20_OUTSIDE		0x001L
#define SNS_DS18S20_FREEZER		0x002L
#define SNS_DS18S20_INSIDE1		0x003L
#define SNS_DS18S20_INSIDE2		0x004L

/* ACT: Actuator data */

/* <ACT_TYPE><GROUP><NODE>
 *
 * <ACT_TYPE>, 5 bits = Actuator type, relay, dimmer etc.
 * <GROUP>, 4 bits = To adress larger groups of actuators.
 * <NODE>, 8 bits = Specific node.
 */
/* ACT masking */
#define ACT_TYPE_MASK	0x1F000L	/* bit[16..12] */
#define ACT_GROUP_MASK	0x00F00L	/* bit[11..8] */
#define ACT_NODE_MASK	0x000FFL	/* bit[7..0] */

#define ACT_TYPE_BITS	12
#define ACT_GROUP_BITS	8
#define ACT_NODE_BITS	0

/* <ACT_TYPE> 5 bits 0x1FL */
#define ACT_TYPE_RELAY		0x01L
#define ACT_TYPE_DIMMER		0x02L
#define ACT_TYPE_SERVO		0x03L
#define ACT_TYPE_BLINDS		ACT_TYPE_SERVO
#define ACT_TYPE_LCD		0x04L
#define ACT_TYPE_IRTRANS	0x05L

/* ACT LCD 12 bits <ACTION><SIZE> 0xFFFL
 *
 * <ACTION>, 4 bits = Type och action.
 * <SIZE>, 8 bits = Size of display.
 */
#define LCD_ACTION_MASK 0xF00L /* bit[12..8] */
#define LCD_SIZE_MASK 0x0FFL /* bit[7..0] */

#define LCD_ACTION_BITS 8
#define LCD_SIZE_BITS 0


/* <ACTION> 4 bits 0xFL */

#define LCD_ACTION_CLR			0x0L
#define LCD_ACTION_CURS			0x1L
#define LCD_ACTION_TEXT			0x2L
#define LCD_ACTION_GET_SIZE		0x3L
#define LCD_ACTION_SET_CONT		0x4L
#define LCD_ACTION_SET_BLIGHT	0x5L
#define LCD_ACTION_GET_CONT		0x6L
#define LCD_ACTION_GET_BLIGHT	0x7L

#define LCD_ACTION_SEND_CONT	0x8L
#define LCD_ACTION_SEND_BLIGHT	0x9L



/* <SIZE> 8 bits 0xFFL */
#define LCD_SIZE_ALL	0x00L
#define LCD_SIZE_16x2	0x02L
#define LCD_SIZE_20x4	0x10L

/*************************************************************
 * <SID> 8 bits 0xFFL 
 ************************************************************/


////////////////////////////////////////////////////////////




/* LCD: strings */
#define LCD_SENSOR_TEMPERATURE_INSIDE           "Inne      "
#define LCD_SENSOR_TEMPERATURE_OUTSIDE          "Ute       "
#define LCD_SENSOR_TEMPERATURE_FREEZER          "Frys      "
#define LCD_SENSOR_TEMPERATURE_REFRIGERATOR     "Kyl       "

/* DS18S20 serial ID */
#define DS18S20_FREEZER                         0x10A7022601080005
#define DS18S20_OUTSIDE                         0x100901E50008001E
#define DS18S20_INSIDE_1                        0x1061C125010800CC
//#define DS18S20_INSIDE_2
//#define DS18S20_REFRIGERATOR
#define DS18S20_UNDEF                           0x00


