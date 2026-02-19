#include "heap.h"
#include "esp32_defs.h"
#include "uart.h"

/* Heap memory block header */
typedef struct heap_block {
    size_t size;                    /* Size of this block (excluding header) */
    bool is_free;                   /* Is this block free? */
    struct heap_block *next;        /* Next block in list */
} heap_block_t;

#define HEAP_BLOCK_HEADER_SIZE sizeof(heap_block_t)
#define ALIGN_SIZE 4

/* Heap start and end from linker script */
extern uint32_t _heap_start;
extern uint32_t _heap_end;

/* Heap state */
static heap_block_t *heap_head = NULL;
static uint32_t heap_size = 0;
static uint32_t heap_used = 0;

/* Align size to 4-byte boundary */
static size_t align_size(size_t size)
{
    return (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
}

/* Initialize heap allocator */
void heap_init(void)
{
    heap_head = (heap_block_t *)&_heap_start;
    heap_size = (uint32_t)&_heap_end - (uint32_t)&_heap_start;
    heap_used = HEAP_BLOCK_HEADER_SIZE;

    /* Initialize first free block */
    heap_head->size = heap_size - HEAP_BLOCK_HEADER_SIZE;
    heap_head->is_free = true;
    heap_head->next = NULL;

    uart_printf("[HEAP] Initialized: %d bytes available\n", heap_head->size);
}

/* Allocate memory from heap */
void *kmalloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    size = align_size(size);

    heap_block_t *current = heap_head;
    heap_block_t *prev = NULL;

    /* Find first fit free block */
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            /* Found a suitable block */

            /* If block is large enough, split it */
            if (current->size >= size + HEAP_BLOCK_HEADER_SIZE + ALIGN_SIZE) {
                /* Create new free block for remaining space */
                heap_block_t *new_block = (heap_block_t *)((uint32_t)current + HEAP_BLOCK_HEADER_SIZE + size);
                new_block->size = current->size - size - HEAP_BLOCK_HEADER_SIZE;
                new_block->is_free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = false;
            heap_used += current->size + HEAP_BLOCK_HEADER_SIZE;

            /* Return pointer to data (after header) */
            return (void *)((uint32_t)current + HEAP_BLOCK_HEADER_SIZE);
        }

        prev = current;
        current = current->next;
    }

    /* No suitable block found */
    uart_printf("[HEAP] ERROR: Out of memory (requested: %d bytes)\n", size);
    return NULL;
}

/* Free memory back to heap */
void kfree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    /* Get block header */
    heap_block_t *block = (heap_block_t *)((uint32_t)ptr - HEAP_BLOCK_HEADER_SIZE);

    if (block->is_free) {
        uart_puts("[HEAP] WARNING: Double free detected\n");
        return;
    }

    block->is_free = true;
    heap_used -= block->size + HEAP_BLOCK_HEADER_SIZE;

    /* Coalesce adjacent free blocks */
    heap_block_t *current = heap_head;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            /* Merge with next block */
            current->size += current->next->size + HEAP_BLOCK_HEADER_SIZE;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

/* Get heap statistics */
void heap_stats(uint32_t *total, uint32_t *used, uint32_t *free)
{
    if (total) *total = heap_size;
    if (used) *used = heap_used;
    if (free) *free = heap_size - heap_used;
}
