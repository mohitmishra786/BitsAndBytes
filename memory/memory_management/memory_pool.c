#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>

#define POOL_SIZE 1024 * 1024
#define BLOCK_SIZE 256
#define NUM_BLOCKS (POOL_SIZE / BLOCK_SIZE)

typedef struct {
    char memory[POOL_SIZE];
    char used[NUM_BLOCKS];
    size_t num_allocations;
    size_t num_frees;
} MemoryPool;

MemoryPool* create_pool() {
    MemoryPool* pool = malloc(sizeof(MemoryPool));
    memset(pool->used, 0, NUM_BLOCKS);
    pool->num_allocations = 0;
    pool->num_frees = 0;
    return pool;
}

void* pool_alloc(MemoryPool* pool) {
    for(int i = 0; i < NUM_BLOCKS; i++) {
        if(!pool->used[i]) {
            pool->used[i] = 1;
            pool->num_allocations++;
            return &pool->memory[i * BLOCK_SIZE];
        }
    }
    return NULL;
}

void pool_free(MemoryPool* pool, void* ptr) {
    if(!ptr) return;
    
    ptrdiff_t offset = (char*)ptr - pool->memory;
    int block_index = offset / BLOCK_SIZE;
    
    if(block_index >= 0 && block_index < NUM_BLOCKS) {
        pool->used[block_index] = 0;
        pool->num_frees++;
    }
}

void destroy_pool(MemoryPool* pool) {
    free(pool);
}

void compare_performance() {
    struct timeval start, end;
    const int NUM_TESTS = 100000;
    
    gettimeofday(&start, NULL);
    for(int i = 0; i < NUM_TESTS; i++) {
        void* ptr = malloc(BLOCK_SIZE);
        free(ptr);
    }
    gettimeofday(&end, NULL);
    double heap_time = (end.tv_sec - start.tv_sec) * 1000000.0 +
                      (end.tv_usec - start.tv_usec);
    
    MemoryPool* pool = create_pool();
    gettimeofday(&start, NULL);
    for(int i = 0; i < NUM_TESTS; i++) {
        void* ptr = pool_alloc(pool);
        pool_free(pool, ptr);
    }
    gettimeofday(&end, NULL);
    double pool_time = (end.tv_sec - start.tv_sec) * 1000000.0 +
                      (end.tv_usec - start.tv_usec);
    
    printf("Standard heap time: %.2f microseconds\n", heap_time / NUM_TESTS);
    printf("Memory pool time: %.2f microseconds\n", pool_time / NUM_TESTS);
    printf("Performance improvement: %.2fx\n", heap_time / pool_time);
    
    destroy_pool(pool);
}

int main() {
    compare_performance();
    return 0;
}