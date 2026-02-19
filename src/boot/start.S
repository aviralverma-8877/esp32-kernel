/* ESP32 Boot Startup Code */
/* This is the first code that runs after the ROM bootloader */

    .section .iram.vectors, "ax"
    .global _start
    .type _start, @function

_start:
    /* Disable interrupts */
    rsil a2, 15

    /* Set up stack pointer */
    movi a1, _stack_top

    /* Clear the BSS section */
    movi a2, _bss_start
    movi a3, _bss_end
    bgeu a2, a3, .bss_done
.bss_loop:
    s32i a0, a2, 0
    addi a2, a2, 4
    bltu a2, a3, .bss_loop
.bss_done:

    /* Set up the window overflow area */
    /* Xtensa requires 4 register windows */
    movi a0, 0
    wsr a0, windowstart
    movi a0, 1
    wsr a0, windowbase
    rsync

    /* Initialize PS register for user mode and interrupts */
    movi a0, 0x00040020  /* PS: WOE=1, CALLINC=0, UM=0, INTLEVEL=0 */
    wsr a0, ps
    rsync

    /* Call the C initialization function */
    call4 init_system

    /* Should never return, but if it does, loop forever */
.halt:
    waiti 15
    j .halt

    .size _start, . - _start


/* Exception and interrupt vectors */
    .section .iram0.text
    .global _KernelExceptionVector
    .type _KernelExceptionVector, @function
    .align 4

_KernelExceptionVector:
    /* For now, just loop forever on exceptions */
    waiti 15
    j _KernelExceptionVector

    .size _KernelExceptionVector, . - _KernelExceptionVector


    .global _UserExceptionVector
    .type _UserExceptionVector, @function
    .align 4

_UserExceptionVector:
    /* For now, just loop forever on exceptions */
    waiti 15
    j _UserExceptionVector

    .size _UserExceptionVector, . - _UserExceptionVector


    .global _DoubleExceptionVector
    .type _DoubleExceptionVector, @function
    .align 4

_DoubleExceptionVector:
    /* For now, just loop forever on exceptions */
    waiti 15
    j _DoubleExceptionVector

    .size _DoubleExceptionVector, . - _DoubleExceptionVector
