#include <stdio.h>
#include <string.h>
#include "allocator.h"

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

typedef struct {
    void *start;
    void *end;
    size_t totalSize;
    FreeBlock* freeList;
} FreeListMeta;

// Вспомогательная функция для слияния двух блоков, если они идут подряд
static void mergeBlocks(FreeBlock* block) {
    FreeBlock* nextBlock = block->next;
    // Если соседний блок идёт сразу за этим
    if (nextBlock && ((char*)block + sizeof(FreeBlock) + block->size == (char*)nextBlock)) {
        block->size += sizeof(FreeBlock) + nextBlock->size;
        block->next = nextBlock->next;
    }
}

static void* freeListAlloc(Allocator* allocator, size_t block_size) {
    FreeListMeta* meta = (FreeListMeta*)allocator->memory;
    FreeBlock* prev = NULL;
    FreeBlock* current = meta->freeList;

    while (current) {
        if (current->size >= block_size) {
            // Найден подходящий блок
            size_t remaining = current->size - block_size;
            void* allocPtr = (char*)current + sizeof(FreeBlock);

            // Если блок точно подошёл по размеру, удаляем его из списка
            if (remaining < sizeof(FreeBlock)) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    meta->freeList = current->next;
                }
            } else {
                // Иначе уменьшаем размер свободного блока
                FreeBlock* newBlock = (FreeBlock*)((char*)allocPtr + block_size);
                newBlock->size = remaining - sizeof(FreeBlock);
                newBlock->next = current->next;
                if (prev) {
                    prev->next = newBlock;
                } else {
                    meta->freeList = newBlock;
                }
            }
            return allocPtr;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

static void freeListFree(Allocator* allocator, void* ptr) {
    if (!ptr) return;
    FreeListMeta* meta = (FreeListMeta*)allocator->memory;

    // Упрощённая проверка: лежит ли ptr в рамках [meta->start, meta->end]
    if (ptr < meta->start || ptr >= meta->end) {
        // В реальном проекте вернём ошибку или сделаем assert
        fprintf(stderr, "[freeListFree] Invalid pointer!\n");
        return;
    }

    // Указатель на структуру FreeBlock, помещаем её перед ptr
    FreeBlock* block = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));

    // Включаем блок обратно в список
    block->next = meta->freeList;
    meta->freeList = block;

    // Пытаемся слить блок с соседними
    mergeBlocks(block);

    // Если блок стал не первым, проверим, может ли предыдущий блок слиться с этим
    if (block != meta->freeList) {
        FreeBlock* current = meta->freeList;
        while (current && current->next && current->next != block) {
            current = current->next;
        }
        if (current) {
            mergeBlocks(current);
        }
    }
}

Allocator* createFreeListAllocator(void *realMemory, size_t memory_size) {
    Allocator* newAllocator = (Allocator*)realMemory;
    FreeListMeta* meta = (FreeListMeta*)((char*)realMemory + sizeof(Allocator));

    newAllocator->memory = meta;
    newAllocator->size = memory_size - sizeof(Allocator);
    newAllocator->alloc = freeListAlloc;
    newAllocator->free_mem = freeListFree;

    meta->start = (char*)realMemory + sizeof(Allocator) + sizeof(FreeListMeta);
    meta->end = (char*)realMemory + memory_size;
    meta->totalSize = newAllocator->size - sizeof(FreeListMeta);

    meta->freeList = (FreeBlock*)(meta->start);
    meta->freeList->size = meta->totalSize - sizeof(FreeBlock);
    meta->freeList->next = NULL;

    return newAllocator;
}