
#include <avr/io.h>

#include "gpio.h"

void gpio_init(void)
{
    LEDPORT |= ( LED0 | LED1 | LED2 ); // all off
	LEDDDR  |= ( LED0 | LED1 | LED2 ); // set as output
}
	
void gpio_set_led(const uint8_t bitmask)
{
	LEDPORT &= ~(bitmask);
}

void gpio_clear_led(const uint8_t bitmask)
{
	LEDPORT |= (bitmask);
}

void gpio_toggle_led(const uint8_t bitmask)
{
	LEDPORT ^= (bitmask);
}
