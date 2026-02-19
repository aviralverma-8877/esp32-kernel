#ifndef UART_H
#define UART_H

#include "types.h"

/* UART configuration */
#define UART_BAUD_RATE  115200

/* Initialize UART0 for serial communication */
void uart_init(void);

/* Write a single character to UART */
void uart_putc(char c);

/* Write a null-terminated string to UART */
void uart_puts(const char *str);

/* Read a character from UART (blocking) */
char uart_getc(void);

/* Check if data is available to read */
bool uart_available(void);

/* Simple printf-like function for formatted output */
void uart_printf(const char *fmt, ...);

/* Write formatted string with arguments */
void uart_vprintf(const char *fmt, uint32_t *args);

#endif /* UART_H */
