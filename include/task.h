#ifndef TASK_H
#define TASK_H

#include "types.h"

/* Task states */
typedef enum {
    TASK_STATE_READY = 0,
    TASK_STATE_RUNNING,
    TASK_STATE_BLOCKED,
    TASK_STATE_TERMINATED
} task_state_t;

/* Task stack size */
#define TASK_STACK_SIZE  2048  /* 2KB per task */
#define MAX_TASKS        8     /* Maximum number of tasks */

/* Task entry point function type */
typedef void (*task_entry_t)(void *arg);

/* Task Control Block (TCB) */
typedef struct {
    uint32_t *stack_ptr;           /* Current stack pointer */
    task_entry_t entry;             /* Task entry function */
    void *arg;                      /* Task argument */
    task_state_t state;             /* Current task state */
    char name[16];                  /* Task name for debugging */
    uint32_t stack_base;            /* Base address of stack */
    uint32_t stack_size;            /* Size of stack */
    uint32_t id;                    /* Task ID */
} task_t;

/* Initialize task system */
void task_init(void);

/* Create a new task */
task_t *task_create(const char *name, task_entry_t entry, void *arg, uint32_t stack_size);

/* Get current running task */
task_t *task_get_current(void);

/* Set current running task */
void task_set_current(task_t *task);

/* Terminate current task */
void task_exit(void);

/* Yield CPU to next task */
void task_yield(void);

#endif /* TASK_H */
