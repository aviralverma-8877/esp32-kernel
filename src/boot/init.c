#include "esp32_defs.h"
#include "uart.h"

/* Forward declarations */
extern void kernel_main(void);

/* Initialize system hardware and clocks */
void init_system(void)
{
    /* Disable APP CPU (we only use PRO CPU for this kernel) */
    REG_WRITE(DPORT_APPCPU_CTRL_B_REG, 0);

    /* Initialize UART for early debugging */
    uart_init();

    /* Print boot message */
    uart_puts("\r\n");
    uart_puts("==============================\r\n");
    uart_puts("ESP32 Bare-Metal Kernel\r\n");
    uart_puts("==============================\r\n");
    uart_puts("[BOOT] Starting initialization...\r\n");

    /* Jump to kernel main */
    uart_puts("[BOOT] Jumping to kernel_main\r\n");
    kernel_main();

    /* Should never return */
    uart_puts("[BOOT] ERROR: kernel_main returned!\r\n");
    while(1);
}
