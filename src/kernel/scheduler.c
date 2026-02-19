#include "kernel.h"
#include "task.h"
#include "uart.h"
#include "esp32_defs.h"

/* Scheduler state */
static bool scheduler_running = false;

/* Initialize scheduler */
void scheduler_init(void)
{
    uart_puts("[SCHED] Scheduler initialized\n");
    scheduler_running = false;
}

/* Start the scheduler (never returns) */
void scheduler_start(void)
{
    uart_puts("[SCHED] Starting scheduler...\n");
    scheduler_running = true;

    /* Get first ready task */
    task_t *first_task = task_get_next_ready();
    if (!first_task) {
        uart_puts("[SCHED] ERROR: No tasks to run!\n");
        while(1);
    }

    /* Set as current task */
    first_task->state = TASK_STATE_RUNNING;
    task_set_current(first_task);

    uart_printf("[SCHED] Starting task '%s'\n", first_task->name);

    /* Jump to first task (assembly) */
    /* We need to restore the context and jump to the task */
    __asm__ volatile (
        "mov a1, %0\n"          /* Load stack pointer */
        "l32i a0, a1, 0\n"      /* Load A0 */
        "l32i a2, a1, 4\n"      /* Load A2 (arg) */
        "l32i a15, a1, 60\n"    /* Load PC */
        "l32i a14, a1, 64\n"    /* Load PS */
        "wsr a14, ps\n"         /* Restore PS */
        "rsync\n"
        "jx a15\n"              /* Jump to task entry */
        : : "r" (first_task->stack_ptr)
    );

    /* Should never reach here */
    while(1);
}

/* Schedule next task (called by task_yield) */
void scheduler_schedule(void)
{
    if (!scheduler_running) {
        return;
    }

    task_t *current = task_get_current();
    task_t *next = task_get_next_ready();

    if (!next) {
        uart_puts("[SCHED] WARNING: No ready tasks, staying with current\n");
        return;
    }

    if (next == current) {
        /* Same task, no need to switch */
        return;
    }

    /* Save current task state */
    if (current && current->state == TASK_STATE_RUNNING) {
        current->state = TASK_STATE_READY;
    }

    /* Set next task as running */
    next->state = TASK_STATE_RUNNING;
    task_set_current(next);

    /* Perform context switch */
    if (current) {
        context_switch(&current->stack_ptr, next->stack_ptr);
    } else {
        /* No previous task, just restore next task context */
        __asm__ volatile (
            "mov a1, %0\n"
            : : "r" (next->stack_ptr)
        );
    }
}

/* Delay in milliseconds */
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        ets_delay_us(1000);
    }
}

/* Delay in microseconds */
void delay_us(uint32_t us)
{
    ets_delay_us(us);
}
