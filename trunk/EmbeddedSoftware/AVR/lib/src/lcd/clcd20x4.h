/*
 * Header file for printing predefined pages to 20x4 character LCD.
 *
 * Author: Erik Larsson
 * Date: 2006-12-29
 *
 */

/*---------------------------------------------------------------
 * Includes
 * ------------------------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*--------------------------------------------------------------
 * Defines
 * -----------------------------------------------------------*/
#define MAX_LENGTH      21
#define LINES           4
#define CHARACTERS      20
#define EDIT_MODE_SYMBOL "&&"

#define MAIN_VIEW       0
#define TEMPSENS_VIEW   1
#define RELAYS_VIEW     2
#define DIMMERS_VIEW    3

#define DATA1           3,1
#define DATA2           13,1
#define DATA3           3,2
#define DATA4           13,2

/*---------------------------------------------------------------
 * Functions
 * ------------------------------------------------------------*/
void printLCDview(uint8_t view);
void printLCDviewData(uint8_t view, uint8_t *data);

/*---------------------------------------------------------------
 * Views
 * <Line 1> <Line 2> <Line 3> <Left> <Right>
 *--------------------------------------------------------------*/
char viewStrings[][ MAX_LENGTH ] = {
    /* Main view */
    "HomeAutomation", "", "Welcome", "NA", "Temp",
    /* Temperature sensors */
    "Temperature Sensors", "1: NA     2: NA", "3: NA     4: NA", "Main", "Relay",
    /* Relay status */
    "Relay Status", "1: NA     2: NA", "3: NA    4: NA", "Temp", "Dimmer",
    /* Dimmer status */
    "Dimmer Status", "1: NA     2: NA", "3: NA    4: NA", "Relay", "NA"
};

