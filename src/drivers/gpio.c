#include "gpio.h"
#include "esp32_defs.h"
#include "uart.h"

/* Initialize GPIO subsystem */
void gpio_init(void)
{
    /* Nothing special needed for initialization */
    uart_puts("[GPIO] GPIO driver initialized\n");
}

/* Configure GPIO pin mode */
void gpio_set_mode(uint32_t gpio_num, gpio_mode_t mode)
{
    if (gpio_num >= 40) {
        uart_puts("[GPIO] ERROR: Invalid GPIO number\n");
        return;
    }

    /* Configure pin function in IO MUX */
    uint32_t io_mux_reg = DR_REG_IO_MUX_BASE + 0x10 + (gpio_num * 4);

    switch (mode) {
        case GPIO_MODE_OUTPUT:
            /* Set as output */
            REG_SET_BIT(GPIO_ENABLE_W1TS_REG, gpio_num);
            /* Configure IO MUX for GPIO function */
            REG_WRITE(io_mux_reg,
                     (2 << MCU_SEL_S) |  /* Function 2 = GPIO */
                     (2 << FUN_DRV_S) |  /* Drive strength */
                     FUN_IE);            /* Input enable */
            /* Set output function */
            REG_WRITE(GPIO_FUNC_OUT_SEL_CFG_REG(gpio_num), 256);  /* Simple GPIO out */
            break;

        case GPIO_MODE_INPUT:
            /* Set as input */
            REG_SET_BIT(GPIO_ENABLE_W1TC_REG, gpio_num);
            /* Configure IO MUX for GPIO function */
            REG_WRITE(io_mux_reg,
                     (2 << MCU_SEL_S) |  /* Function 2 = GPIO */
                     FUN_IE);            /* Input enable */
            break;

        case GPIO_MODE_INPUT_PULLUP:
            /* Set as input with pull-up */
            REG_SET_BIT(GPIO_ENABLE_W1TC_REG, gpio_num);
            /* Configure IO MUX with pull-up */
            REG_WRITE(io_mux_reg,
                     (2 << MCU_SEL_S) |  /* Function 2 = GPIO */
                     FUN_IE |            /* Input enable */
                     FUN_WPU);           /* Weak pull-up */
            break;

        case GPIO_MODE_INPUT_PULLDOWN:
            /* Set as input with pull-down */
            REG_SET_BIT(GPIO_ENABLE_W1TC_REG, gpio_num);
            /* Configure IO MUX with pull-down */
            REG_WRITE(io_mux_reg,
                     (2 << MCU_SEL_S) |  /* Function 2 = GPIO */
                     FUN_IE |            /* Input enable */
                     FUN_WPD);           /* Weak pull-down */
            break;
    }
}

/* Set GPIO output level */
void gpio_set_level(uint32_t gpio_num, gpio_level_t level)
{
    if (gpio_num >= 40) {
        return;
    }

    if (level == GPIO_LEVEL_HIGH) {
        /* Set bit (output high) */
        REG_WRITE(GPIO_OUT_W1TS_REG, BIT(gpio_num));
    } else {
        /* Clear bit (output low) */
        REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_num));
    }
}

/* Get GPIO input level */
gpio_level_t gpio_get_level(uint32_t gpio_num)
{
    if (gpio_num >= 40) {
        return GPIO_LEVEL_LOW;
    }

    uint32_t val = REG_READ(GPIO_IN_REG);
    return (val & BIT(gpio_num)) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
}

/* Toggle GPIO output level */
void gpio_toggle(uint32_t gpio_num)
{
    if (gpio_num >= 40) {
        return;
    }

    uint32_t current_level = (REG_READ(GPIO_OUT_REG) & BIT(gpio_num)) ? 1 : 0;
    gpio_set_level(gpio_num, current_level ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}
