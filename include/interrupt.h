#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "types.h"

/* Interrupt handler function type */
typedef void (*interrupt_handler_t)(void *arg);

/* Initialize interrupt system */
void interrupt_init(void);

/* Enable interrupts globally */
void interrupt_enable(void);

/* Disable interrupts globally */
void interrupt_disable(void);

/* Register an interrupt handler */
void interrupt_register_handler(uint32_t int_num, interrupt_handler_t handler, void *arg);

/* Unregister an interrupt handler */
void interrupt_unregister_handler(uint32_t int_num);

#endif /* INTERRUPT_H */
