#include "kernel.h"
#include "task.h"
#include "heap.h"
#include "uart.h"
#include "gpio.h"

/* Forward declarations for demo tasks */
extern void demo_init_tasks(void);

/* Idle task - runs when no other tasks are ready */
void idle_task(void *arg)
{
    uart_puts("[IDLE] Idle task started\n");

    while (1) {
        /* Just yield to other tasks */
        task_yield();
    }
}

/* Main kernel entry point */
void kernel_main(void)
{
    uart_puts("\n[KERNEL] Kernel initialization started\n");

    /* Initialize subsystems */
    uart_puts("[KERNEL] Initializing heap...\n");
    heap_init();

    uart_puts("[KERNEL] Initializing task system...\n");
    task_init();

    uart_puts("[KERNEL] Initializing scheduler...\n");
    scheduler_init();

    uart_puts("[KERNEL] Initializing GPIO...\n");
    gpio_init();

    /* Print heap statistics */
    uint32_t total, used, free;
    heap_stats(&total, &used, &free);
    uart_printf("[KERNEL] Heap: %d bytes total, %d used, %d free\n", total, used, free);

    /* Create idle task */
    uart_puts("[KERNEL] Creating idle task...\n");
    task_t *idle = task_create("idle", idle_task, NULL, TASK_STACK_SIZE);
    if (!idle) {
        uart_puts("[KERNEL] ERROR: Failed to create idle task\n");
        while(1);
    }

    /* Create demo tasks */
    uart_puts("[KERNEL] Creating demo tasks...\n");
    demo_init_tasks();

    /* Print final heap statistics */
    heap_stats(&total, &used, &free);
    uart_printf("[KERNEL] Heap after task creation: %d bytes used, %d free\n", used, free);

    /* Start the scheduler (never returns) */
    uart_puts("[KERNEL] Starting scheduler...\n");
    uart_puts("==============================\n\n");

    scheduler_start();

    /* Should never reach here */
    uart_puts("[KERNEL] ERROR: Scheduler returned!\n");
    while(1);
}

/* Task yield implementation */
void task_yield(void)
{
    scheduler_schedule();
}
