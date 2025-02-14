#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "allocator.h"

#define MEMORY_SIZE 4096
#define NUM_ITERATIONS 1000

int main() {
    void* buffer1 = malloc(MEMORY_SIZE);
    void* buffer2 = malloc(MEMORY_SIZE);
    
    Allocator* freeListAllocInst = createFreeListAllocator(buffer1, MEMORY_SIZE);
    Allocator* buddyAllocInst    = createBuddyAllocator(buffer2, MEMORY_SIZE);
    
    void* allocationsFreeList[NUM_ITERATIONS];
    void* allocationsBuddy[NUM_ITERATIONS];
    
    clock_t start, end;
    double allocTime, freeTime;
    
    // Тест выделения памяти для free-list
    start = clock();
    for (size_t i = 0; i < NUM_ITERATIONS; i++) {
        allocationsFreeList[i] = freeListAllocInst->alloc(freeListAllocInst, 32);
    }
    end = clock();
    allocTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Free-list allocation time: %f seconds\n", allocTime);
    
    // Тест освобождения памяти для free-list
    start = clock();
    for (size_t i = 0; i < NUM_ITERATIONS; i++) {
        freeListAllocInst->free_mem(freeListAllocInst, allocationsFreeList[i]);
    }
    end = clock();
    freeTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Free-list free time: %f seconds\n", freeTime);
    
    // Тест выделения памяти для buddy-системы
    start = clock();
    for (size_t i = 0; i < NUM_ITERATIONS; i++) {
        allocationsBuddy[i] = buddyAllocInst->alloc(buddyAllocInst, 32);
    }
    end = clock();
    allocTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Buddy system allocation time: %f seconds\n", allocTime);
    
    // Тест освобождения памяти для buddy-системы
    start = clock();
    for (size_t i = 0; i < NUM_ITERATIONS; i++) {
        buddyAllocInst->free_mem(buddyAllocInst, allocationsBuddy[i]);
    }
    end = clock();
    freeTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Buddy system free time: %f seconds\n", freeTime);
    
    free(buffer1);
    free(buffer2);
    
    return 0;
}