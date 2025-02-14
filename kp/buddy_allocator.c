#include <stdio.h>
#include <math.h>
#include "allocator.h"

typedef struct {
    void* start;
    void* end; 
    size_t totalSize;
    int maxOrder;
    void** freeLists; 
} BuddyMeta;

static int findOrder(size_t size) {
    int order = 0;
    size_t currentSize = 1;
    while (currentSize < size) {
        currentSize <<= 1;
        order++;
    }
    return order;
}

// Рекурсивная функция объединения buddy-блоков
static void buddyMerge(BuddyMeta* meta, void* ptr, int order) {
    if (order >= meta->maxOrder) {
        // Достигли максимального порядка
        *(void**)ptr = meta->freeLists[order];
        meta->freeLists[order] = ptr;
        return;
    }
    // Вычисляем "базу" адреса при данном порядке
    size_t offset = (char*)ptr - (char*)meta->start;
    size_t sizeBlock = (1 << order);
    size_t buddyOffset;

    // Определяем, является ли ptr чётным или нечётным блоком
    if (((offset / sizeBlock) & 1) == 0) {
        // Buddy-блок будет справа
        buddyOffset = offset + sizeBlock;
    } else {
        // Buddy-блок будет слева
        buddyOffset = offset - sizeBlock;
    }
    void* buddyPtr = (char*)meta->start + buddyOffset;

    // Ищем buddyPtr в списке свободных блоков того же порядка
    void** prev = &meta->freeLists[order];
    void* curr = meta->freeLists[order];
    while (curr) {
        if (curr == buddyPtr) {
            // Нашли buddy — убираем его из списка
            *prev = *(void**)curr;
            // Рекурсивно объединяем два блока в более крупный (order+1)
            void* merged = (offset < buddyOffset) ? ptr : buddyPtr;
            buddyMerge(meta, merged, order+1);
            return;
        }
        prev = (void**)curr;
        curr = *(void**)curr;
    }

    // Если buddy не найден, просто кладём ptr в список
    *(void**)ptr = meta->freeLists[order];
    meta->freeLists[order] = ptr;
}

static void* buddyAlloc(Allocator* allocator, size_t block_size) {
    BuddyMeta* meta = (BuddyMeta*)allocator->memory;
    int order = findOrder(block_size);
    if (order > meta->maxOrder) return NULL;

    // Ищем свободные блоки нужного порядка или выше
    int o = order;
    while (o <= meta->maxOrder && !meta->freeLists[o]) {
        o++;
    }
    if (o > meta->maxOrder) {
        return NULL; // нет подходящих блоков
    }

    // Разделяем более крупные блоки на два меньших, пока не достигнем order
    void* block = meta->freeLists[o];
    meta->freeLists[o] = *(void**)block; // удаляем из списка
    while (o > order) {
        o--;
        char* buddy = (char*)block + (1 << o);
        *(void**)buddy = meta->freeLists[o];
        meta->freeLists[o] = buddy;
    }
    return block;
}

static void buddyFree(Allocator* allocator, void* ptr) {
    if (!ptr) return;
    BuddyMeta* meta = (BuddyMeta*)allocator->memory;

    // Проверяем, лежит ли ptr в рамках диапазона
    if (ptr < meta->start || ptr >= meta->end) {
        fprintf(stderr, "[buddyFree] Invalid pointer!\n");
        return;
    }

    // Определяем минимальный порядок, на самом деле нужно знать точный размер,
    // но для простоты пусть будет минимальный порядок 0 (1 байт).
    int order = 0; 
    // Попробуем найти возможное слияние buddy-блоков
    buddyMerge(meta, ptr, order);
}

Allocator* createBuddyAllocator(void *realMemory, size_t memory_size) {
    Allocator* newAllocator = (Allocator*)realMemory;
    BuddyMeta* meta = (BuddyMeta*)((char*)realMemory + sizeof(Allocator));

    newAllocator->memory = meta;
    newAllocator->size = memory_size - sizeof(Allocator);
    newAllocator->alloc = buddyAlloc;
    newAllocator->free_mem = buddyFree;

    meta->start = (char*)realMemory + sizeof(Allocator) + sizeof(BuddyMeta);
    meta->end = (char*)realMemory + memory_size;
    meta->totalSize = newAllocator->size - sizeof(BuddyMeta);
    meta->maxOrder = (int)floor(log2(meta->totalSize));
    // Массив списков по порядкам
    int arraySize = meta->maxOrder + 1;
    meta->freeLists = (void**)((char*)meta->start);
    // Устанавливаем начало свободной памяти после массива указателей
    meta->start = (char*)meta->start + arraySize * sizeof(void*);
    meta->totalSize -= arraySize * sizeof(void*);

    // Изначально один большой блок макс. порядка
    for (int i = 0; i <= meta->maxOrder; i++) {
        meta->freeLists[i] = NULL;
    }
    meta->freeLists[meta->maxOrder] = meta->start;
    // Следующий указатель в свободном блоке
    *(void**)meta->freeLists[meta->maxOrder] = NULL;

    return newAllocator;
}
