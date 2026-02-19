#ifndef GPIO_H
#define GPIO_H

#include "types.h"

/* GPIO modes */
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_INPUT_PULLDOWN
} gpio_mode_t;

/* GPIO levels */
typedef enum {
    GPIO_LEVEL_LOW = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

/* Initialize GPIO subsystem */
void gpio_init(void);

/* Configure GPIO pin mode */
void gpio_set_mode(uint32_t gpio_num, gpio_mode_t mode);

/* Set GPIO output level */
void gpio_set_level(uint32_t gpio_num, gpio_level_t level);

/* Get GPIO input level */
gpio_level_t gpio_get_level(uint32_t gpio_num);

/* Toggle GPIO output level */
void gpio_toggle(uint32_t gpio_num);

#endif /* GPIO_H */
