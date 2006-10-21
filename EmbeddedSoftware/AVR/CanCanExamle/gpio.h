/* IO defintions HDI */

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>

#define LEDPORT (PORTC)
#define LEDDDR  (DDRC)
#define LED0 (1<<PC0)
#define LED1 (1<<PC1)
#define LED2 (1<<PC0)

void gpio_init(void);
void gpio_set_led(const uint8_t bitmask);
void gpio_clear_led(const uint8_t bitmask);
void gpio_toggle_led(const uint8_t bitmask);

#endif

