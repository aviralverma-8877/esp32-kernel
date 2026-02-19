#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"
#include "task.h"

/* Scheduler functions */
void scheduler_init(void);
void scheduler_start(void) __attribute__((noreturn));
void scheduler_schedule(void);

/* Context switch function (implemented in assembly) */
extern void context_switch(uint32_t **old_sp, uint32_t *new_sp);

/* Delay functions */
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#endif /* KERNEL_H */
