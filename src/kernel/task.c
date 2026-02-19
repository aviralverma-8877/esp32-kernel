#include "task.h"
#include "heap.h"
#include "uart.h"

/* Current running task */
static task_t *current_task = NULL;

/* Task list */
static task_t *task_list[MAX_TASKS];
static uint32_t task_count = 0;
static uint32_t next_task_id = 0;

/* String copy function */
static void strncpy_safe(char *dst, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

/* Initialize Xtensa register context on stack */
static void task_init_stack(task_t *task)
{
    uint32_t *stack_top = (uint32_t *)(task->stack_base + task->stack_size);

    /* Reserve space for context (Xtensa windowed ABI) */
    /* We need space for: A0-A15, PC, PS, SAR */
    stack_top -= 19;  /* 19 registers */

    /* Initialize stack frame */
    stack_top[0] = 0;                        /* A0 (return address) */
    stack_top[1] = (uint32_t)task->arg;      /* A2 (first argument) */
    stack_top[2] = 0;                        /* A3 */
    stack_top[3] = 0;                        /* A4 */
    stack_top[4] = 0;                        /* A5 */
    stack_top[5] = 0;                        /* A6 */
    stack_top[6] = 0;                        /* A7 */
    stack_top[7] = 0;                        /* A8 */
    stack_top[8] = 0;                        /* A9 */
    stack_top[9] = 0;                        /* A10 */
    stack_top[10] = 0;                       /* A11 */
    stack_top[11] = 0;                       /* A12 */
    stack_top[12] = 0;                       /* A13 */
    stack_top[13] = 0;                       /* A14 */
    stack_top[14] = 0;                       /* A15 */
    stack_top[15] = (uint32_t)task->entry;   /* PC (entry point) */
    stack_top[16] = 0x00040020;              /* PS (user mode, interrupts enabled) */
    stack_top[17] = 0;                       /* SAR */
    stack_top[18] = (uint32_t)task_exit;     /* A1 (exit function if task returns) */

    /* Set the task's stack pointer */
    task->stack_ptr = stack_top;
}

/* Initialize task system */
void task_init(void)
{
    task_count = 0;
    next_task_id = 0;
    current_task = NULL;

    for (uint32_t i = 0; i < MAX_TASKS; i++) {
        task_list[i] = NULL;
    }

    uart_puts("[TASK] Task system initialized\n");
}

/* Create a new task */
task_t *task_create(const char *name, task_entry_t entry, void *arg, uint32_t stack_size)
{
    if (task_count >= MAX_TASKS) {
        uart_puts("[TASK] ERROR: Maximum tasks reached\n");
        return NULL;
    }

    /* Allocate TCB */
    task_t *task = (task_t *)kmalloc(sizeof(task_t));
    if (!task) {
        uart_puts("[TASK] ERROR: Failed to allocate TCB\n");
        return NULL;
    }

    /* Allocate stack */
    uint32_t *stack = (uint32_t *)kmalloc(stack_size);
    if (!stack) {
        uart_puts("[TASK] ERROR: Failed to allocate stack\n");
        kfree(task);
        return NULL;
    }

    /* Initialize TCB */
    task->entry = entry;
    task->arg = arg;
    task->state = TASK_STATE_READY;
    task->stack_base = (uint32_t)stack;
    task->stack_size = stack_size;
    task->id = next_task_id++;
    strncpy_safe(task->name, name, sizeof(task->name));

    /* Initialize stack with context */
    task_init_stack(task);

    /* Add to task list */
    task_list[task_count++] = task;

    uart_printf("[TASK] Created task '%s' (ID: %d, stack: %x)\n",
                task->name, task->id, task->stack_base);

    return task;
}

/* Get current running task */
task_t *task_get_current(void)
{
    return current_task;
}

/* Set current running task */
void task_set_current(task_t *task)
{
    current_task = task;
}

/* Terminate current task */
void task_exit(void)
{
    if (current_task) {
        uart_printf("[TASK] Task '%s' exiting\n", current_task->name);
        current_task->state = TASK_STATE_TERMINATED;
        task_yield();  /* Switch to another task */
    }

    /* Should never reach here */
    while(1);
}

/* Get next ready task (round-robin) */
task_t *task_get_next_ready(void)
{
    static uint32_t last_index = 0;
    uint32_t start_index = last_index;

    do {
        last_index = (last_index + 1) % task_count;
        if (task_list[last_index] &&
            task_list[last_index]->state == TASK_STATE_READY) {
            return task_list[last_index];
        }
    } while (last_index != start_index);

    return NULL;  /* No ready tasks */
}
