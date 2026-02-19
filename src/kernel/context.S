/* Context switching for Xtensa ESP32 */
/* Saves current task context and restores next task context */

    .section .iram0.text
    .global context_switch
    .type context_switch, @function
    .align 4

/* void context_switch(uint32_t **old_sp, uint32_t *new_sp) */
/* a2 = pointer to old stack pointer (save current SP here) */
/* a3 = new stack pointer (restore from here) */
context_switch:
    /* Entry: Save minimal state for call4 */
    entry a1, 32

    /* Save callee-saved registers on current stack */
    /* We need to save: A0-A15, PC, PS, SAR */

    /* Allocate space on stack for context (19 words) */
    addi a1, a1, -76

    /* Save all registers */
    s32i a0, a1, 0      /* A0 - return address */
    s32i a2, a1, 4      /* A2 */
    s32i a3, a1, 8      /* A3 */
    s32i a4, a1, 12     /* A4 */
    s32i a5, a1, 16     /* A5 */
    s32i a6, a1, 20     /* A6 */
    s32i a7, a1, 24     /* A7 */
    s32i a8, a1, 28     /* A8 */
    s32i a9, a1, 32     /* A9 */
    s32i a10, a1, 36    /* A10 */
    s32i a11, a1, 40    /* A11 */
    s32i a12, a1, 44    /* A12 */
    s32i a13, a1, 48    /* A13 */
    s32i a14, a1, 52    /* A14 */
    s32i a15, a1, 56    /* A15 */

    /* Save PS and SAR */
    rsr a4, ps
    s32i a4, a1, 64     /* PS */
    rsr a4, sar
    s32i a4, a1, 68     /* SAR */

    /* Save current PC (return address) */
    s32i a0, a1, 60     /* PC */

    /* Save current SP to *old_sp */
    /* a2 still contains old_sp pointer from function argument */
    l32i a4, a1, 76     /* Load original a2 from entry frame */
    s32i a1, a4, 0      /* Store current SP to *old_sp */

    /* Load new SP from a3 (new_sp) */
    l32i a1, a1, 80     /* Load original a3 from entry frame */

    /* Restore registers from new stack */
    l32i a0, a1, 0      /* A0 */
    l32i a2, a1, 4      /* A2 */
    l32i a3, a1, 8      /* A3 */
    l32i a4, a1, 12     /* A4 */
    l32i a5, a1, 16     /* A5 */
    l32i a6, a1, 20     /* A6 */
    l32i a7, a1, 24     /* A7 */
    l32i a8, a1, 28     /* A8 */
    l32i a9, a1, 32     /* A9 */
    l32i a10, a1, 36    /* A10 */
    l32i a11, a1, 40    /* A11 */
    l32i a12, a1, 44    /* A12 */
    l32i a13, a1, 48    /* A13 */
    l32i a14, a1, 52    /* A14 */
    l32i a15, a1, 56    /* A15 */

    /* Restore PS and SAR */
    l32i a4, a1, 64     /* PS */
    wsr a4, ps
    l32i a4, a1, 68     /* SAR */
    wsr a4, sar
    rsync

    /* Deallocate context frame */
    addi a1, a1, 76

    /* Return to new task */
    ret

    .size context_switch, . - context_switch
