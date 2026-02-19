#ifndef TYPES_H
#define TYPES_H

/* Standard integer types */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

/* Size types */
typedef unsigned int       size_t;
typedef signed int         ssize_t;

/* Boolean type */
typedef enum {
    false = 0,
    true = 1
} bool;

/* NULL pointer */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* Useful macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Bit manipulation */
#define BIT(n) (1U << (n))
#define SET_BIT(reg, bit) ((reg) |= BIT(bit))
#define CLEAR_BIT(reg, bit) ((reg) &= ~BIT(bit))
#define READ_BIT(reg, bit) (((reg) >> (bit)) & 1U)

/* Register access */
#define REG_READ(addr) (*((volatile uint32_t *)(addr)))
#define REG_WRITE(addr, val) (*((volatile uint32_t *)(addr)) = (val))
#define REG_SET_BIT(addr, bit) (REG_WRITE((addr), REG_READ(addr) | BIT(bit)))
#define REG_CLEAR_BIT(addr, bit) (REG_WRITE((addr), REG_READ(addr) & ~BIT(bit)))

/* Alignment macros */
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))

/* Min/Max macros */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif /* TYPES_H */
