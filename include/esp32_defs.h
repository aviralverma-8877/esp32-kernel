#ifndef ESP32_DEFS_H
#define ESP32_DEFS_H

#include "types.h"

/* ===== Memory Map ===== */
#define DRAM_BASE           0x3FFB0000
#define IRAM_BASE           0x40080000
#define DROM_BASE           0x3F400000
#define IROM_BASE           0x400D0000

/* ===== Peripheral Base Addresses ===== */
#define DR_REG_UART_BASE            0x3FF40000
#define DR_REG_UART1_BASE           0x3FF50000
#define DR_REG_UART2_BASE           0x3FF6E000
#define DR_REG_GPIO_BASE            0x3FF44000
#define DR_REG_IO_MUX_BASE          0x3FF49000
#define DR_REG_DPORT_BASE           0x3FF00000
#define DR_REG_INTERRUPT_BASE       0x3FF00000

/* ===== UART0 Registers ===== */
#define UART_FIFO_REG(i)            (DR_REG_UART_BASE + 0x00)
#define UART_INT_RAW_REG(i)         (DR_REG_UART_BASE + 0x04)
#define UART_INT_ST_REG(i)          (DR_REG_UART_BASE + 0x08)
#define UART_INT_ENA_REG(i)         (DR_REG_UART_BASE + 0x0C)
#define UART_INT_CLR_REG(i)         (DR_REG_UART_BASE + 0x10)
#define UART_CLKDIV_REG(i)          (DR_REG_UART_BASE + 0x14)
#define UART_STATUS_REG(i)          (DR_REG_UART_BASE + 0x1C)
#define UART_CONF0_REG(i)           (DR_REG_UART_BASE + 0x20)
#define UART_CONF1_REG(i)           (DR_REG_UART_BASE + 0x24)

/* UART Status bits */
#define UART_TXFIFO_CNT             0x000000FF
#define UART_RXFIFO_CNT             0x000000FF
#define UART_TXFIFO_CNT_S           16
#define UART_RXFIFO_CNT_S           0

/* UART Config bits */
#define UART_TICK_REF_ALWAYS_ON     BIT(27)
#define UART_PARITY_EN              BIT(1)
#define UART_PARITY                 BIT(0)

/* ===== GPIO Registers ===== */
#define GPIO_OUT_REG                (DR_REG_GPIO_BASE + 0x04)
#define GPIO_OUT_W1TS_REG           (DR_REG_GPIO_BASE + 0x08)
#define GPIO_OUT_W1TC_REG           (DR_REG_GPIO_BASE + 0x0C)
#define GPIO_ENABLE_REG             (DR_REG_GPIO_BASE + 0x20)
#define GPIO_ENABLE_W1TS_REG        (DR_REG_GPIO_BASE + 0x24)
#define GPIO_ENABLE_W1TC_REG        (DR_REG_GPIO_BASE + 0x28)
#define GPIO_IN_REG                 (DR_REG_GPIO_BASE + 0x3C)
#define GPIO_STATUS_REG             (DR_REG_GPIO_BASE + 0x44)
#define GPIO_STATUS_W1TC_REG        (DR_REG_GPIO_BASE + 0x48)

#define GPIO_PIN0_REG               (DR_REG_GPIO_BASE + 0x88)
#define GPIO_FUNC_OUT_SEL_CFG_REG(n) (DR_REG_GPIO_BASE + 0x530 + (n)*4)

/* GPIO pin numbers */
#define GPIO_NUM_0      0
#define GPIO_NUM_1      1
#define GPIO_NUM_2      2
#define GPIO_NUM_3      3
#define GPIO_NUM_4      4
#define GPIO_NUM_5      5

/* ===== IO MUX Registers ===== */
#define PERIPHS_IO_MUX_GPIO0_U      (DR_REG_IO_MUX_BASE + 0x44)
#define PERIPHS_IO_MUX_U0TXD_U      (DR_REG_IO_MUX_BASE + 0x88)  /* GPIO1 */
#define PERIPHS_IO_MUX_GPIO2_U      (DR_REG_IO_MUX_BASE + 0x40)
#define PERIPHS_IO_MUX_U0RXD_U      (DR_REG_IO_MUX_BASE + 0x84)  /* GPIO3 */

/* IO MUX bits */
#define FUN_DRV_S                   10
#define FUN_DRV                     0x3
#define FUN_IE                      BIT(9)
#define FUN_WPU                     BIT(8)
#define FUN_WPD                     BIT(7)
#define MCU_SEL_S                   12
#define MCU_SEL                     0x7

/* ===== DPORT Registers (System/Clock) ===== */
#define DPORT_APPCPU_CTRL_A_REG     (DR_REG_DPORT_BASE + 0x02C)
#define DPORT_APPCPU_CTRL_B_REG     (DR_REG_DPORT_BASE + 0x030)
#define DPORT_CPU_PER_CONF_REG      (DR_REG_DPORT_BASE + 0x03C)
#define DPORT_PRO_CACHE_CTRL_REG    (DR_REG_DPORT_BASE + 0x040)
#define DPORT_PRO_CACHE_CTRL1_REG   (DR_REG_DPORT_BASE + 0x044)

/* ===== Interrupt Registers ===== */
#define DPORT_PRO_INTR_STATUS_0_REG     (DR_REG_DPORT_BASE + 0x0DC)
#define DPORT_PRO_INTR_STATUS_1_REG     (DR_REG_DPORT_BASE + 0x0E0)
#define DPORT_PRO_INTR_STATUS_2_REG     (DR_REG_DPORT_BASE + 0x0E4)

/* ===== CPU Frequency ===== */
#define APB_CLK_FREQ                80000000  /* 80 MHz */
#define CPU_CLK_FREQ                160000000 /* 160 MHz */

/* ===== Interrupt Numbers ===== */
#define ETS_UART0_INUM              5
#define ETS_GPIO_INUM               10
#define ETS_TIMER1_INUM             16

/* ===== ROM Functions ===== */
/* ESP32 ROM contains useful functions we can call */
extern void ets_delay_us(uint32_t us);
extern void ets_printf(const char *fmt, ...);

/* ===== External symbols from linker script ===== */
extern uint32_t _bss_start;
extern uint32_t _bss_end;
extern uint32_t _data_start;
extern uint32_t _data_end;
extern uint32_t _stack_top;
extern uint32_t _heap_start;
extern uint32_t _heap_end;

#endif /* ESP32_DEFS_H */
