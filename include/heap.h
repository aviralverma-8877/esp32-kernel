#ifndef HEAP_H
#define HEAP_H

#include "types.h"

/* Initialize heap allocator */
void heap_init(void);

/* Allocate memory from heap */
void *kmalloc(size_t size);

/* Free memory back to heap */
void kfree(void *ptr);

/* Get heap statistics */
void heap_stats(uint32_t *total, uint32_t *used, uint32_t *free);

#endif /* HEAP_H */
