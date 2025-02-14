#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct Allocator {
    void*  memory;
    size_t size;
    void*  (*alloc)(struct Allocator*, size_t);
    void   (*free_mem)(struct Allocator*, void*);
} Allocator;

// Создание аллокатора для списков свободных блоков (first fit)
Allocator* createFreeListAllocator(void *realMemory, size_t memory_size);

// Создание аллокатора для блоков кратных степеням двойки (buddy system)
Allocator* createBuddyAllocator(void *realMemory, size_t memory_size);

#endif