#include "task.h"
#include "kernel.h"
#include "uart.h"
#include "gpio.h"
#include "esp32_defs.h"

/* LED GPIO pin - most ESP32 boards have LED on GPIO2 */
#define LED_GPIO  GPIO_NUM_2

/* LED blink counter */
static uint32_t blink_count = 0;

/* LED Blink Task */
void led_blink_task(void *arg)
{
    uart_puts("[LED_TASK] LED blink task started\n");

    /* Configure LED pin as output */
    gpio_set_mode(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, GPIO_LEVEL_LOW);

    uart_printf("[LED_TASK] LED configured on GPIO%d\n", LED_GPIO);

    while (1) {
        /* Turn LED on */
        gpio_set_level(LED_GPIO, GPIO_LEVEL_HIGH);
        uart_printf("[LED_TASK] LED ON (blink #%d)\n", ++blink_count);

        /* Delay ~500ms */
        for (volatile int i = 0; i < 50; i++) {
            delay_ms(10);
            task_yield();  /* Allow other tasks to run */
        }

        /* Turn LED off */
        gpio_set_level(LED_GPIO, GPIO_LEVEL_LOW);
        uart_puts("[LED_TASK] LED OFF\n");

        /* Delay ~500ms */
        for (volatile int i = 0; i < 50; i++) {
            delay_ms(10);
            task_yield();  /* Allow other tasks to run */
        }
    }
}

/* UART Status Task */
void uart_status_task(void *arg)
{
    uart_puts("[UART_TASK] UART status task started\n");

    uint32_t counter = 0;

    while (1) {
        /* Print status message */
        uart_printf("[UART_TASK] Status update #%d - System running OK\n", ++counter);

        /* Delay ~2 seconds */
        for (volatile int i = 0; i < 200; i++) {
            delay_ms(10);
            task_yield();  /* Allow other tasks to run */
        }

        /* Print heap statistics periodically */
        if (counter % 5 == 0) {
            uint32_t total, used, free;
            extern void heap_stats(uint32_t *total, uint32_t *used, uint32_t *free);
            heap_stats(&total, &used, &free);
            uart_printf("[UART_TASK] Heap: %d bytes used, %d bytes free\n", used, free);
        }
    }
}

/* Compute Task - does some work */
void compute_task(void *arg)
{
    uart_puts("[COMPUTE_TASK] Compute task started\n");

    uint32_t fibonacci = 0;
    uint32_t prev1 = 1, prev2 = 0;

    while (1) {
        /* Calculate next Fibonacci number */
        fibonacci = prev1 + prev2;
        prev2 = prev1;
        prev1 = fibonacci;

        /* Reset if too large */
        if (fibonacci > 1000000) {
            fibonacci = 0;
            prev1 = 1;
            prev2 = 0;
            uart_puts("[COMPUTE_TASK] Fibonacci sequence reset\n");
        }

        /* Print every 10th number */
        static uint32_t fib_count = 0;
        if (++fib_count % 10 == 0) {
            uart_printf("[COMPUTE_TASK] Fibonacci: %d\n", fibonacci);
        }

        /* Yield frequently */
        task_yield();
    }
}

/* Initialize demo tasks */
void demo_init_tasks(void)
{
    /* Create LED blink task */
    task_t *led_task = task_create("led_blink", led_blink_task, NULL, TASK_STACK_SIZE);
    if (!led_task) {
        uart_puts("[DEMO] ERROR: Failed to create LED task\n");
    }

    /* Create UART status task */
    task_t *uart_task = task_create("uart_status", uart_status_task, NULL, TASK_STACK_SIZE);
    if (!uart_task) {
        uart_puts("[DEMO] ERROR: Failed to create UART task\n");
    }

    /* Create compute task */
    task_t *compute = task_create("compute", compute_task, NULL, TASK_STACK_SIZE);
    if (!compute) {
        uart_puts("[DEMO] ERROR: Failed to create compute task\n");
    }

    uart_puts("[DEMO] All demo tasks created successfully\n");
}
