#include "interrupt.h"
#include "uart.h"

#define MAX_INTERRUPTS  32

/* Interrupt handler table */
typedef struct {
    interrupt_handler_t handler;
    void *arg;
} interrupt_entry_t;

static interrupt_entry_t interrupt_table[MAX_INTERRUPTS];

/* Initialize interrupt system */
void interrupt_init(void)
{
    /* Clear all interrupt handlers */
    for (int i = 0; i < MAX_INTERRUPTS; i++) {
        interrupt_table[i].handler = NULL;
        interrupt_table[i].arg = NULL;
    }

    uart_puts("[INT] Interrupt system initialized\n");
}

/* Enable interrupts globally */
void interrupt_enable(void)
{
    __asm__ volatile (
        "rsil a2, 0\n"  /* Set interrupt level to 0 (enable all) */
        : : : "a2"
    );
}

/* Disable interrupts globally */
void interrupt_disable(void)
{
    __asm__ volatile (
        "rsil a2, 15\n"  /* Set interrupt level to 15 (disable all) */
        : : : "a2"
    );
}

/* Register an interrupt handler */
void interrupt_register_handler(uint32_t int_num, interrupt_handler_t handler, void *arg)
{
    if (int_num >= MAX_INTERRUPTS) {
        uart_puts("[INT] ERROR: Invalid interrupt number\n");
        return;
    }

    interrupt_table[int_num].handler = handler;
    interrupt_table[int_num].arg = arg;

    uart_printf("[INT] Registered handler for interrupt %d\n", int_num);
}

/* Unregister an interrupt handler */
void interrupt_unregister_handler(uint32_t int_num)
{
    if (int_num >= MAX_INTERRUPTS) {
        return;
    }

    interrupt_table[int_num].handler = NULL;
    interrupt_table[int_num].arg = NULL;
}

/* Common interrupt dispatcher (called from assembly) */
void interrupt_dispatch(uint32_t int_num)
{
    if (int_num < MAX_INTERRUPTS && interrupt_table[int_num].handler) {
        interrupt_table[int_num].handler(interrupt_table[int_num].arg);
    } else {
        uart_printf("[INT] Unhandled interrupt: %d\n", int_num);
    }
}
