#include "uart.h"
#include "esp32_defs.h"

/* Simple strlen implementation */
static size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* Simple itoa implementation (integer to ASCII) */
static void itoa(int32_t value, char *buffer, int base)
{
    char *ptr = buffer;
    char *ptr1 = buffer;
    char tmp_char;
    int32_t tmp_value;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    /* Handle negative numbers for base 10 */
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        ptr1++;
        value = -value;
    }

    /* Convert to string in reverse order */
    while (value) {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value - value * base];
    }

    *ptr-- = '\0';

    /* Reverse the string */
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}

/* Initialize UART0 */
void uart_init(void)
{
    /* Configure UART0 clock divider for 115200 baud */
    /* Baud rate = APB_CLK / (CLKDIV * 16) */
    /* For 115200: CLKDIV = 80000000 / (115200 * 16) = 43.4 */
    uint32_t clkdiv = (APB_CLK_FREQ << 4) / UART_BAUD_RATE;
    REG_WRITE(UART_CLKDIV_REG(0), clkdiv);

    /* Configure UART parameters: 8N1 (8 bits, no parity, 1 stop bit) */
    uint32_t conf0 = 0;
    conf0 |= (3 << 0);  /* 8 data bits */
    conf0 |= (1 << 4);  /* 1 stop bit */
    REG_WRITE(UART_CONF0_REG(0), conf0);

    /* Reset FIFO */
    REG_SET_BIT(UART_CONF0_REG(0), 17);  /* RXFIFO_RST */
    REG_SET_BIT(UART_CONF0_REG(0), 18);  /* TXFIFO_RST */
    REG_CLEAR_BIT(UART_CONF0_REG(0), 17);
    REG_CLEAR_BIT(UART_CONF0_REG(0), 18);

    /* Configure GPIO pins for UART */
    /* GPIO1 = TXD (already default) */
    /* GPIO3 = RXD (already default) */
    /* The ROM bootloader already configured these pins */
}

/* Write a single character to UART */
void uart_putc(char c)
{
    /* Wait until TX FIFO has space */
    while (((REG_READ(UART_STATUS_REG(0)) >> UART_TXFIFO_CNT_S) & UART_TXFIFO_CNT) >= 126);

    /* Write character to FIFO */
    REG_WRITE(UART_FIFO_REG(0), c);
}

/* Write a null-terminated string to UART */
void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');  /* Convert \n to \r\n */
        }
        uart_putc(*str++);
    }
}

/* Read a character from UART (blocking) */
char uart_getc(void)
{
    /* Wait until RX FIFO has data */
    while (((REG_READ(UART_STATUS_REG(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT) == 0);

    /* Read character from FIFO */
    return (char)(REG_READ(UART_FIFO_REG(0)) & 0xFF);
}

/* Check if data is available to read */
bool uart_available(void)
{
    return ((REG_READ(UART_STATUS_REG(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT) > 0;
}

/* Simple printf-like function */
void uart_printf(const char *fmt, ...)
{
    uint32_t *args = ((uint32_t *)&fmt) + 1;
    uart_vprintf(fmt, args);
}

/* Formatted output with variadic arguments */
void uart_vprintf(const char *fmt, uint32_t *args)
{
    char buffer[32];
    int arg_index = 0;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':  /* Decimal integer */
                case 'i':
                    itoa((int32_t)args[arg_index++], buffer, 10);
                    uart_puts(buffer);
                    break;
                case 'u':  /* Unsigned integer */
                    itoa((uint32_t)args[arg_index++], buffer, 10);
                    uart_puts(buffer);
                    break;
                case 'x':  /* Hexadecimal */
                    itoa((uint32_t)args[arg_index++], buffer, 16);
                    uart_puts(buffer);
                    break;
                case 's':  /* String */
                    uart_puts((char *)args[arg_index++]);
                    break;
                case 'c':  /* Character */
                    uart_putc((char)args[arg_index++]);
                    break;
                case '%':  /* Literal % */
                    uart_putc('%');
                    break;
                default:
                    uart_putc('%');
                    uart_putc(*fmt);
                    break;
            }
        } else {
            uart_putc(*fmt);
        }
        fmt++;
    }
}
